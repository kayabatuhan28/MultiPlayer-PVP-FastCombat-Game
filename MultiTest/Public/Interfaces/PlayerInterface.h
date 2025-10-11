// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

UENUM(BlueprintType)
enum EPlayerHitEffects
{
	None			UMETA(DisplayName = "None"),
	Burn			UMETA(DisplayName = "Burn"),
	Silence			UMETA(DisplayName = "Silence"),
	Slowed			UMETA(DisplayName = "Slowed"),
	LifeSteal		UMETA(DisplayName = "LifeSteal"),
	BlindEffect		UMETA(DisplayName = "BlindEffect"),
};

UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};


class MULTITEST_API IPlayerInterface
{
	GENERATED_BODY()

public:
	
	virtual void TakeHit(AActor* AttackerActor,float TakingDamage, UAnimMontage* HitReaction, EPlayerHitEffects HitEffects, UParticleSystem* HitVfx) = 0;
};
