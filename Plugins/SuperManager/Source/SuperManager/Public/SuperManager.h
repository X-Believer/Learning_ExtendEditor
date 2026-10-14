// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class ISceneOutliner;
class ISceneOutlinerColumn;
class UEditorActorSubsystem;

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
	void OnAdvancedDeletionClicked();
	void FixUpRedirectors();
	
	TArray<FString> FolderPathsSelected;
	
#pragma endregion
	
#pragma region CustomEditorTab
	
	void RegisterCustomEditorTab();
	TSharedRef<SDockTab> OnSpawnAdvanceDeletionTab(const FSpawnTabArgs& SpawnTabArgs);
	TArray<TSharedPtr<FAssetData>> GetAllAssetDataUnderSelectedFolder();
	
#pragma endregion
	
#pragma region LevelEditorMenuExtension
	
	void InitLevelEditorMenuExtension();
	
	TSharedRef<FExtender> CustomLevelEditorMenuExtender(const TSharedRef<FUICommandList> CommandList, const TArray<AActor*> SelectedActors);
	
	void AddLevelEditorMenuEntry(FMenuBuilder& MenuBuilder);
	
	void OnLockActorSelectionClicked();
	void OnUnlockActorSelectionClicked();
	
	FDelegateHandle PostEngineInitHandle;

#pragma endregion
	
#pragma region SelectionLock
	
	void InitCustomSelectionEvent();
	void OnActorSelectionChanged(UObject* Object);
	void LockActorSelection(AActor* ActorToLock);
	void UnlockActorSelection(AActor* ActorToUnlock);
	bool IsActorSelectionLocked(AActor* ActorToCheck);
	
	TWeakObjectPtr<UEditorActorSubsystem> EditorActorSubsystem;
	
	bool GetEditorActorSubsystem();
	
#pragma endregion 
	
#pragma region CustomEditorUICommand
	
	TSharedPtr<FUICommandList> CustomUICommands;
	
	void InitCustomUICommands();
	void OnSelectionHotKeyPressed();
	void OnUnlockSelectionHotKeyPressed();
	
#pragma endregion 
	
#pragma region SceneOutlinerColumnExtension
	
	void InitSceneOutlinerColumnExtension();
	
	TSharedRef<ISceneOutlinerColumn> OnCreateSelectionLockColumn(ISceneOutliner& SceneOutliner);
	
#pragma endregion 
public:
#pragma region ProcessDataForAdvanceDeletion

	bool DeleteSingleAssetForAssetList(const FAssetData& AssetDataToDelete);
	bool DeleteMultipleAssetsForAssetList(const TArray<FAssetData>& AssetsToDelete);
	void ListUnusedAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetsToFilter, TArray<TSharedPtr<FAssetData>>& OutUnusedAssets);
	void ListSameNameAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetsToFilter, TArray<TSharedPtr<FAssetData>>& OutSameNameAssets);
	void SyncCBToAssetList(const FString& AssetPath);
	
#pragma endregion
};
