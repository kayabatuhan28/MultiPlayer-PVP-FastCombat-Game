// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "MultiTest/MultiTestCharacter.h"
#include "ANS_SetMovementMode.generated.h"


UCLASS()
class MULTITEST_API UANS_SetMovementMode : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditDefaultsOnly)
	AMultiTestCharacter* PlayerRef;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EMovementMode> SetMovement;
	
};
