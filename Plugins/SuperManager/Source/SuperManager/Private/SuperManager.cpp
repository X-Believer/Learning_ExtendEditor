// Copyright Epic Games, Inc. All Rights Reserved.

#include "SuperManager.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "HLSLTree/HLSLTreeTypes.h"
#include "SlateWidgets/AdvanceDeletionWidget.h"

#define LOCTEXT_NAMESPACE "FSuperManagerModule"

void FSuperManagerModule::StartupModule()
{
	InitCBMenuExtension();
	RegisterCustomEditorTab();
}

void FSuperManagerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#pragma region ContentBrowserMenuExtension

void FSuperManagerModule::InitCBMenuExtension()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	
	TArray<FContentBrowserMenuExtender_SelectedPaths>& ContentBrowserModuleExtenders =  ContentBrowserModule.GetAllPathViewContextMenuExtenders();
	FContentBrowserMenuExtender_SelectedPaths CustomCBMenuDelegate;
	
	ContentBrowserModuleExtenders.Add(FContentBrowserMenuExtender_SelectedPaths::CreateRaw(this, &FSuperManagerModule::CustomCBMenuExtender));
}

TSharedRef<FExtender> FSuperManagerModule::CustomCBMenuExtender(const TArray<FString>& SelectedPaths)
{
	TSharedRef<FExtender> MenuExtender = MakeShared<FExtender>();
	if (SelectedPaths.Num() > 0)
	{
		MenuExtender->AddMenuExtension(
			FName("Delete"),
			EExtensionHook::After,
			TSharedPtr<FUICommandList>(),
			FMenuExtensionDelegate::CreateRaw(this, &FSuperManagerModule::AddCBMenuEntry)
		);
		FolderPathsSelected = SelectedPaths;
	}
	
	return MenuExtender;
}

void FSuperManagerModule::AddCBMenuEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		LOCTEXT("DeleteUnusedAssets", "Delete Unused Assets"),
		LOCTEXT("DeleteUnusedAssets_Tooltip", "Safely delete all unused assets under folder"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnDeleteUnusedAssetsClicked))
	);
	
	MenuBuilder.AddMenuEntry(
		LOCTEXT("DeleteEmptyFolders", "Delete Empty Folders"),
		LOCTEXT("DeleteEmptyFolders_Tooltip", "Delete all empty folders under the selected folder"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnDeleteEmptyFoldersClicked))
	);
	
	MenuBuilder.AddMenuEntry(
		LOCTEXT("AdvancedDeletion", "Advanced Deletion"),
		LOCTEXT("AdvancedDeletion_Tooltip", "Perform advanced deletion operations"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnAdvancedDeletionClicked))
	);
}

void FSuperManagerModule::OnDeleteUnusedAssetsClicked()
{
	if (FolderPathsSelected.Num() > 1)
	{
		DebugHelper::ShowMessageDialog(EAppMsgType::Ok, TEXT("Please select only one folder."), true);
		return;
	}
	
	TArray<FString> AssetsPathNames = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0], /*bRecursive=*/true, /*bIncludeFolder=*/false);
	
	if (AssetsPathNames.Num() == 0)
	{
		DebugHelper::ShowMessageDialog(EAppMsgType::Ok, TEXT("No assets found in the selected folder."), true);
		return;
	}
	
	EAppReturnType::Type ConfirmResult = DebugHelper::ShowMessageDialog(EAppMsgType::YesNo, TEXT("This will delete all unused assets under the selected folder. Are you sure?"), true);
	if (ConfirmResult == EAppReturnType::No) return;
	
	FixUpRedirectors();
	
	TArray<FAssetData> UnusedAssets;
	for (FString Asset : AssetsPathNames)
	{
		if (AssetsPathNames.Contains(TEXT("Developers")) || AssetsPathNames.Contains(TEXT("Collections")) || AssetsPathNames.Contains(TEXT("Plugins")) || AssetsPathNames.Contains(TEXT("Engine")) || AssetsPathNames.Contains(TEXT("__ExternalActors__")) || AssetsPathNames.Contains(TEXT("__ExternalObjects__")))
		{
			continue;
		}
		
		if (!UEditorAssetLibrary::DoesAssetExist(Asset)) continue;
		
		TArray<FString> Referencers = UEditorAssetLibrary::FindPackageReferencersForAsset(Asset);
		if (Referencers.Num() == 0)
		{
			const FAssetData UnusedAssetData = UEditorAssetLibrary::FindAssetData(Asset);
			UnusedAssets.Add(UnusedAssetData);
		}
	}
	if (UnusedAssets.Num() > 0)
	{
		ObjectTools::DeleteAssets(UnusedAssets);
	}
	else
	{
		DebugHelper::ShowMessageDialog(EAppMsgType::Ok, TEXT("No unused assets found in the selected folder."), false);
	}
}

