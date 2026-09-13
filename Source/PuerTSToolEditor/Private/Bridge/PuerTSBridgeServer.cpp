#include "Bridge/PuerTSBridgeServer.h"

#include "Async/Async.h"
#include "Common/TcpListener.h"
#include "Dom/JsonObject.h"
#include "Editor.h"
#include "Framework/Docking/TabManager.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "Misc/PackageName.h"
#include "PuerTSToolLogChannels.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Widgets/SWindow.h"

FPuerTSBridgeServer::FPuerTSBridgeServer()
{
}

FPuerTSBridgeServer::~FPuerTSBridgeServer()
{
	Stop();
}

bool FPuerTSBridgeServer::Start(const int32 InPort)
{
	if (Listener)
	{
		return true;
	}

	FIPv4Address Address;
	FIPv4Address::Parse(TEXT("127.0.0.1"), Address);
	const FIPv4Endpoint Endpoint(Address, InPort);

	Listener = MakeUnique<FTcpListener>(Endpoint);
	Listener->OnConnectionAccepted().BindRaw(this, &FPuerTSBridgeServer::HandleConnectionAccepted);
	Port = InPort;

	UE_LOG(LogPuerTSToolEditor, Log, TEXT("PuerTS VSCode bridge listening on 127.0.0.1:%d"), Port);
	return true;
}

void FPuerTSBridgeServer::Stop()
{
	if (!Listener)
	{
		return;
	}

	Listener->Stop();
	Listener.Reset();
	UE_LOG(LogPuerTSToolEditor, Log, TEXT("PuerTS VSCode bridge stopped"));
}

bool FPuerTSBridgeServer::HandleConnectionAccepted(FSocket* ClientSocket, const FIPv4Endpoint& ClientEndpoint)
{
	if (!ClientSocket)
	{
		return false;
	}

	Async(EAsyncExecution::ThreadPool, [ClientSocket]()
	{
		HandleClientSocket(ClientSocket);
	});
	return true;
}

void FPuerTSBridgeServer::HandleClientSocket(FSocket* ClientSocket)
{
	FString Payload;
	if (!TryReadSocketPayload(ClientSocket, Payload))
	{
		SendSocketResponse(ClientSocket, MakeResponse(false, TEXT("empty request")));
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ClientSocket);
		return;
	}

	SendSocketResponse(ClientSocket, HandleCommand(Payload));
	ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ClientSocket);
}

FString FPuerTSBridgeServer::HandleCommand(const FString& Payload)
{
	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Payload);
	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		return MakeResponse(false, TEXT("invalid json"));
	}

	FString Command;
	if (!JsonObject->TryGetStringField(TEXT("cmd"), Command))
	{
		return MakeResponse(false, TEXT("missing cmd"));
	}

	if (Command.Equals(TEXT("ping"), ESearchCase::IgnoreCase))
	{
		return MakeResponse(true, TEXT("pong"));
	}

	if (!Command.Equals(TEXT("openAsset"), ESearchCase::IgnoreCase))
	{
		return MakeResponse(false, FString::Printf(TEXT("unsupported cmd: %s"), *Command));
	}

	FString AssetPath;
	if (!JsonObject->TryGetStringField(TEXT("assetPath"), AssetPath) || AssetPath.IsEmpty())
	{
		return MakeResponse(false, TEXT("missing assetPath"));
	}

	FString Mode;
	if (!JsonObject->TryGetStringField(TEXT("mode"), Mode) || Mode.IsEmpty())
	{
		Mode = TEXT("open");
	}

	OpenAssetOnGameThread(AssetPath, Mode);
	return MakeResponse(true, TEXT("asset open request accepted"));
}

