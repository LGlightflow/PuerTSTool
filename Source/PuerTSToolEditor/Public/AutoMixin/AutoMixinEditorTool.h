#pragma once

#include "CoreMinimal.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/Commands/UICommandList.h"

class FAutoMixinEditorTool
{
public:

	FAutoMixinEditorTool();

	virtual ~FAutoMixinEditorTool() = default;

	/** 初始化：绑定命令 */
	virtual void Initialize();

	/** Mixin 按钮执行回调：生成/打开 TS 混入文件并更新mixin.ts集合文件 */
	void GenerateTS(const UBlueprint* Blueprint);

	/** 处理模板内容，替换占位符 */
	FString ProcessTemplate(const FString &TemplateContent, FString BlueprintPath, const FString &FileName, const FString &RootRelativePath);

protected:

	/** 绑定命令（命令到执行代理） */
	virtual void BindCommands();

    
    // TODO:buttonClick统一使用
};