void FSuperManagerModule::OnDeleteEmptyFoldersClicked()
{
	FixUpRedirectors();
	
	TArray<FString> FolderPaths = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0], true, true);
	uint32 Counter = 0;
	
	FString EmptyFolderPathNames;
	TArray<FString> EmptyFoldersPaths;
	
	for (FString Folder : FolderPaths)
	{
		if (Folder.Contains(TEXT("Developers")) || Folder.Contains(TEXT("Collections")) || Folder.Contains(TEXT("Plugins")) || Folder.Contains(TEXT("Engine")) || Folder.Contains(TEXT("__ExternalActors__")) || Folder.Contains(TEXT("__ExternalObjects__")))
		{
			continue;
		}
		
		if (!UEditorAssetLibrary::DoesDirectoryExist(Folder)) continue;
		if (!UEditorAssetLibrary::DoesDirectoryHaveAssets(Folder))
		{
			EmptyFolderPathNames.Append(Folder + TEXT("\n"));
			EmptyFoldersPaths.Add(Folder);
		}
	}
	
	if (EmptyFoldersPaths.Num() == 0)
	{
		DebugHelper::ShowMessageDialog(EAppMsgType::Ok, TEXT("No empty folders found in the selected folder."), false);
		return;
	}
		
	EAppReturnType::Type Result = DebugHelper::ShowMessageDialog(EAppMsgType::YesNo, FString::Printf(TEXT("The following empty folders will be deleted:\n%s\nAre you sure?"), *EmptyFolderPathNames), true);
	if (Result == EAppReturnType::No) return;
		
	for (FString EmptyFolder : EmptyFoldersPaths)
	{
		if (UEditorAssetLibrary::DeleteDirectory(EmptyFolder))
		{
			Counter++;
		}
		else
		{
			DebugHelper::Print(FString::Printf(TEXT("Failed to delete folder: %s"), *EmptyFolder), FColor::Red);
		}
	}
	DebugHelper::ShowMessageDialog(EAppMsgType::Ok, FString::Printf(TEXT("Deleted %d empty folders:\n%s"), Counter, *EmptyFolderPathNames), false);

}

void FSuperManagerModule::OnAdvancedDeletionClicked()
{
	FixUpRedirectors();
	FGlobalTabmanager::Get()->TryInvokeTab(FName("AdvanceDeletion"));
}

void FSuperManagerModule::FixUpRedirectors()
{
	TArray<UObjectRedirector*> RedirectorsToFix;
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	
	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Emplace(FName("/Game"));
	Filter.ClassPaths.Add(UObjectRedirector::StaticClass()->GetClassPathName());
	
	TArray<FAssetData> OutRedirectors;
	AssetRegistryModule.Get().GetAssets(Filter, OutRedirectors);
	
	for (const FAssetData& RedirectorData : OutRedirectors)
	{
		UObjectRedirector* Redirector = Cast<UObjectRedirector>(RedirectorData.GetAsset());
		if (Redirector)
		{
			RedirectorsToFix.Add(Redirector);
		}
	}
	
	FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get().FixupReferencers(RedirectorsToFix);
}

#pragma endregion

#pragma region CustomEditorTab

void FSuperManagerModule::RegisterCustomEditorTab()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner("AdvanceDeletion", FOnSpawnTab::CreateRaw(this, &FSuperManagerModule::OnSpawnAdvanceDeletionTab))
		.SetDisplayName(LOCTEXT("AdvanceDeletionTabTitle", "Advance Deletion"));
}

