// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "PuerTSToolStyle.h"

class FPuerTSToolCommands : public TCommands<FPuerTSToolCommands>
{
public:

	FPuerTSToolCommands()
		: TCommands<FPuerTSToolCommands>(TEXT("PuerTSTool"), NSLOCTEXT("Contexts", "PuerTSTool", "PuerTSTool Plugin"), NAME_None, FPuerTSToolStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
