// Copyright Epic Games, Inc. All Rights Reserved.

#include "PuerTSToolCommands.h"

#define LOCTEXT_NAMESPACE "FPuerTSToolEditorModule"

void FPuerTSToolCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "AutoMixin", "Execute AutoMixin action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
