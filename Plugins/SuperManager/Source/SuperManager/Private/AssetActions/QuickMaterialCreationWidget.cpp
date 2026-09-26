// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/QuickMaterialCreationWidget.h"

#include "AssetToolsModule.h"
#include "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "EditorUtilityLibrary.h"
#include "Factories/MaterialFactoryNew.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialInstanceConstant.h"

#pragma region QuickMaterialCreationCore
	
void UQuickMaterialCreationWidget::CreateMaterialFromSelectedTextures()
{
	if (bCustomMaterialName)
	{
		if (NewMaterialName.IsEmpty() || NewMaterialName.Equals(TEXT("M_")))
		{
			DebugHelper::ShowMessageDialog(EAppMsgType::Ok, TEXT("Material name cannot be empty."), true);
			return;
		}
	}
	
	TArray<FAssetData> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<UTexture2D*> SelectedTextures;
	FString SelectedTexturePackagePath;
	uint32 PinConnectedCounter = 0;
	
	if (!ProcessSelectedData(SelectedAssets, SelectedTextures, SelectedTexturePackagePath))
	{
		NewMaterialName = TEXT("M_");
		return;
	}
	
	if (CheckIsNameUsed(NewMaterialName, SelectedTexturePackagePath))
	{
		DebugHelper::ShowMessageDialog(EAppMsgType::Ok, TEXT("Material name is already used."), true);
		return;
	}
	UMaterial* NewMaterial = CreateMaterial(NewMaterialName, SelectedTexturePackagePath);
	
	if (!NewMaterial)
	{
		DebugHelper::ShowMessageDialog(EAppMsgType::Ok, TEXT("Failed to create material."), true);
		return;
	}
	
	for (UTexture2D* Texture : SelectedTextures)
	{
		if (!Texture) continue;

		switch (ChannelPackingType)
		{
		case E_ChannelPackingType::ECPT_None:
			Default_CreateMaterialNodes(NewMaterial, Texture, PinConnectedCounter);
			break;
		case E_ChannelPackingType::ECPT_ORM:
			ORM_CreateMaterialNodes(NewMaterial, Texture, PinConnectedCounter);
			break;
		case E_ChannelPackingType::ECPT_MAX:
			break;
		default:
			break;
		}
	}
	
	if (PinConnectedCounter > 0)
	{
		DebugHelper::ShowNotifyInfo(FString::Printf(TEXT("Successfully created material and connected %d texture nodes."), PinConnectedCounter));
	}
	
	if (bCreateMaterialInstance)
	{
		UMaterialInstanceConstant* MaterialInstance = CreateMaterialInstance(NewMaterial, NewMaterialName, SelectedTexturePackagePath);
		if (MaterialInstance)
		{
			DebugHelper::ShowNotifyInfo(FString::Printf(TEXT("Successfully created material instance: %s"), *MaterialInstance->GetName()));
		}
		else
		{
			DebugHelper::ShowMessageDialog(EAppMsgType::Ok, TEXT("Failed to create material instance."), true);
		}
	}
	
	NewMaterialName = TEXT("M_");
}

#pragma endregion

#pragma region QuickMaterialCreation

bool UQuickMaterialCreationWidget::ProcessSelectedData(const TArray<FAssetData>& SelectedAssets, TArray<UTexture2D*>& OutSelectedTextures, FString& OutPackagePath)
{
	if (SelectedAssets.Num() == 0)
	{
		DebugHelper::ShowMessageDialog(EAppMsgType::Ok, TEXT("No texture selected."), true);
		return false;
	}
	
	bool bMaterialNameSet = false;
	for (const FAssetData& AssetData : SelectedAssets)
	{
		UObject* Asset = AssetData.GetAsset();
		if (!Asset) continue;
		
		UTexture2D* Texture = Cast<UTexture2D>(Asset);
		if (!Texture)
		{
			DebugHelper::ShowMessageDialog(EAppMsgType::Ok, TEXT("Selected asset is not a texture."), true);
			return false;
		}
		
		OutSelectedTextures.Add(Texture);
		if (OutPackagePath.IsEmpty()) OutPackagePath = AssetData.PackagePath.ToString();
		
		if (!bCustomMaterialName && !bMaterialNameSet)
		{
			NewMaterialName = Asset->GetName();
			NewMaterialName.RemoveFromStart(TEXT("T_"));
			NewMaterialName.InsertAt(0, TEXT("M_"));
			bMaterialNameSet = true;
		}
	}
	return true;
}

