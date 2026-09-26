// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "QuickMaterialCreationWidget.generated.h"

class UMaterialInstanceConstant;

UENUM(BlueprintType)
enum class E_ChannelPackingType : uint8
{
	ECPT_None UMETA(DisplayName = "None"),
	ECPT_ORM UMETA(DisplayName = "OcclusionRoughnessMetallic"),
	ECPT_MAX UMETA(DisplayName = "DefaultMax")
};

class UMaterialExpressionTextureSample;
/**
 * 
 */
UCLASS()
class SUPERMANAGER_API UQuickMaterialCreationWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()
	
public:
#pragma region QuickMaterialCreationCore
	
	UFUNCTION(BlueprintCallable)
	void CreateMaterialFromSelectedTextures();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CreateMaterialFromSelectedTextures")
	E_ChannelPackingType ChannelPackingType = E_ChannelPackingType::ECPT_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CreateMaterialFromSelectedTextures")
	bool bCreateMaterialInstance = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CreateMaterialFromSelectedTextures")
	bool bCustomMaterialName = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CreateMaterialFromSelectedTextures", meta=(EditCondition="bCustomMaterialName"))
	FString NewMaterialName = TEXT("M_");
	
#pragma endregion 
	
#pragma region SupportedTextureName
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SupportedTextureName")
	TArray<FString> BaseColorArray = { TEXT("_BaseColor"), TEXT("_Albedo"), TEXT("_Diffuse"), TEXT("_diff") };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SupportedTextureName")
	TArray<FString> MetallicArray = { TEXT("_Metallic"), TEXT("_metal") };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SupportedTextureName")
	TArray<FString> NormalArray = { TEXT("_Normal"), TEXT("_nrm"), TEXT("_nor") };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SupportedTextureName")
	TArray<FString> RoughnessArray = { TEXT("_Roughness"), TEXT("_rough") };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SupportedTextureName")
	TArray<FString> AmbientOcclusionArray = { TEXT("_AO"), TEXT("_occlusion") };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SupportedTextureName")
	TArray<FString> ORMArray = { TEXT("_ORM"), TEXT("_arm"), TEXT("_occlusion_roughness_metallic") };

#pragma endregion 
	
private:
	
#pragma region QuickMaterialCreation

	bool ProcessSelectedData(const TArray<FAssetData>& SelectedAssets, TArray<UTexture2D*>& OutSelectedTextures, FString& OutPackagePath);
	bool CheckIsNameUsed(const FString& Name, const FString& PackagePath);
	UMaterial* CreateMaterial(const FString& MaterialName, const FString& PackagePath);
	void Default_CreateMaterialNodes(UMaterial* NewMaterial, UTexture2D* SelectedTexture, uint32& PinConnectedCounter);
	void ORM_CreateMaterialNodes(UMaterial* NewMaterial, UTexture2D* SelectedTexture, uint32& PinConnectedCounter);
	
#pragma endregion

#pragma region CreateMaterialNodes
	
	bool TryConnectBaseColor(UMaterialExpressionTextureSample* TextureSample, UTexture2D* SelectedTexture, UMaterial* NewMaterial);
	bool TryConnectMetallic(UMaterialExpressionTextureSample* TextureSample, UTexture2D* SelectedTexture, UMaterial* NewMaterial);
	bool TryConnectNormal(UMaterialExpressionTextureSample* TextureSample, UTexture2D* SelectedTexture, UMaterial* NewMaterial);
	bool TryConnectRoughness(UMaterialExpressionTextureSample* TextureSample, UTexture2D* SelectedTexture, UMaterial* NewMaterial);
	bool TryConnectAmbientOcclusion(UMaterialExpressionTextureSample* TextureSample, UTexture2D* SelectedTexture, UMaterial* NewMaterial);
	bool TryConnectORM(UMaterialExpressionTextureSample* TextureSample, UTexture2D* SelectedTexture, UMaterial* NewMaterial);

#pragma endregion 
	
	UMaterialInstanceConstant* CreateMaterialInstance(UMaterial* ParentMaterial, FString& MaterialInstanceName, const FString& PackagePath);
};
