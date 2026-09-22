// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Widgets/SCompoundWidget.h"

class SAdvanceDeletionTab : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAdvanceDeletionTab) {}
	SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, AssetDataToStore)
	SLATE_ARGUMENT(FString, CurrentSelectedFolder)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	
private:
	TArray<TSharedPtr<FAssetData>> AssetsToDelete;
	TArray<TSharedPtr<FAssetData>> StoredAssetDataArray;
	TArray<TSharedPtr<FAssetData>> DisplayAssetDataArray;
	TArray<TSharedRef<SCheckBox>> CheckBoxes;
	TSharedPtr<SListView<TSharedPtr<FAssetData>>> AssetListView;
	
#pragma region WidgetConstruction
	
	TSharedRef<SListView<TSharedPtr<FAssetData>>> ConstructAssetListView();
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FAssetData> InItem, const TSharedRef<STableViewBase>& OwnerTable);
	TSharedRef<SCheckBox> ConstructCheckBox(const TSharedPtr<FAssetData>& InItem);
	TSharedRef<STextBlock> ConstructTextBlock(const FString& InText, const FSlateFontInfo& InFontInfo);
	
	void OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> InItem);
	void OnRowWidgetButtonClicked(TSharedPtr<FAssetData> InItem);

#pragma endregion 
	
#pragma region TabButtons
	
	TSharedRef<STextBlock> ConstructTextForButton(const FString& InText);
	
	TSharedRef<SButton> ConstructDeleteButton(const TSharedPtr<FAssetData>& InItem);
	FReply OnDeleteButtonClicked(TSharedPtr<FAssetData> InItem);
	TSharedRef<SButton> ConstructDeleteAllButton();
	FReply OnDeleteAllButtonClicked();
	TSharedRef<SButton> ConstructSelectAllButton();
	FReply OnSelectAllButtonClicked();
	TSharedRef<SButton> ConstructDeselectAllButton();
	FReply OnDeselectAllButtonClicked();
	
#pragma endregion 
	
#pragma region ComboBoxForFiltering
	
	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructComboBox();
	TSharedRef<STextBlock> ConstructComboHelpText(const FString& InText, ETextJustify::Type InJustification);
	TArray<TSharedPtr<FString>> ComboBoxSourceItems;
	TSharedRef<SWidget> OnGenerateComboBoxContent(TSharedPtr<FString> InItem);
	TSharedPtr<STextBlock> ComboBoxTextBlock;
	
	void OnComboBoxSelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);
	
#pragma endregion
	
	void RefreshAssetListView();
	static FSlateFontInfo GetEmbossedTextFont() { return FCoreStyle::Get().GetFontStyle(FName("EmbossedText")); }
};
