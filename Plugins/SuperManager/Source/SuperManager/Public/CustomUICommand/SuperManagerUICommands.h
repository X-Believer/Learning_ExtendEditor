// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Framework/Commands/Commands.h"

class FSuperManagerUICommands : public TCommands<FSuperManagerUICommands>
{
public:
	FSuperManagerUICommands() : TCommands<FSuperManagerUICommands>(
		TEXT("SuperManager"), // Context name for fast lookup
		NSLOCTEXT("Contexts", "SuperManager", "SuperManager UI Commands"), // Localized context name for displaying
		NAME_None, // Parent
		TEXT("SuperManager") // Icon Style Set
	){}
	
	TSharedPtr<FUICommandInfo> LockActorSelection;
	TSharedPtr<FUICommandInfo> UnlockActorSelection;
	
	virtual void RegisterCommands() override;
};