bool UQuickMaterialCreationWidget::CheckIsNameUsed(const FString& Name, const FString& PackagePath)
{
	TArray<FString> Assets = UEditorAssetLibrary::ListAssets(PackagePath, false);
	return Assets.ContainsByPredicate([&](const FString& AssetPath)
	{
		FString AssetName = FPaths::GetBaseFilename(AssetPath);
		return AssetName.Equals(Name);
	});
}

UMaterial* UQuickMaterialCreationWidget::CreateMaterial(const FString& MaterialName, const FString& PackagePath)
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	
	UMaterialFactoryNew* MaterialFactory = NewObject<UMaterialFactoryNew>();
	UObject* NewAsset = AssetToolsModule.Get().CreateAsset(MaterialName, PackagePath, UMaterial::StaticClass(), MaterialFactory);
	
	return Cast<UMaterial>(NewAsset);
}

void UQuickMaterialCreationWidget::Default_CreateMaterialNodes(UMaterial* NewMaterial, UTexture2D* SelectedTexture, uint32& PinConnectedCounter)
{
	UMaterialExpressionTextureSample* TextureSample = NewObject<UMaterialExpressionTextureSample>(NewMaterial);
	if (!TextureSample) return;
	
	if (!NewMaterial->HasBaseColorConnected())
	{
		if (TryConnectBaseColor(TextureSample, SelectedTexture, NewMaterial))
		{
			PinConnectedCounter++;
			return;
		}
	}
	if (!NewMaterial->HasMetallicConnected())
	{
		if (TryConnectMetallic(TextureSample, SelectedTexture, NewMaterial))
		{
			PinConnectedCounter++;
			return;
		}
	}
	if (!NewMaterial->HasNormalConnected())
	{
		if (TryConnectNormal(TextureSample, SelectedTexture, NewMaterial))
		{
			PinConnectedCounter++;
			return;
		}
	}
	if (!NewMaterial->HasRoughnessConnected())
	{
		if (TryConnectRoughness(TextureSample, SelectedTexture, NewMaterial))
		{
			PinConnectedCounter++;
			return;
		}
	}
	if (!NewMaterial->HasAmbientOcclusionConnected())
	{
		if (TryConnectAmbientOcclusion(TextureSample, SelectedTexture, NewMaterial))
		{
			PinConnectedCounter++;
			return;
		}
	}
	
	DebugHelper::Print(FString::Printf(TEXT("Texture %s does not match any supported texture type."), *SelectedTexture->GetName()), FColor::Yellow);
}

void UQuickMaterialCreationWidget::ORM_CreateMaterialNodes(UMaterial* NewMaterial, UTexture2D* SelectedTexture, uint32& PinConnectedCounter)
{
	UMaterialExpressionTextureSample* TextureSample = NewObject<UMaterialExpressionTextureSample>(NewMaterial);
	if (!TextureSample) return;
	
	if (!NewMaterial->HasBaseColorConnected())
	{
		if (TryConnectBaseColor(TextureSample, SelectedTexture, NewMaterial))
		{
			PinConnectedCounter++;
			return;
		}
	}
	if (!NewMaterial->HasNormalConnected())
	{
		if (TryConnectNormal(TextureSample, SelectedTexture, NewMaterial))
		{
			PinConnectedCounter++;
			return;
		}
	}
	
	if (!NewMaterial->HasRoughnessConnected())
	{
		if (TryConnectORM(TextureSample, SelectedTexture, NewMaterial))
		{
			PinConnectedCounter += 3;
			return;
		}
	}
}

