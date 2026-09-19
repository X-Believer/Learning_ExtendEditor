// Copyright Epic Games, Inc. All Rights Reserved.

#include "SuperManager.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "HLSLTree/HLSLTreeTypes.h"

#define LOCTEXT_NAMESPACE "FSuperManagerModule"

void FSuperManagerModule::StartupModule()
{
	InitCBMenuExtension();
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

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSuperManagerModule, SuperManager)