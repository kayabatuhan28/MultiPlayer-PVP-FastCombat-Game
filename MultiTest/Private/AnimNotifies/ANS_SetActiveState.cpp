// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_SetActiveState.h"

void UANS_SetActiveState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	PlayerRef = Cast<AMultiTestCharacter>(MeshComp->GetOwner());
	if(PlayerRef)
	{
		PlayerRef->ActiveState = ChosenState;
	}
}

void UANS_SetActiveState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	PlayerRef = Cast<AMultiTestCharacter>(MeshComp->GetOwner());
	if(PlayerRef)
	{
		PlayerRef->ActiveState = EPlayerActiveState::Empty;
	}
}
