// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/QuickAssetAction.h"

#include "AssetToolsModule.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Materials/MaterialInstanceConstant.h"

void UQuickAssetAction::DuplicateAsset(int32 NumOfDuplicates)
{
	if (NumOfDuplicates <= 0)
	{
		ShowMessageDialog(EAppMsgType::Ok, TEXT("Number of duplicates must be greater than 0."), true);
		return;
	}
	
	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	uint32 Counter = 1;
	
	for (const FAssetData& AssetData : SelectedAssetsData)
	{
		FString AssetPath = AssetData.GetObjectPathString();
		FString AssetName = AssetData.AssetName.ToString();
		FString AssetPackagePath = AssetData.PackagePath.ToString();

		for (int32 i = 0; i < NumOfDuplicates; ++i)
		{
			FString NewAssetName = FString::Printf(TEXT("%s_%d"), *AssetName, Counter++);
			FString NewAssetPath = FPaths::Combine(AssetPackagePath, *NewAssetName);

			if (UEditorAssetLibrary::DuplicateAsset(AssetPath, NewAssetPath))
			{
				UEditorAssetLibrary::SaveAsset(NewAssetPath, false);
				ShowNotifyInfo(FString::Printf(TEXT("Duplicated asset: %s to %s"), *AssetPath, *NewAssetPath));
			}
			else
			{
				Print(FString::Printf(TEXT("Failed to duplicate asset: %s"), *AssetPath), FColor::Red);
			}
		}
	}
}

void UQuickAssetAction::AddPrefixes()
{
	TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
	uint32 Counter = 0;
	
	for (UObject* Object : SelectedAssets)
	{
		if (!Object) continue;
		
		FString* PrefixFound = PrefixMap.Find(Object->GetClass());
		if (!PrefixFound || PrefixFound->IsEmpty())
		{
			Print(FString::Printf(TEXT("No prefix defined for class: %s"), *Object->GetClass()->GetName()), FColor::Yellow);
			continue;
		}
		
		FString OldName = Object->GetName();
		if (OldName.StartsWith(*PrefixFound))
		{
			Print(FString::Printf(TEXT("Asset %s already has the prefix %s"), *OldName, **PrefixFound), FColor::Yellow);
			continue;
		}
		if (Object->IsA<UMaterialInstanceConstant>())
		{
			OldName.RemoveFromStart(TEXT("M_"));
			OldName.RemoveFromEnd(TEXT("_Inst"));
		}
		
		FString NewName = FString::Printf(TEXT("%s%s"), **PrefixFound, *OldName);
		UEditorUtilityLibrary::RenameAsset(Object, NewName);
		Counter++;
	}
	
	ShowNotifyInfo(FString::Printf(TEXT("Added prefixes to %d assets."), Counter));
}

void UQuickAssetAction::RemoveUnusedAssets()
{
	TArray<FAssetData> SelectedDataAssets =  UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<FAssetData> UnusedAssets;
	
	FixUpRedirectors();
	
	for (const FAssetData& AssetData : SelectedDataAssets)
	{
		TArray<FString> Referencers =
		UEditorAssetLibrary::FindPackageReferencersForAsset(AssetData.ObjectPath.ToString());
		
		if (Referencers.Num() == 0)
		{
			UnusedAssets.Add(AssetData);
		}
	}
	
	if (UnusedAssets.Num() == 0)
	{
		ShowMessageDialog(EAppMsgType::Ok, TEXT("No unused assets found."), false);
		return;
	}
	
	int32 NumDeleted = ObjectTools::DeleteAssets(UnusedAssets);
	
	if (NumDeleted == 0) return;;
	
	ShowNotifyInfo(FString::Printf(TEXT("Deleted %d unused assets."), NumDeleted));
}

void UQuickAssetAction::FixUpRedirectors()
{
	TArray<UObjectRedirector*> RedirectorsToFix;
	
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Emplace(FName("/Game"));
	Filter.ClassNames.Emplace(FName("ObjectRedirector"));
	
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
