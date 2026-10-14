// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomUICommand/SuperManagerUICommands.h"

#define LOCTEXT_NAMESPACE "FSuperManagerModule"

void FSuperManagerUICommands::RegisterCommands()
{
	UI_COMMAND(
	LockActorSelection,
	"Lock Actor Selection",
	"Lock the selection of actors in the scene.",
	EUserInterfaceActionType::Button,
	FInputChord(EKeys::W, EModifierKey::Alt)
	);
	
	UI_COMMAND(
	UnlockActorSelection,
	"Unlock Actor Selection",
	"Unlock the selection of actors in the scene.",
	EUserInterfaceActionType::Button,
	FInputChord(EKeys::W, EModifierKey::Alt | EModifierKey::Shift)
	);
}

#undef LOCTEXT_NAMESPACE