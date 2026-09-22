// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidgets/AdvanceDeletionWidget.h"
#include "DebugHeader.h"
#include "SuperManager.h"

#define ListAll TEXT("List All Available Assets")
#define ListUnused TEXT("List Unused Assets")
#define ListSameName TEXT("List Assets With Same Name")

void SAdvanceDeletionTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;
	
	StoredAssetDataArray = InArgs._AssetDataToStore;
	DisplayAssetDataArray = StoredAssetDataArray;
	
	CheckBoxes.Empty();
	AssetsToDelete.Empty();
	ComboBoxSourceItems.Empty();
	
	ComboBoxSourceItems.Add(MakeShared<FString>(ListAll));
	ComboBoxSourceItems.Add(MakeShared<FString>(ListUnused));
	ComboBoxSourceItems.Add(MakeShared<FString>(ListSameName));
	
	FSlateFontInfo TitleFont = GetEmbossedTextFont();
	TitleFont.Size = 30;
	
	ChildSlot
	[
		// Main VerticalBox
		SNew(SVerticalBox)

		// Title
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Advance Deletion")))
			.Font(TitleFont)
			.ColorAndOpacity(FLinearColor::White)
			.Justification(ETextJustify::Center)
		]

		// Dropdown
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			// Combo Box
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				ConstructComboBox()
			]

			// Help Text
			+SHorizontalBox::Slot()
			.FillWidth(.6f)
			[
				ConstructComboHelpText(TEXT("Select an option to filter the asset list. Left mouse click to go to where asset is located."), ETextJustify::Center)
			]

			+SHorizontalBox::Slot()
			.FillWidth(.6f)
			[
				ConstructComboHelpText(TEXT("Current Folder:\n") + InArgs._CurrentSelectedFolder, ETextJustify::Right)
			]
		]

		// Asset List
		+SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		[
			SNew(SScrollBox)

			+SScrollBox::Slot()
			[
				ConstructAssetListView()
			]
		]

		// Buttons
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			// Button slot
			+SHorizontalBox::Slot()
			.FillWidth(10.f)
			.Padding(5.f)
			[
				ConstructDeleteAllButton()
			]

			+SHorizontalBox::Slot()
			.FillWidth(10.f)
			.Padding(5.f)
			[
				ConstructSelectAllButton()
			]

			+SHorizontalBox::Slot()
			.FillWidth(10.f)
			.Padding(5.f)
			[
				ConstructDeselectAllButton()
			]
		]
	];
	
}

#pragma region WidgetConstruction

TSharedRef<SListView<TSharedPtr<FAssetData>>> SAdvanceDeletionTab::ConstructAssetListView()
{
	AssetListView = SNew(SListView<TSharedPtr<FAssetData>>)
	.ItemHeight(24.0f)
	.ListItemsSource(&DisplayAssetDataArray)
	.OnGenerateRow(this, &SAdvanceDeletionTab::OnGenerateRow)
	.OnMouseButtonClick(this, &SAdvanceDeletionTab::OnRowWidgetButtonClicked);
	return AssetListView.ToSharedRef();
}

TSharedRef<ITableRow> SAdvanceDeletionTab::OnGenerateRow(TSharedPtr<FAssetData> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (!InItem.IsValid()) return SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable);
	
	const FString AssetName = InItem->AssetName.ToString();
	const FString AssetClass = InItem->AssetClassPath.GetAssetName().ToString();
	
	FSlateFontInfo AssetClassFont = GetEmbossedTextFont();
	AssetClassFont.Size = 10;
	FSlateFontInfo AssetNameFont = GetEmbossedTextFont();
	AssetNameFont.Size = 15;
	
	
	TSharedRef<STableRow<TSharedPtr<FAssetData>>> TableRow = SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable)
		.Padding(FMargin(5.0f))
	[
		SNew(SHorizontalBox)
		// Check box
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.FillWidth(0.05f)
		[
			ConstructCheckBox(InItem)
		]

		// Asset class name
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Fill)
		.FillWidth(0.5f)
		[
			ConstructTextBlock(AssetClass, AssetClassFont)
		]

		// Asset name
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Fill)
		[
			ConstructTextBlock(AssetName, AssetNameFont)
		]
		
		// Button
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Fill)
		[
			ConstructDeleteButton(InItem)
		]
	];
	return TableRow;
}

