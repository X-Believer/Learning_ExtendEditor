// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Styling/SlateStyle.h"

class FSuperManagerStyle
{
public:
	static void InitializeIcons();
	static void Shutdown();
	static FName GetStyleSetName() { return StyleSetName; }
	static TSharedRef<FSlateStyleSet> GetStyleSet() { return CreatedStyleSet.ToSharedRef(); }
	
private:
	static FName StyleSetName;
	static TSharedPtr<FSlateStyleSet> CreatedStyleSet;
	
	static TSharedRef<FSlateStyleSet> CreateSlateStyleSet();
};
