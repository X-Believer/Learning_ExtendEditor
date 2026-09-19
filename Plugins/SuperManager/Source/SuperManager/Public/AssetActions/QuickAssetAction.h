// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "Animation/AnimComposite.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "QuickAssetAction.generated.h"

/**
 * 
 */
UCLASS()
class SUPERMANAGER_API UQuickAssetAction : public UAssetActionUtility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(CallInEditor)
	void DuplicateAsset(int32 NumOfDuplicates);
	
	UFUNCTION(CallInEditor)
	void AddPrefixes();
	
	UFUNCTION(CallInEditor)
	void RemoveUnusedAssets();
	
private:
	TMap<UClass*, FString> PrefixMap = 
	{
		{ UMaterial::StaticClass(), TEXT("M_") },
		{ UMaterialInstance::StaticClass(), TEXT("MI_") },
		{ UTexture::StaticClass(), TEXT("T_") },
		{ USoundWave::StaticClass(), TEXT("SW_") },
		{ UStaticMesh::StaticClass(), TEXT("SM_") },
		{ USkeletalMesh::StaticClass(), TEXT("SK_") },
		{ UBlueprint::StaticClass(), TEXT("BP_") },
		{ UAnimSequence::StaticClass(), TEXT("ANIM_") },
		{ UDataTable::StaticClass(), TEXT("DT_") },
		{ UCurveFloat::StaticClass(), TEXT("CURVE_") },
		{ UTexture2D::StaticClass(), TEXT("T2D_") },
		{ UMaterialFunction::StaticClass(), TEXT("MF_") },
		{ UAnimBlueprint::StaticClass(), TEXT("ABP_") },
		{ UAnimMontage::StaticClass(), TEXT("AM_") },
		{ UAnimSequence::StaticClass(), TEXT("AS_") },
		{ UAnimComposite::StaticClass(), TEXT("AC_") },
		{ UAnimNotify::StaticClass(), TEXT("AN_") },
		{ UAnimNotifyState::StaticClass(), TEXT("ANS_") },
		{ UAnimInstance::StaticClass(), TEXT("AI_") },
		{ UAnimSequenceBase::StaticClass(), TEXT("ASB_") },
		{ UAnimSequence::StaticClass(), TEXT("AS_") },
		{ UAnimMontage::StaticClass(), TEXT("AM_") },
		{ UAnimComposite::StaticClass(), TEXT("AC_") },
		{ UAnimNotify::StaticClass(), TEXT("AN_") },
		{ UAnimNotifyState::StaticClass(), TEXT("ANS_") },
	};
	
	void FixUpRedirectors();
};
