// Fill out your copyright notice in the Description page of Project Settings.


#include "ScreenMessage.h"
#include "TimerManager.h"
#include "Components/TextBlock.h"
void UScreenMessage::SetMessageText(FString Message)
{
//	FText MessageText = FText::FromString(Message);
//	MessageTextBlock->SetText(MessageText);
if (!MessageTextBlock)
{
	UE_LOG(LogTemp, Error, TEXT("MessageTextBlock is null in SetMessageText"));
	return;
}

MessageTextBlock->SetText(FText::FromString(Message));

GetWorld()->GetTimerManager().SetTimer(MessageTimerHandle,this,&UScreenMessage::ClearMessage,2.0f,false);
}

void UScreenMessage::ClearMessage()
{
	if (MessageTextBlock)
	{
		MessageTextBlock->SetText(FText::FromString(""));
	}
}