TSharedRef<SDockTab> FSuperManagerModule::OnSpawnAdvanceDeletionTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SAdvanceDeletionTab)
			.AssetDataToStore(GetAllAssetDataUnderSelectedFolder())
			.CurrentSelectedFolder(FolderPathsSelected.Num() > 0 ? FolderPathsSelected[0] : FString())
		];
}

TArray<TSharedPtr<FAssetData>> FSuperManagerModule::GetAllAssetDataUnderSelectedFolder()
{
	TArray<TSharedPtr<FAssetData>> AvailableAssetData;
	
	TArray<FString> AssetsPathNames = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0]);
	
	for (FString Path : AssetsPathNames)
	{
		if (Path.Contains(TEXT("Developers")) || Path.Contains(TEXT("Collections")) || Path.Contains(TEXT("Plugins")) || Path.Contains(TEXT("Engine")) || Path.Contains(TEXT("__ExternalActors__")) || Path.Contains(TEXT("__ExternalObjects__")))
		{
			continue;
		}
		
		if (!UEditorAssetLibrary::DoesAssetExist(Path)) continue;
		const FAssetData AssetData = UEditorAssetLibrary::FindAssetData(Path);
		AvailableAssetData.Add(MakeShared<FAssetData>(AssetData));
	}
	return AvailableAssetData;
}


#pragma endregion

#pragma region ProcessDataForAdvanceDeletion

bool FSuperManagerModule::DeleteSingleAssetForAssetList(const FAssetData& AssetDataToDelete)
{
	TArray<FAssetData> AssetsToDelete;
	AssetsToDelete.Add(AssetDataToDelete);
	
	if (ObjectTools::DeleteAssets(AssetsToDelete) > 0)
	{
		return true;
	}
	return false;
}

bool FSuperManagerModule::DeleteMultipleAssetsForAssetList(const TArray<FAssetData>& AssetsToDelete)
{
	if (ObjectTools::DeleteAssets(AssetsToDelete) > 0)
	{
		return true;
	}
	return false;
}

void FSuperManagerModule::ListUnusedAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetsToFilter, TArray<TSharedPtr<FAssetData>>& OutUnusedAssets)
{
	OutUnusedAssets.Empty();
	
	for (const TSharedPtr<FAssetData>& Asset : AssetsToFilter)
	{
		TArray<FString> Referencers = UEditorAssetLibrary::FindPackageReferencersForAsset(Asset->GetObjectPathString());
		if (Referencers.Num() == 0)
		{
			OutUnusedAssets.Add(Asset);
		}
	}
}

void FSuperManagerModule::ListSameNameAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetsToFilter, TArray<TSharedPtr<FAssetData>>& OutSameNameAssets)
{
	OutSameNameAssets.Empty();
	
	TMultiMap<FString, TSharedPtr<FAssetData>> AssetsInfo;
	
	for (const TSharedPtr<FAssetData>& Asset : AssetsToFilter)
	{
		AssetsInfo.Emplace(Asset->AssetName.ToString(), Asset);
	}
	
	for (const TSharedPtr<FAssetData>& Asset : AssetsToFilter)
	{
		TArray<TSharedPtr<FAssetData>> SameNameAssets;
		AssetsInfo.MultiFind(Asset->AssetName.ToString(), SameNameAssets);
		
		if (SameNameAssets.Num() <= 1) continue;
		
		for (const TSharedPtr<FAssetData>& SameNameAsset : SameNameAssets)
		{
			if (SameNameAsset.IsValid())
			{
				OutSameNameAssets.AddUnique(SameNameAsset);
			}
		}
	}
}

void FSuperManagerModule::SyncCBToAssetList(const FString& AssetPath)
{
	TArray<FString> AssetPathsToSync;
	AssetPathsToSync.Add(AssetPath);
	UEditorAssetLibrary::SyncBrowserToObjects(AssetPathsToSync);
}

#pragma endregion 

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSuperManagerModule, SuperManager)