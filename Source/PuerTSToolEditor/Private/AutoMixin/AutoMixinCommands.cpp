// Copyright Epic Games, Inc. All Rights Reserved.

#include "AutoMixin/AutoMixinCommands.h"

#define LOCTEXT_NAMESPACE "FPuerTSToolEditorModule"

void FAutoMixinCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "AutoMixin", "Execute AutoMixin action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
