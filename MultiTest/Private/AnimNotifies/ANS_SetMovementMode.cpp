// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_SetMovementMode.h"

#include "GameFramework/CharacterMovementComponent.h"

void UANS_SetMovementMode::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                       float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	PlayerRef = Cast<AMultiTestCharacter>(MeshComp->GetOwner());
	if(PlayerRef)
	{
		PlayerRef->GetCharacterMovement()->MovementMode = SetMovement;
	}
	
}

void UANS_SetMovementMode::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	PlayerRef = Cast<AMultiTestCharacter>(MeshComp->GetOwner());
	if(PlayerRef)
	{
		PlayerRef->GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking;
	}
}
