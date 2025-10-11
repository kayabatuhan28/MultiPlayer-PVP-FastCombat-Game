// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_SetRotationMode.h"

#include "MultiTest/MultiTestCharacter.h"

void UANS_SetRotationMode::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                       float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if(AMultiTestCharacter* PlayerRef = Cast<AMultiTestCharacter>(MeshComp->GetOwner()))
	{
		PlayerRef->bUseControllerRotationYaw = false;
	}
}

void UANS_SetRotationMode::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if(AMultiTestCharacter* PlayerRef = Cast<AMultiTestCharacter>(MeshComp->GetOwner()))
	{
		PlayerRef->bUseControllerRotationYaw = true;
	}
}
