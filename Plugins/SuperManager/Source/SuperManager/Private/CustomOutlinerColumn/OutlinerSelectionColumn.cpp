// // Fill out your copyright notice in the Description page of Project Settings.
//
//
// #include "CustomOutlinerColumn/OutlinerSelectionColumn.h"
//
// #include "CustomStyle/SuperManagerStyle.h"
//
// SHeaderRow::FColumn::FArguments FOutLinerSelectionLockColumn::ConstructHeaderRowColumn()
// {
// 	 SHeaderRow::FColumn::FArguments ConstructedHeaderRow = SHeaderRow::Column(GetColumnID())
// 		.DefaultLabel(FText::FromString("Selection Lock"))
// 		.DefaultTooltip(FText::FromString("Press to lock selection"))
// 		.FixedWidth(24.f)
// 		.HAlignHeader(HAlign_Center)
// 		.VAlignHeader(VAlign_Center)
// 		.HAlignCell(HAlign_Center)
// 		.VAlignCell(VAlign_Center)
// 	[
// 		SNew(SImage)
// 		.ColorAndOpacity(FSlateColor::UseForeground())
// 		.Image(FSuperManagerStyle::GetStyleSet()->GetBrush(FName("SuperManager.AdvanceDeletion")))
// 	];
// 	return ConstructedHeaderRow;
// }
//
// const TSharedRef<SWidget> FOutLinerSelectionLockColumn::ConstructRowWidget(FSceneOutlinerTreeItemRef TreeItem, const STableRow<FSceneOutlinerTreeItemPtr>& Row)
// {
// 	return SNullWidget::NullWidget;
// }
