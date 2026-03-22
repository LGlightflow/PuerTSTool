// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FPuerTSToolEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
private:
	// 添加mixin文件
	void AddMixinFile() const;

	// 按照AddMixinFile 的方式部署框架文件，用于代替AddMixinFile 的功能
	void DeployPuerTSFramework() const;

	// 注册按键
	void RegistrationButton() const;
	// 按键按下
	static void ButtonPressed();
	// 获取当前激活的Blueprint
	static UBlueprint* GetActiveBlueprint();

	// 在内容浏览器右键注册一个按键
	void RegistrationContextButton() const;

	/**
	 * @brief 菜单按钮按下
	 * @param SelectedAssets	选中的资产 
	 */
	static void ContextButtonPressed(const TArray<FAssetData>& SelectedAssets);

	// 生成ts文件
	static void GenerateTs(const UBlueprint* Blueprint);

	/**
	 * 处理模板
	 * @param TemplateContent	模板内容 
	 * @param BlueprintPath		蓝图路径路径
	 * @param FileName			文件名
	 * @param RootRelativePath	相对脚本根目录路径（用于替换ROOT_PATH）
	 * @return 
	 */
	static FString ProcessTemplate(const FString& TemplateContent, FString BlueprintPath, const FString& FileName, const FString& RootRelativePath);
	

	// 样式
	static TSharedPtr<FSlateStyleSet> StyleSet;

	// mixin文件路径
	const FString MixinPath = FPaths::Combine(FPaths::ProjectDir(),TEXT("TypeScript"),TEXT("mixin.ts"));
};