TSharedRef<SCheckBox> SAdvanceDeletionTab::ConstructCheckBox(const TSharedPtr<FAssetData>& InItem)
{
	TSharedRef<SCheckBox> CheckBox = SNew(SCheckBox)
	.Type(ESlateCheckBoxType::CheckBox)
	.OnCheckStateChanged(this, &SAdvanceDeletionTab::OnCheckBoxStateChanged, InItem)
	.Visibility(EVisibility::Visible);
	
	CheckBoxes.Add(CheckBox);
	return CheckBox;
}

TSharedRef<STextBlock> SAdvanceDeletionTab::ConstructTextBlock(const FString& InText, const FSlateFontInfo& InFontInfo)
{
	TSharedRef<STextBlock> TextBlock = SNew(STextBlock)
	.Text(FText::FromString(InText))
	.Font(InFontInfo)
	.ColorAndOpacity(FLinearColor::White);
	
	return TextBlock;
}

#pragma endregion 

#pragma region TabButtons

TSharedRef<STextBlock> SAdvanceDeletionTab::ConstructTextForButton(const FString& InText)
{
	FSlateFontInfo ButtonFont = GetEmbossedTextFont();
	ButtonFont.Size = 15;
	
	TSharedRef<STextBlock> TextBlock = SNew(STextBlock)
	.Text(FText::FromString(InText))
	.Font(ButtonFont)
	.ColorAndOpacity(FLinearColor::White)
	.Justification(ETextJustify::Center);
	
	return TextBlock;
}

TSharedRef<SButton> SAdvanceDeletionTab::ConstructDeleteButton(const TSharedPtr<FAssetData>& InItem)
{
	TSharedRef<SButton> Button = SNew(SButton)
	.Text(FText::FromString("Delete"))
	.OnClicked(this, &SAdvanceDeletionTab::OnDeleteButtonClicked, InItem);
	
	return Button;
}

TSharedRef<SButton> SAdvanceDeletionTab::ConstructDeleteAllButton()
{
	TSharedRef<SButton> Button = SNew(SButton)
	.ContentPadding(5.f)
	.OnClicked(this, &SAdvanceDeletionTab::OnDeleteAllButtonClicked);
	
	Button->SetContent(ConstructTextForButton(TEXT("Delete All")));
	return Button;
}

FReply SAdvanceDeletionTab::OnDeleteAllButtonClicked()
{
	if (AssetsToDelete.IsEmpty())
	{
		DebugHelper::ShowMessageDialog(EAppMsgType::Ok, TEXT("No Asset Selected"));
		return FReply::Handled();
	}
	TArray<FAssetData> AssetsDataToDelete;
	
	for (const TSharedPtr<FAssetData>& Asset : AssetsToDelete)
	{
		AssetsDataToDelete.Add(*Asset.Get());
	}
	
	FSuperManagerModule& SuperManagerModule = FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));
	const bool bDeleted = SuperManagerModule.DeleteMultipleAssetsForAssetList(AssetsDataToDelete);
	
	if (bDeleted)
	{
		for (const TSharedPtr<FAssetData>& Asset : AssetsToDelete)
		{
			if (StoredAssetDataArray.Contains(Asset))
			{
				StoredAssetDataArray.Remove(Asset);
			}
			
			if (DisplayAssetDataArray.Contains(Asset))
			{
				DisplayAssetDataArray.Remove(Asset);
			}
		}
		
		RefreshAssetListView();
	}
	
	return FReply::Handled();
}

TSharedRef<SButton> SAdvanceDeletionTab::ConstructSelectAllButton()
{
	TSharedRef<SButton> Button = SNew(SButton)
	.ContentPadding(5.f)
	.OnClicked(this, &SAdvanceDeletionTab::OnSelectAllButtonClicked);
	
	Button->SetContent(ConstructTextForButton(TEXT("Select All")));
	return Button;
}

FReply SAdvanceDeletionTab::OnSelectAllButtonClicked()
{
	if (CheckBoxes.Num() == 0) return FReply::Handled();
	
	for (const TSharedRef<SCheckBox>& CheckBox : CheckBoxes)
	{
		if (!CheckBox->IsChecked())
		{
			CheckBox->ToggleCheckedState();
		}
	}
	
	return FReply::Handled();
}

TSharedRef<SButton> SAdvanceDeletionTab::ConstructDeselectAllButton()
{
	TSharedRef<SButton> Button = SNew(SButton)
	.ContentPadding(5.f)
	.OnClicked(this, &SAdvanceDeletionTab::OnDeselectAllButtonClicked);
	
	Button->SetContent(ConstructTextForButton(TEXT("Deselect All")));
	return Button;
}

