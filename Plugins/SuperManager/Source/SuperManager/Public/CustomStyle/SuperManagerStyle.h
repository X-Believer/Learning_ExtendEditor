// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Styling/SlateStyle.h"

class FSuperManagerStyle
{
public:
	static void InitializeIcons();
	static void Shutdown();
	static FName GetStyleSetName() { return StyleSetName; }
	
private:
	static FName StyleSetName;
	static TSharedPtr<FSlateStyleSet> CreatedStyleSet;
	
	static TSharedRef<FSlateStyleSet> CreateSlateStyleSet();
};
