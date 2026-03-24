#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PuerTSToolSettings.generated.h"

UENUM()
enum class EPuertsMixinEditorType : uint8
{
	/** Visual Studio Code */
	VSCode,

	/** 其他基于 VSCode 的编辑器（需手动指定可执行文件名） */
	Custom
};
    
UCLASS(config = Editor, defaultconfig, meta = (DisplayName = "PuerTSTool"))
class PUERTSTOOLEDITOR_API UPuerTSToolSettings : public UObject
{
    GENERATED_BODY()

public:
    UPuerTSToolSettings();

    // 获取默认设置实例
    static const UPuerTSToolSettings* Get() { return GetDefault<UPuerTSToolSettings>(); }

	//TODO: 不同的Cpp/蓝图类做对应模板
	UPROPERTY(EditAnywhere, Config, Category = "TypeScript", meta = (DisplayName = "Template File Name"))
	FString TemplateName;

	UPROPERTY(EditAnywhere, Config, Category = "TypeScript", meta = (DisplayName = "Template File Name"))
	FString TemplateDir;
	
	//项目目录的TS文件夹
	UPROPERTY(VisibleAnywhere, Config, Category = "TypeScript", meta = (DisplayName = "TypeScript Directory"))
	FString TypeScriptDir;

	//插件目录的TS文件夹(框架文件夹)
	UPROPERTY(EditAnywhere, Config, Category = "TypeScript", meta = (DisplayName = "Puerts Framework Path"))
	FString PuertsFrameworkPath;

	//import mixin的ts文件
	UPROPERTY(VisibleAnywhere, Config, Category = "TypeScript", meta = (DisplayName = "Import Mixin File Name"))
	FString ImportMixinFileName;
	
	UPROPERTY(EditAnywhere, Config, Category = "TypeScript", meta = (DisplayName = "Overwrite Files When Deploying PuerTS Framework"))
	bool bOverwriteAllTSFilesWhenDeploy = false; 
	

};