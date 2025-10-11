// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "OnHeadHealthWidget.generated.h"


UCLASS()
class MULTITEST_API UOnHeadHealthWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UProgressBar* HealthBar;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UProgressBar* ManaBar;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UTextBlock* CurrentHealthTxt;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UTextBlock* CurrentManaTxt;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UTextBlock* PlayerName;

	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UImage* PlayerSteamAvatar;

	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UImage* PlayerSilenceImage;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UImage* PlayerSlowedImage;

	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	class AMultiTestCharacter* OwnerPlayer;

	

	
};
