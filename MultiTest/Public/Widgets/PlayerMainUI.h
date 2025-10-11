// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "PlayerMainUI.generated.h"

/**
 * 
 */
UCLASS()
class MULTITEST_API UPlayerMainUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UProgressBar* Skill1ProgressBar;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UProgressBar* Skill2ProgressBar;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UProgressBar* Skill3ProgressBar;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UProgressBar* Skill4ProgressBar;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UProgressBar* Skill5ProgressBar;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UProgressBar* Skill6ProgressBar;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UProgressBar* Skill7ProgressBar;
	
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UImage* Aim;
	
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UTextBlock* Skill1CooldownText;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UTextBlock* Skill2CooldownText;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UTextBlock* Skill3CooldownText;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UTextBlock* Skill4CooldownText;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UTextBlock* Skill5CooldownText;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UTextBlock* Skill6CooldownText;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UTextBlock* Skill7CooldownText;

	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UImage* Skill1SilenceImg;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UImage* Skill2SilenceImg;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UImage* Skill3SilenceImg;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UImage* Skill4SilenceImg;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UImage* Skill5SilenceImg;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UImage* Skill6SilenceImg;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UImage* Skill7SilenceImg;
	
};
