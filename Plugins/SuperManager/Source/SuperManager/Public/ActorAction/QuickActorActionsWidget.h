// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "QuickActorActionsWidget.generated.h"

UENUM(BlueprintType)
enum class EDuplicatedAxis : uint8 
{
	EDA_XAxis UMETA(DisplayName = "X Axis"),
	EDA_YAxis UMETA(DisplayName = "Y Axis"),
	EDA_ZAxis UMETA(DisplayName = "Z Axis"),
	EDA_MAX UMETA(DisplayName = "Default MAX")
};

USTRUCT(BlueprintType)
struct FRandomActorRotation
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRandomYaw = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bRandomYaw"))
	float RotYawMin = -45.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bRandomYaw"))
	float RotYawMax = 45.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRandomPitch = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bRandomPitch"))
	float RotPitchMin = -45.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bRandomPitch"))
	float RotPitchMax = 45.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRandomRoll = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bRandomRoll"))
	float RotRollMin = -45.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bRandomRoll"))
	float RotRollMax = 45.f;
};

class UEditorActorSubsystem;
/**
 * 
 */
UCLASS()
class SUPERMANAGER_API UQuickActorActionsWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()
	
public:
#pragma region ActorBatchSelection
	
	UFUNCTION(BlueprintCallable)
	void SelectAllActorsOfSameName();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchSelection")
	TEnumAsByte<ESearchCase::Type> SearchCase = ESearchCase::IgnoreCase;
	
#pragma endregion 

#pragma region ActorBatchDuplication
	UFUNCTION(BlueprintCallable)
	void DuplicateSelectedActors();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchDuplication")
	EDuplicatedAxis DuplicatedAxis = EDuplicatedAxis::EDA_XAxis;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchDuplication")
	int32 NumberOfDuplicates = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchDuplication")
	float OffsetDist = 100.f;
	
#pragma endregion 
	
	
#pragma region ActorBatchRandomization
	
	UFUNCTION(BlueprintCallable)
	void RandomizeActorTransform();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ActorBatchRandomization")
	FRandomActorRotation RandomRotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ActorBatchRandomization")
	bool bRandomizeScale = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ActorBatchRandomization", meta=(EditCondition="bRandomizeScale"))
	float ScaleMin = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ActorBatchRandomization", meta=(EditCondition="bRandomizeScale"))
	float ScaleMax = 2.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ActorBatchRandomization")
	bool bRandomizeOffset = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ActorBatchRandomization", meta=(EditCondition="bRandomizeOffset"))
	FVector OffsetMin = FVector(-100.0f, -100.0f, -100.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ActorBatchRandomization", meta=(EditCondition="bRandomizeOffset"))
	FVector OffsetMax = FVector(100.0f, 100.0f, 100.0f);

#pragma endregion
private:
	UPROPERTY()
	UEditorActorSubsystem* EditorActorSubsystem;
	
	bool GetEditorActorSubsystem();
};
