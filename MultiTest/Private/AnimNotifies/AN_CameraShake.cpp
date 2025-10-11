// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_CameraShake.h"

#include "MultiTest/MultiTestCharacter.h"

void UAN_CameraShake::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                             const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if(AMultiTestCharacter* PlayerRef = Cast<AMultiTestCharacter>(MeshComp->GetOwner()))
	{
		PlayerRef->PlayCameraShake();
	}
}
