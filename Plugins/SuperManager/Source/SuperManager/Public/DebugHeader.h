#pragma once
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

inline void Print (const FString& Message, const FColor& Color = FColor::Green)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, Color, Message);
	}
}

inline void PrintLog (const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
}

inline EAppReturnType::Type ShowMessageDialog (EAppMsgType::Type MsgType, const FString& Message, bool bShowMessageAsWarning = false)
{
	if (bShowMessageAsWarning)
	{
		FText DialogTitle = FText::FromString(TEXT("Warning"));
		return FMessageDialog::Open(MsgType, FText::FromString(Message), DialogTitle);
	}
	else
	{
		return FMessageDialog::Open(MsgType, FText::FromString(Message));
	}
	
}

inline void ShowNotifyInfo (const FString& Message)
{
	FNotificationInfo Info(FText::FromString(Message));
	Info.bUseLargeFont = true;
	Info.FadeOutDuration = 3.0f;
	
	FSlateNotificationManager::Get().AddNotification(Info);
}