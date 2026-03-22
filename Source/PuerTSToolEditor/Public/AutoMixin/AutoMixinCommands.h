// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "AutoMixin/AutoMixinStyle.h"

class FAutoMixinCommands : public TCommands<FAutoMixinCommands>
{
public:

	FAutoMixinCommands()
		: TCommands<FAutoMixinCommands>(TEXT("AutoMixin"), NSLOCTEXT("Contexts", "AutoMixin", "AutoMixin Plugin"), NAME_None, FAutoMixinStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