FReply SAdvanceDeletionTab::OnDeselectAllButtonClicked()
{
	if (CheckBoxes.Num() == 0) return FReply::Handled();
	
	for (const TSharedRef<SCheckBox>& CheckBox : CheckBoxes)
	{
		if (CheckBox->IsChecked())
		{
			CheckBox->ToggleCheckedState();
		}
	}
	
	return FReply::Handled();
}

void SAdvanceDeletionTab::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> InItem)
{
	switch (NewState) 
	{
	case ECheckBoxState::Unchecked:
		// Handle unchecked state
		if (AssetsToDelete.Contains(InItem)) AssetsToDelete.Remove(InItem);
		break;
	case ECheckBoxState::Checked:
		AssetsToDelete.AddUnique(InItem);
		break;
	case ECheckBoxState::Undetermined:
		// Handle undetermined state
		break;
	}
}

void SAdvanceDeletionTab::OnRowWidgetButtonClicked(TSharedPtr<FAssetData> InItem)
{
	FSuperManagerModule& SuperManagerModule = FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));
	SuperManagerModule.SyncCBToAssetList(InItem->GetObjectPathString());
}

FReply SAdvanceDeletionTab::OnDeleteButtonClicked(TSharedPtr<FAssetData> InItem)
{
	FSuperManagerModule& SuperManagerModule = FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));
	const bool bDeleted = SuperManagerModule.DeleteSingleAssetForAssetList(*InItem.Get());
	
	// Remove the item from the list
	if (bDeleted)
	{
		if (StoredAssetDataArray.Contains(InItem))
		{
			StoredAssetDataArray.Remove(InItem);
		}
		
		if (DisplayAssetDataArray.Contains(InItem))
		{
			DisplayAssetDataArray.Remove(InItem);
		}
		RefreshAssetListView();
		
	}

	return FReply::Handled();
}

#pragma endregion 

#pragma region ComboBoxForFiltering

TSharedRef<SComboBox<TSharedPtr<FString>>> SAdvanceDeletionTab::ConstructComboBox()
{
	TSharedRef<SComboBox<TSharedPtr<FString>>> ComboBox = SNew(SComboBox<TSharedPtr<FString>>)
	.OptionsSource(&ComboBoxSourceItems)
	.OnGenerateWidget(this, &SAdvanceDeletionTab::OnGenerateComboBoxContent)
	.OnSelectionChanged(this, &SAdvanceDeletionTab::OnComboBoxSelectionChanged)
	[
		SAssignNew(ComboBoxTextBlock, STextBlock)
		.Text(FText::FromString("List Assets Option"))
	];
		
	return ComboBox;
}

TSharedRef<STextBlock> SAdvanceDeletionTab::ConstructComboHelpText(const FString& InText, ETextJustify::Type InJustification)
{
	FSlateFontInfo HelpTextFont = GetEmbossedTextFont();
	HelpTextFont.Size = 10;
	
	TSharedRef<STextBlock> TextBlock = SNew(STextBlock)
	.Text(FText::FromString(InText))
	.Font(HelpTextFont)
	.ColorAndOpacity(FLinearColor::White)
	.Justification(InJustification)
	.AutoWrapText(true);
	
	return TextBlock;
}

TSharedRef<SWidget> SAdvanceDeletionTab::OnGenerateComboBoxContent(TSharedPtr<FString> InItem)
{
	TSharedRef<STextBlock> TextBlock = SNew(STextBlock)
	.Text(FText::FromString(*InItem.Get()));
	
	return TextBlock;
}

void SAdvanceDeletionTab::OnComboBoxSelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
	ComboBoxTextBlock->SetText(FText::FromString(*NewSelection.Get()));
	
	FSuperManagerModule& SuperManagerModule = FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));
	
	if (*NewSelection.Get() == ListAll)
	{
		DisplayAssetDataArray = StoredAssetDataArray;
		RefreshAssetListView();
	}
	else if (*NewSelection.Get() == ListUnused)
	{
		SuperManagerModule.ListUnusedAssetsForAssetList(StoredAssetDataArray, DisplayAssetDataArray);
		RefreshAssetListView();
	}
	else if (*NewSelection.Get() == ListSameName)
	{
		SuperManagerModule.ListSameNameAssetsForAssetList(StoredAssetDataArray, DisplayAssetDataArray);
		RefreshAssetListView();
	}
}

#pragma endregion 

void SAdvanceDeletionTab::RefreshAssetListView()
{
	AssetsToDelete.Empty();
	CheckBoxes.Empty();
	if (AssetListView.IsValid())
	{
		AssetListView->RebuildList();
	}
}
