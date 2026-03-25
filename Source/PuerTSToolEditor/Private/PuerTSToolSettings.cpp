#include "PuerTSToolSettings.h"

#include "Blueprint/UserWidget.h"

UPuerTSToolSettings::UPuerTSToolSettings()
{
	// TypeScript defaults
	DefaultTemplateName = TEXT("BpTemplate.ts");
	TemplateDir = TEXT("Template");
	TypeScriptDir = TEXT("TypeScript"); 
	//PuertsFrameworkPath = TEXT("TypeScript"); 
	ImportMixinFileName = TEXT("PreMixin.ts"); 
	if (DoNotOverwritePaths.Num() == 0)
	{
		DoNotOverwritePaths.Add(TEXT("G_App.ts"));
		DoNotOverwritePaths.Add(TEXT("PreMixin.ts"));
	}
	
	if (TemplateMappings.IsEmpty())
	{
		TemplateMappings = {
			{
				UUserWidget::StaticClass(),
				TEXT("UITemplate.ts")
			},
		};
	}
	
	
	PuertsCodeEditorType = EPuertsCodeEditorType::VSCode;
	bOpenCodeEditor = true;
}

FString UPuerTSToolSettings::GetCodeEditorCommand() const
{
	switch (PuertsCodeEditorType)
	{
		case EPuertsCodeEditorType::VSCode:
			return TEXT("code.cmd");

		case EPuertsCodeEditorType::Custom:
			return CustomEditorExecutable.IsEmpty() ? TEXT("code.cmd") : CustomEditorExecutable;

		default:
			return TEXT("code.cmd");
	}
}