void FPuerTSBridgeServer::OpenAssetOnGameThread(const FString& AssetPath, const FString& Mode)
{
	AsyncTask(ENamedThreads::GameThread, [AssetPath = NormalizeBlueprintAssetPath(AssetPath), Mode]()
	{
		if (!GEditor)
		{
			UE_LOG(LogPuerTSToolEditor, Warning, TEXT("PuerTS bridge cannot open asset because GEditor is null"));
			return;
		}

		if (!FPackageName::IsValidObjectPath(AssetPath))
		{
			UE_LOG(LogPuerTSToolEditor, Warning, TEXT("PuerTS bridge invalid asset path: %s"), *AssetPath);
			return;
		}

		FString PackagePath;
		FString AssetName;
		if (!AssetPath.Split(TEXT("."), &PackagePath, &AssetName) || PackagePath.IsEmpty() || AssetName.IsEmpty())
		{
			UE_LOG(LogPuerTSToolEditor, Warning, TEXT("PuerTS bridge cannot split asset path: %s"), *AssetPath);
			return;
		}

		UPackage* Package = LoadPackage(nullptr, *PackagePath, LOAD_NoRedirects);
		UObject* Asset = nullptr;
		if (Package)
		{
			Package->FullyLoad();
			Asset = FindObject<UObject>(Package, *AssetName);
		}

		if (!Asset)
		{
			Asset = LoadObject<UObject>(nullptr, *AssetPath);
		}

		if (!Asset)
		{
			UE_LOG(LogPuerTSToolEditor, Warning, TEXT("PuerTS bridge asset not found: %s"), *AssetPath);
			return;
		}

		const TSharedPtr<SWindow> RootWindow = FGlobalTabmanager::Get()->GetRootWindow();
		if (RootWindow.IsValid())
		{
			if (RootWindow->IsWindowMinimized())
			{
				RootWindow->Restore();
			}
			else
			{
				RootWindow->HACK_ForceToFront();
			}
		}

		if (Mode.Equals(TEXT("reveal"), ESearchCase::IgnoreCase))
		{
			TArray<UObject*> Assets;
			Assets.Add(Asset);
			GEditor->SyncBrowserToObjects(Assets);
			return;
		}

		if (UAssetEditorSubsystem* EditorAssetSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
		{
			EditorAssetSubsystem->OpenEditorForAsset(Asset);
		}
	});
}

FString FPuerTSBridgeServer::NormalizeBlueprintAssetPath(FString AssetPath)
{
	AssetPath.TrimStartAndEndInline();
	AssetPath.TrimQuotesInline();
	AssetPath.ReplaceInline(TEXT("\\"), TEXT("/"));
	AssetPath.RemoveFromStart(TEXT("BlueprintGeneratedClass'"));
	AssetPath.RemoveFromStart(TEXT("Blueprint'"));
	AssetPath.RemoveFromStart(TEXT("Class'"));
	AssetPath.RemoveFromEnd(TEXT("'"));

	if (AssetPath.EndsWith(TEXT("_C")))
	{
		AssetPath.LeftChopInline(2);
	}

	return AssetPath;
}

bool FPuerTSBridgeServer::TryReadSocketPayload(FSocket* ClientSocket, FString& OutPayload)
{
	if (!ClientSocket)
	{
		return false;
	}

	TArray<uint8> Bytes;
	double StartTime = FPlatformTime::Seconds();
	while (FPlatformTime::Seconds() - StartTime < 2.0)
	{
		if (!ClientSocket->Wait(ESocketWaitConditions::WaitForRead, FTimespan::FromMilliseconds(100)))
		{
			continue;
		}

		uint32 PendingDataSize = 0;
		while (ClientSocket->HasPendingData(PendingDataSize))
		{
			TArray<uint8> Buffer;
			Buffer.SetNumUninitialized(FMath::Min(PendingDataSize, 65507u));

			int32 BytesRead = 0;
			if (!ClientSocket->Recv(Buffer.GetData(), Buffer.Num(), BytesRead) || BytesRead <= 0)
			{
				break;
			}

			Buffer.SetNum(BytesRead);
			Bytes.Append(Buffer);
			StartTime = FPlatformTime::Seconds();

			if (Bytes.Contains(static_cast<uint8>('\n')))
			{
				break;
			}
		}

		if (Bytes.Contains(static_cast<uint8>('\n')))
		{
			break;
		}
	}

	if (Bytes.IsEmpty())
	{
		return false;
	}

	const FUTF8ToTCHAR Converter(reinterpret_cast<const ANSICHAR*>(Bytes.GetData()), Bytes.Num());
	OutPayload = FString(Converter.Length(), Converter.Get());
	OutPayload.TrimStartAndEndInline();
	return !OutPayload.IsEmpty();
}

void FPuerTSBridgeServer::SendSocketResponse(FSocket* ClientSocket, const FString& Response)
{
	if (!ClientSocket)
	{
		return;
	}

	const FString ResponseLine = Response + TEXT("\n");
	const FTCHARToUTF8 Converter(*ResponseLine);
	int32 BytesSent = 0;
	ClientSocket->Send(reinterpret_cast<const uint8*>(Converter.Get()), Converter.Length(), BytesSent);
}

FString FPuerTSBridgeServer::MakeResponse(const bool bOk, const FString& Message)
{
	const TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetBoolField(TEXT("ok"), bOk);
	JsonObject->SetStringField(TEXT("message"), Message);

	FString Output;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);
	FJsonSerializer::Serialize(JsonObject, Writer);
	return Output;
}
