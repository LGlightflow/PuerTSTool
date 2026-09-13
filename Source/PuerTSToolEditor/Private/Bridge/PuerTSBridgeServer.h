#pragma once

#include "CoreMinimal.h"

class FSocket;
class FTcpListener;
struct FIPv4Endpoint;

class FPuerTSBridgeServer
{
public:
	FPuerTSBridgeServer();
	~FPuerTSBridgeServer();

	bool Start(int32 InPort);
	void Stop();

private:
	bool HandleConnectionAccepted(FSocket* ClientSocket, const FIPv4Endpoint& ClientEndpoint);
	static void HandleClientSocket(FSocket* ClientSocket);
	static FString HandleCommand(const FString& Payload);
	static void OpenAssetOnGameThread(const FString& AssetPath, const FString& Mode);

	static FString NormalizeBlueprintAssetPath(FString AssetPath);
	static bool TryReadSocketPayload(FSocket* ClientSocket, FString& OutPayload);
	static void SendSocketResponse(FSocket* ClientSocket, const FString& Response);
	static FString MakeResponse(bool bOk, const FString& Message);

	TUniquePtr<FTcpListener> Listener;
	int32 Port = 0;
};
