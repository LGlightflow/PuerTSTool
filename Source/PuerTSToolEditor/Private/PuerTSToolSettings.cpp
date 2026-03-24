#include "PuerTSToolSettings.h"

UPuerTSToolSettings::UPuerTSToolSettings()
{
	// TypeScript defaults
	TemplateName = TEXT("BpTemplate.ts");
	TemplateDir = TEXT("Framework/Template");
	TypeScriptDir = TEXT("TypeScript"); 
	PuertsFrameworkPath = TEXT("PuerTSTool/TypeScript"); 
	ImportMixinFileName = TEXT("PreMixin.ts"); 
	bOverwriteAllTSFilesWhenDeploy = false;
}