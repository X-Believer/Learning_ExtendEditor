// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorAction/QuickActorActionsWidget.h"

#include "DebugHeader.h"
#include "Subsystems/EditorActorSubsystem.h"

void UQuickActorActionsWidget::SelectAllActorsOfSameName()
{
	if (!GetEditorActorSubsystem()) return;
	
	TArray<AActor*> SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();
	uint32 SelectionCounter = 0;
	
	if (SelectedActors.Num()== 0)
	{
		DebugHelper::ShowNotifyInfo(TEXT("No actors selected."));
		return;
	}
	
	if (SelectedActors.Num() > 1)
	{
		DebugHelper::ShowNotifyInfo(TEXT("Please select only one actor."));
		return;
	}
	
	FString SelectedActorName = SelectedActors[0]->GetActorLabel();
	const FString NameToSearch = SelectedActorName.LeftChop(4); // Remove the last 4 characters (e.g., "_001")
	
	TArray<AActor*> AllActors = EditorActorSubsystem->GetAllLevelActors();
	
	for (AActor* Actor : AllActors)
	{
		if (!Actor) continue;
		if (Actor->GetActorLabel().Contains(NameToSearch, SearchCase))
		{
			EditorActorSubsystem->SetActorSelectionState(Actor, true);
			SelectionCounter++;
		}
	}
	
	if (SelectionCounter > 0)
	{
		DebugHelper::ShowNotifyInfo(FString::Printf(TEXT("Selected %d actors with the same name."), SelectionCounter));
	}
	else
	{
		DebugHelper::ShowNotifyInfo(TEXT("No actors found with the same name."));
	}
}

void UQuickActorActionsWidget::DuplicateSelectedActors()
{
	if (!GetEditorActorSubsystem()) return;
	
	TArray<AActor*> SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();
	
	if (SelectedActors.Num() == 0)
	{
		DebugHelper::ShowNotifyInfo(TEXT("No actors selected."));
		return;
	}
	
	if (NumberOfDuplicates <= 0 || OffsetDist <= 0.f)
	{
		DebugHelper::ShowNotifyInfo(TEXT("Invalid number of duplicates, or offset distance. Please ensure they are greater than zero."));
		return;
	}

	for (AActor* Actor : SelectedActors)
	{
		if (!Actor) continue;
		
		int32 Counter = 0;
		for (int32 i = 1; i <= NumberOfDuplicates; ++i)
		{
			AActor* DuplicatedActor = EditorActorSubsystem->DuplicateActor(Actor, Actor->GetWorld());
			
			if (!DuplicatedActor) continue;
			
			const float Offset = i * OffsetDist;

			switch (DuplicatedAxis)
			{
				case EDuplicatedAxis::EDA_XAxis:
					DuplicatedActor->AddActorWorldOffset(FVector(Offset, 0.0f, 0.0f));
					break;
				case EDuplicatedAxis::EDA_YAxis:
					DuplicatedActor->AddActorWorldOffset(FVector(0.0f, Offset, 0.0f));
					break;
				case EDuplicatedAxis::EDA_ZAxis:
					DuplicatedActor->AddActorWorldOffset(FVector(0.0f, 0.0f, Offset));
					break;
				default:
					break;
			}
			
			EditorActorSubsystem->SetActorSelectionState(DuplicatedActor, true);
			Counter++;
		}
		
		if (Counter > 0)
		{
			DebugHelper::ShowNotifyInfo(FString::Printf(TEXT("Duplicated %d actors."), Counter));
		}
	}
}

void UQuickActorActionsWidget::RandomizeActorTransform()
{
	const bool bConditionNotSet = !RandomRotation.bRandomYaw 
	&& !RandomRotation.bRandomPitch 
	&& !RandomRotation.bRandomRoll
	&& !bRandomizeScale
	&& !bRandomizeOffset;
	
	if (bConditionNotSet)
	{
		DebugHelper::ShowNotifyInfo(TEXT("No randomization conditions set."));
		return;
	}
	
	if (!GetEditorActorSubsystem()) return;
	
	TArray<AActor*> SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();
	int32 Counter = 0;
	
	if (SelectedActors.Num() == 0)
	{
		DebugHelper::ShowNotifyInfo(TEXT("No actors selected."));
		return;
	}
	
	for (AActor* Actor : SelectedActors)
	{
		if (!Actor) continue;
		
		if (RandomRotation.bRandomYaw)
		{
			const float RandomYaw = FMath::RandRange(RandomRotation.RotYawMin, RandomRotation.RotYawMax);
			FRotator NewRotation = Actor->GetActorRotation();
			NewRotation.Yaw += RandomYaw;
			Actor->SetActorRotation(NewRotation);
		}
		
		if (RandomRotation.bRandomPitch)
		{
			const float RandomPitch = FMath::RandRange(RandomRotation.RotPitchMin, RandomRotation.RotPitchMax);
			FRotator NewRotation = Actor->GetActorRotation();
			NewRotation.Pitch += RandomPitch;
			Actor->SetActorRotation(NewRotation);
		}

		if (RandomRotation.bRandomRoll)
		{
			const float RandomRoll = FMath::RandRange(RandomRotation.RotRollMin, RandomRotation.RotRollMax);
			FRotator NewRotation = Actor->GetActorRotation();
			NewRotation.Roll += RandomRoll;
			Actor->SetActorRotation(NewRotation);
		}

		if (bRandomizeScale)
		{
			const float RandomScale = FMath::RandRange(ScaleMin, ScaleMax);
			Actor->SetActorScale3D(FVector(RandomScale, RandomScale, RandomScale));
		}

		if (bRandomizeOffset)
		{
			const FVector RandomOffset = FVector(
				FMath::RandRange(OffsetMin.X, OffsetMax.X),
				FMath::RandRange(OffsetMin.Y, OffsetMax.Y),
				0.f
			);
			Actor->AddActorWorldOffset(RandomOffset);
		}

		Counter++;
	}
	
	if (Counter > 0)
	{
		DebugHelper::ShowNotifyInfo(FString::Printf(TEXT("Randomized transform of %d actors."), Counter));
	}
}

bool UQuickActorActionsWidget::GetEditorActorSubsystem()
{
	if (!EditorActorSubsystem)
	{
		EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	}
	
	return EditorActorSubsystem != nullptr;
}