#pragma endregion

#pragma region CreateMaterialNodes

bool UQuickMaterialCreationWidget::TryConnectBaseColor(UMaterialExpressionTextureSample* TextureSample, UTexture2D* SelectedTexture, UMaterial* NewMaterial)
{
	for (const FString& BaseColorSuffix : BaseColorArray)
	{
		if (SelectedTexture->GetName().Contains(BaseColorSuffix))
		{
			TextureSample->Texture = SelectedTexture;
			
			NewMaterial->GetExpressionCollection().AddExpression(TextureSample);
			FExpressionInput* BaseColorInput = NewMaterial->GetExpressionInputForProperty(MP_BaseColor);
			if (BaseColorInput)
			{
				BaseColorInput->Expression = TextureSample;
				NewMaterial->PostEditChange();
				
				TextureSample->MaterialExpressionEditorX -=600;
				return true;
			}
		}
	}
	return false;
}

bool UQuickMaterialCreationWidget::TryConnectMetallic(UMaterialExpressionTextureSample* TextureSample, UTexture2D* SelectedTexture, UMaterial* NewMaterial)
{
	for (const FString& MetallicSuffix : MetallicArray)
	{
		if (SelectedTexture->GetName().Contains(MetallicSuffix))
		{
			SelectedTexture->CompressionSettings = TC_Default;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();
			
			TextureSample->Texture = SelectedTexture;
			TextureSample->SamplerType = SAMPLERTYPE_LinearColor;
			
			NewMaterial->GetExpressionCollection().AddExpression(TextureSample);
			FExpressionInput* MetallicInput = NewMaterial->GetExpressionInputForProperty(MP_Metallic);
			if (MetallicInput)
			{
				MetallicInput->Expression = TextureSample;
				NewMaterial->PostEditChange();
				
				TextureSample->MaterialExpressionEditorX -= 600;
				TextureSample->MaterialExpressionEditorY += 240;
				return true;
			}
		}
	}
	return false;
}

bool UQuickMaterialCreationWidget::TryConnectNormal(UMaterialExpressionTextureSample* TextureSample, UTexture2D* SelectedTexture, UMaterial* NewMaterial)
{
	for (const FString& NormalSuffix : NormalArray)
	{
		if (SelectedTexture->GetName().Contains(NormalSuffix))
		{
			TextureSample->Texture = SelectedTexture;
			TextureSample->SamplerType = SAMPLERTYPE_Normal;
			
			NewMaterial->GetExpressionCollection().AddExpression(TextureSample);
			FExpressionInput* NormalInput = NewMaterial->GetExpressionInputForProperty(MP_Normal);
			if (NormalInput)
			{
				NormalInput->Expression = TextureSample;
				NewMaterial->PostEditChange();
				
				TextureSample->MaterialExpressionEditorX -= 600;
				TextureSample->MaterialExpressionEditorY += 720;
				return true;
			}
		}
	}
	return false;
}

bool UQuickMaterialCreationWidget::TryConnectRoughness(UMaterialExpressionTextureSample* TextureSample, UTexture2D* SelectedTexture, UMaterial* NewMaterial)
{
	for (const FString& RoughnessSuffix : RoughnessArray)
	{
		if (SelectedTexture->GetName().Contains(RoughnessSuffix))
		{
			SelectedTexture->CompressionSettings = TC_Default;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();
			
			TextureSample->SamplerType = SAMPLERTYPE_LinearColor;
			TextureSample->Texture = SelectedTexture;
			
			NewMaterial->GetExpressionCollection().AddExpression(TextureSample);
			FExpressionInput* RoughnessInput = NewMaterial->GetExpressionInputForProperty(MP_Roughness);
			if (RoughnessInput)
			{
				RoughnessInput->Expression = TextureSample;
				NewMaterial->PostEditChange();
				
				TextureSample->MaterialExpressionEditorX -= 600;
				TextureSample->MaterialExpressionEditorY += 480;
				return true;
			}
		}
	}
	return false;
}

