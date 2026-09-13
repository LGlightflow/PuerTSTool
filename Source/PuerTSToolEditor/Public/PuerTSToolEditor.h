// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AutoMixin/AutoMixinBPToolBar.h"
#include "AutoMixin/AutoMixinCMToolBar.h"
#include "Modules/ModuleManager.h"

class FPuerTSBridgeServer;

class FPuerTSToolEditorModule : public IModuleInterface
{
public:
	virtual ~FPuerTSToolEditorModule() override;

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	void RegisterSettings();
	void UnregisterSettings();
	
private:
	
	void OnPostEngineInit();
	
	// 部署框架文件
	void DeployPuerTSFramework() const;
	
	TSharedPtr<FAutoMixinBPToolBar> AutoMixinBPToolBar;
	TSharedPtr<FAutoMixinCMToolBar> AutoMixinCMToolBar;
	TUniquePtr<FPuerTSBridgeServer> BridgeServer;
	// 样式
	static TSharedPtr<FSlateStyleSet> StyleSet;
	
};
