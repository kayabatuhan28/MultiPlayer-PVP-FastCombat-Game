// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/OnHeadHealthWidget.h"

#include "MultiTest/MultiTestCharacter.h"

void UOnHeadHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	OwnerPlayer = Cast<AMultiTestCharacter>(GetOwningPlayerPawn());
	if(OwnerPlayer)
	{
		//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("OnHeadWidget Has Owner!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,TEXT("OnHeadWidget Has No Owner!"));
	}
}