bool UQuickMaterialCreationWidget::TryConnectAmbientOcclusion(UMaterialExpressionTextureSample* TextureSample, UTexture2D* SelectedTexture, UMaterial* NewMaterial)
{
	for (const FString& AmbientOcclusionSuffix : AmbientOcclusionArray)
	{
		if (SelectedTexture->GetName().Contains(AmbientOcclusionSuffix))
		{
			SelectedTexture->CompressionSettings = TC_Default;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();
			
			TextureSample->SamplerType = SAMPLERTYPE_LinearColor;
			TextureSample->Texture = SelectedTexture;
			
			NewMaterial->GetExpressionCollection().AddExpression(TextureSample);
			FExpressionInput* AOInput = NewMaterial->GetExpressionInputForProperty(MP_AmbientOcclusion);
			if (AOInput)
			{
				AOInput->Expression = TextureSample;
				NewMaterial->PostEditChange();
				
				TextureSample->MaterialExpressionEditorX -= 600;
				TextureSample->MaterialExpressionEditorY += 960;
				return true;
			}
		}
	}
	return false;
}

bool UQuickMaterialCreationWidget::TryConnectORM(UMaterialExpressionTextureSample* TextureSample,
	UTexture2D* SelectedTexture, UMaterial* NewMaterial)
{
	for (const FString& ORMSuffix : ORMArray)
	{
		if (SelectedTexture->GetName().Contains(ORMSuffix))
		{
			SelectedTexture->CompressionSettings = TC_Masks;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();
			
			TextureSample->SamplerType = SAMPLERTYPE_Masks;
			TextureSample->Texture = SelectedTexture;
			
			NewMaterial->GetExpressionCollection().AddExpression(TextureSample);
			
			FExpressionInput* RoughnessInput = NewMaterial->GetExpressionInputForProperty(MP_Roughness);
			FExpressionInput* MetallicInput = NewMaterial->GetExpressionInputForProperty(MP_Metallic);
			FExpressionInput* AOInput = NewMaterial->GetExpressionInputForProperty(MP_AmbientOcclusion);
			
			if (RoughnessInput && MetallicInput && AOInput)
			{
				AOInput->Connect(1, TextureSample);        // R
				RoughnessInput->Connect(2, TextureSample); // G
				MetallicInput->Connect(3, TextureSample);  // B
				
				NewMaterial->PostEditChange();
				
				TextureSample->MaterialExpressionEditorX -= 600;
				TextureSample->MaterialExpressionEditorY += 240;
				return true;
			}
		}
	}
	return false;
}

#pragma endregion

UMaterialInstanceConstant* UQuickMaterialCreationWidget::CreateMaterialInstance(UMaterial* ParentMaterial, FString& MaterialInstanceName, const FString& PackagePath)
{
	MaterialInstanceName.RemoveFromStart(TEXT("M_"));
	MaterialInstanceName.InsertAt(0, TEXT("MI_"));
	UMaterialInstanceConstantFactoryNew* NewMaterialInstanceConstantFactoryNew = NewObject<UMaterialInstanceConstantFactoryNew>();
	
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	UObject* NewAsset = AssetToolsModule.Get().CreateAsset(MaterialInstanceName, PackagePath, UMaterialInstanceConstant::StaticClass(), NewMaterialInstanceConstantFactoryNew);
	
	if (UMaterialInstanceConstant* CreatedMI = Cast<UMaterialInstanceConstant>(NewAsset))
	{
		CreatedMI->SetParentEditorOnly(ParentMaterial);
		CreatedMI->PostEditChange();
		ParentMaterial->PostEditChange();
		return CreatedMI;
	}
	return nullptr;
}
