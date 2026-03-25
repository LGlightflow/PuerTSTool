// Copyright Epic Games, Inc. All Rights Reserved.

#include "PuerTSToolEditor.h"

#include "ISettingsModule.h"
#include "PuerTSToolSettings.h"
#include "AutoMixin/AutoMixinBPToolBar.h"
#include "AutoMixin/AutoMixinCMToolBar.h"
#include "PuerTSToolStyle.h"
#include "PuerTSToolCommands.h"


#define LOCTEXT_NAMESPACE "FPuerTSToolEditorModule"



TSharedPtr<FSlateStyleSet> FPuerTSToolEditorModule::StyleSet = nullptr;


void FPuerTSToolEditorModule::StartupModule()
{

	FPuerTSToolStyle::Initialize();
	FPuerTSToolStyle::ReloadTextures();
	FPuerTSToolCommands::Register();

	RegisterSettings();
	
	DeployPuerTSFramework();
	
	FCoreDelegates::OnPostEngineInit.AddRaw(this,&FPuerTSToolEditorModule::OnPostEngineInit);
	
	AutoMixinBPToolBar = MakeShared<FAutoMixinBPToolBar>();
	AutoMixinCMToolBar = MakeShared<FAutoMixinCMToolBar>();
}




void FPuerTSToolEditorModule::DeployPuerTSFramework() const
{
	const UPuerTSToolSettings* Settings = GetDefault<UPuerTSToolSettings>();
	
	// 插件内 TypeScript 框架目录
	const FString SourceDir = FPaths::Combine(
		FPaths::ProjectPluginsDir(),
		Settings->PuertsFrameworkPath
	);

	// 项目目录下 TypeScript
	const FString TargetDir = FPaths::Combine(
		FPaths::ProjectDir(),
		Settings->TypeScriptDir
	);

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// 1️ 确保目标目录存在
	if (!PlatformFile.DirectoryExists(*TargetDir))
	{
		PlatformFile.CreateDirectoryTree(*TargetDir);
	}

	// 2️ 递归复制目录
	const bool bOverwrite = Settings->bOverwriteAllTSFilesWhenDeploy; // 是否覆盖已有文件

	bool bSuccess = PlatformFile.CopyDirectoryTree(
		*TargetDir,
		*SourceDir,
		bOverwrite
	);

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Deploy PuerTS Framework Success:\n%s -> %s"), *SourceDir, *TargetDir);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Deploy PuerTS Framework Failed:\n%s -> %s"), *SourceDir, *TargetDir);
	}
}



void FPuerTSToolEditorModule::ShutdownModule()
{
	UnregisterSettings();
	
	FCoreDelegates::OnPostEngineInit.RemoveAll(this);
	
	AutoMixinBPToolBar->Uninitialize();
	AutoMixinCMToolBar->Uninitialize();
	
	FPuerTSToolStyle::Shutdown();
	FPuerTSToolCommands::Unregister();
}

void FPuerTSToolEditorModule::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings(
			"Project", "Plugins", "PuerTSTool",
			LOCTEXT("PuerTSToolSettingsName", "PuerTSTool"),
			LOCTEXT("PuerTSToolSettingsDescription", "Configure PuerTSTool Plugin Settings"),
			GetMutableDefault<UPuerTSToolSettings>()
		);
	}
}

void FPuerTSToolEditorModule::UnregisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "PuerTSTool");
	}
}

void FPuerTSToolEditorModule::OnPostEngineInit()
{
	if (!GEditor)
	{
		return;
	}

	// 引擎初始化完成后再初始化工具栏，此时可以安全加载 Kismet 模块
	if (AutoMixinBPToolBar.IsValid())
	{
		AutoMixinBPToolBar->Initialize();
	}
	
	if (AutoMixinCMToolBar.IsValid())
	{
		AutoMixinCMToolBar->Initialize();
	}

	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPuerTSToolEditorModule, PuerTSToolEditor)