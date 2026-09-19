// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FSuperManagerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
private:
	
#pragma region ContentBrowserMenuExtension
	
	void InitCBMenuExtension();
	
	TSharedRef<FExtender> CustomCBMenuExtender(const TArray<FString>& SelectedPaths);
	void AddCBMenuEntry(FMenuBuilder& MenuBuilder);
	void OnDeleteUnusedAssetsClicked();
	void OnDeleteEmptyFoldersClicked();
	void FixUpRedirectors();
	
	TArray<FString> FolderPathsSelected;
	
#pragma endregion
};
