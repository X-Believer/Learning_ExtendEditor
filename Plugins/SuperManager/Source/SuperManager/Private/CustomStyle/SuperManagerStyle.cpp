// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomStyle/SuperManagerStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

FName FSuperManagerStyle::StyleSetName(TEXT("SuperManagerStyle"));
TSharedPtr<FSlateStyleSet> FSuperManagerStyle::CreatedStyleSet = nullptr;

void FSuperManagerStyle::InitializeIcons()
{
	if (!CreatedStyleSet.IsValid())
	{
		CreatedStyleSet = CreateSlateStyleSet();
	}
	FSlateStyleRegistry::RegisterSlateStyle(*CreatedStyleSet);
}

void FSuperManagerStyle::Shutdown()
{
	if (CreatedStyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*CreatedStyleSet);
		CreatedStyleSet.Reset();
	}
}

TSharedRef<FSlateStyleSet> FSuperManagerStyle::CreateSlateStyleSet()
{
	TSharedRef<FSlateStyleSet> StyleSet = MakeShareable(new FSlateStyleSet(StyleSetName));
	StyleSet->SetContentRoot(IPluginManager::Get().FindPlugin(TEXT("SuperManager"))->GetBaseDir() / TEXT("Resources"));
	
	StyleSet->Set(
		TEXT("ContentBrowser.DeleteUnusedAssets"),
		new FSlateImageBrush(
			StyleSet->RootToContentDir(TEXT("DeleteUnusedAsset.png")),
			FVector2D(16.0f, 16.0f)
		)
	);
	
	StyleSet->Set(
		TEXT("ContentBrowser.DeleteEmptyFolders"),
		new FSlateImageBrush(
			StyleSet->RootToContentDir(TEXT("DeleteEmptyFolders.png")),
			FVector2D(16.0f, 16.0f)
		)
	);
	
	StyleSet->Set(
		TEXT("ContentBrowser.AdvanceDeletion"),
		new FSlateImageBrush(
			StyleSet->RootToContentDir(TEXT("AdvanceDeletion.png")),
			FVector2D(16.0f, 16.0f)
		)
	);
	
	return StyleSet;
}
