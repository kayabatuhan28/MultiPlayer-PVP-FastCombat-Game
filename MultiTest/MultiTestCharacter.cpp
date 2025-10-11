// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiTestCharacter.h"

#include "MultiTestGameMode.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "Widgets/OnHeadHealthWidget.h"


AMultiTestCharacter::AMultiTestCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	TurnRateGamepad = 50.f;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true; 	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 
	
	GetCharacterMovement()->JumpZVelocity = 1250.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; 
	CameraBoom->bUsePawnControlRotation = true; 
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false;
	
	OnHeadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("OnHeadWidget"));
	OnHeadWidgetComponent->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
	OnHeadWidgetComponent->SetIsReplicated(true);

	DaggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DaggerMesh"));
	DaggerMesh->SetupAttachment(GetMesh(),"DaggerUnEquipSocket");

	StaffMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaffMesh"));
	StaffMesh->SetupAttachment(GetMesh(),"MageEquipSocket");
	
	
}

void AMultiTestCharacter::BeginPlay()
{
	Super::BeginPlay();
	bAlwaysRelevant = true;
	OnHeadWidgetRef = Cast<UOnHeadHealthWidget>(OnHeadWidgetComponent->GetUserWidgetObject());

	
	
}

void AMultiTestCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMultiTestCharacter, CurrentHp)
	DOREPLIFETIME(AMultiTestCharacter, MaxHp)
	DOREPLIFETIME(AMultiTestCharacter, CurrentMana)
	DOREPLIFETIME(AMultiTestCharacter, MaxMana)
	DOREPLIFETIME(AMultiTestCharacter, OnHeadWidgetComponent)
	DOREPLIFETIME(AMultiTestCharacter, bIsWeaponEquipped)
	DOREPLIFETIME(AMultiTestCharacter, PlayerType)
	DOREPLIFETIME(AMultiTestCharacter, ActiveState)
	DOREPLIFETIME(AMultiTestCharacter, M_EquipAnim)
	DOREPLIFETIME(AMultiTestCharacter, M_UnEquipAnim)
	DOREPLIFETIME(AMultiTestCharacter, SkillsActor)
	DOREPLIFETIME(AMultiTestCharacter, SkillCooldown)
	DOREPLIFETIME(AMultiTestCharacter, SkillsAnim)
	DOREPLIFETIME(AMultiTestCharacter, SkillCastTime)
	DOREPLIFETIME(AMultiTestCharacter, SkillIcon)
	DOREPLIFETIME(AMultiTestCharacter, SkillManaCost)
	DOREPLIFETIME(AMultiTestCharacter, DaggerMesh)
	DOREPLIFETIME(AMultiTestCharacter, LightAttackIndex)
	DOREPLIFETIME(AMultiTestCharacter, bIsSkillCooldown)
	DOREPLIFETIME(AMultiTestCharacter, DashManaCost)
	DOREPLIFETIME(AMultiTestCharacter, DashCooldown)
	DOREPLIFETIME(AMultiTestCharacter, bIsDashCd)
	DOREPLIFETIME(AMultiTestCharacter, DashVfx)
	DOREPLIFETIME(AMultiTestCharacter, M_Dash)
	DOREPLIFETIME(AMultiTestCharacter, StaffMesh)
	DOREPLIFETIME(AMultiTestCharacter, bIsSilenced)
	DOREPLIFETIME(AMultiTestCharacter, bIsDead)
	DOREPLIFETIME(AMultiTestCharacter, WPressed)
	DOREPLIFETIME(AMultiTestCharacter, SPressed)
	DOREPLIFETIME(AMultiTestCharacter, APressed)
	DOREPLIFETIME(AMultiTestCharacter, DPressed)
}



// Data Table And Structure Set
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void AMultiTestCharacter::SetPlayerData(FName RowName)
{
	SetPlayerDataServer(RowName);
}

void AMultiTestCharacter::SetPlayerDataMulticast_Implementation(FName RowName)
{
	if(PlayerDataTable)
	{
		FPlayerDataStruct* HoldDatas;
		static const FString ContextString(TEXT("ContextString"));
		HoldDatas = PlayerDataTable->FindRow<FPlayerDataStruct>(RowName,ContextString,true);
		if(HoldDatas)
		{
			SetPlayerDataStruct(HoldDatas);
		}
	}
}

void AMultiTestCharacter::SetPlayerDataServer_Implementation(FName RowName)
{
	SetPlayerDataMulticast(RowName);
}

void AMultiTestCharacter::SetPlayerDataStruct(FPlayerDataStruct* ReceivedData)
{

	PlayerType = ReceivedData->DT_ChoisenType;
	
	MaxHp = ReceivedData->DT_HealthMax;
	CurrentHp = MaxHp;
	MaxMana = ReceivedData->DT_ManaMax;
	CurrentMana = MaxMana;

	OnHeadWidgetRef->ManaBar->SetPercent(CurrentMana / MaxMana);
	OnHeadWidgetRef->CurrentManaTxt->SetText(FText::AsNumber(CurrentMana));
	OnHeadWidgetRef->HealthBar->SetPercent(CurrentHp / MaxHp);
	OnHeadWidgetRef->CurrentHealthTxt->SetText(FText::AsNumber(CurrentHp));

	
	for(int i = 0; i < ReceivedData->DT_Skills.Num() ; i++)
	{
		SkillsActor.Add(ReceivedData->DT_Skills[i]);
	}

	for(int i = 0; i < ReceivedData->DT_SkillsAnim.Num() ; i++)
	{
		SkillsAnim.Add(ReceivedData->DT_SkillsAnim[i]);
	}

	for(int i = 0; i < ReceivedData->DT_SkillsCastTime.Num() ; i++)
	{
		SkillCastTime.Add(ReceivedData->DT_SkillsCastTime[i]);
	}

	for(int i = 0; i < ReceivedData->DT_SkillsCooldown.Num() ; i++)
	{
		SkillCooldown.Add(ReceivedData->DT_SkillsCooldown[i]);
	}

	for(int i = 0; i < ReceivedData->DT_SkillsManaCost.Num() ; i++)
	{
		SkillManaCost.Add(ReceivedData->DT_SkillsManaCost[i]);
	}

	M_EquipAnim = ReceivedData->DT_EquipAnim;
	M_UnEquipAnim = ReceivedData->DT_UnEquipAnim;

	for(int i = 0; i < ReceivedData->DT_DodgeAnims.Num() ; i++)
	{
		M_DodgeAnims.Add(ReceivedData->DT_DodgeAnims[i]);
	}

	DashCooldown = ReceivedData->DT_DashCd;
	DashManaCost = ReceivedData->DT_DashManaCost;

	if(PlayerType == EPlayerType::Assassin)
	{
		Tags[0] = "Player";
		
	}
	else if(PlayerType == EPlayerType::Mage)
	{
		Tags[0] = "Player";
		DaggerMesh->SetVisibility(false);
	}

	//PlayerMainUi->Skill1ProgressBar->SetPercent(.4f);
	
}
// Data Table And Structure Set
//-----------------------------------------------------------------------------------------------------------------------------------------------------


void AMultiTestCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AMultiTestCharacter::EquipWeapon);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AMultiTestCharacter::DashOrTp);
	
	/*
	PlayerInputComponent->BindAction<FSprintDelegate>("Sprint", IE_Pressed, this,&AMultiTestCharacter::SetWalkSpeed, 900.f);
	PlayerInputComponent->BindAction<FSprintDelegate>("Sprint", IE_Released, this,&AMultiTestCharacter::SetWalkSpeed, 600.f);
	*/
	
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &AMultiTestCharacter::DodgeFunction);
	
	PlayerInputComponent->BindAction<FCastSkillDelegate>("Skill1", IE_Pressed, this,&AMultiTestCharacter::CastSkill, 0);
	PlayerInputComponent->BindAction<FCastSkillDelegate>("Skill2", IE_Pressed, this,&AMultiTestCharacter::CastSkill, 1);
	PlayerInputComponent->BindAction<FCastSkillDelegate>("Skill3", IE_Pressed, this,&AMultiTestCharacter::CastSkill, 2);
	PlayerInputComponent->BindAction<FCastSkillDelegate>("Skill4", IE_Pressed, this,&AMultiTestCharacter::CastSkill, 3);
	PlayerInputComponent->BindAction<FCastSkillDelegate>("Skill5", IE_Pressed, this,&AMultiTestCharacter::CastSkill, 4);
	PlayerInputComponent->BindAction<FCastSkillDelegate>("Skill6", IE_Pressed, this,&AMultiTestCharacter::CastSkill, 5);
	PlayerInputComponent->BindAction<FCastSkillDelegate>("Skill7", IE_Pressed, this,&AMultiTestCharacter::CastSkill, 6);
	
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AMultiTestCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AMultiTestCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AMultiTestCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AMultiTestCharacter::LookUpAtRate);
	
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMultiTestCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMultiTestCharacter::TouchStopped);

	// Directional
	PlayerInputComponent->BindAction("WKey", IE_Pressed, this, &AMultiTestCharacter::WPress);
	PlayerInputComponent->BindAction("WKey", IE_Released, this, &AMultiTestCharacter::WRelease);
	PlayerInputComponent->BindAction("SKey", IE_Pressed, this, &AMultiTestCharacter::SPress);
	PlayerInputComponent->BindAction("SKey", IE_Released, this, &AMultiTestCharacter::SRelease);
	PlayerInputComponent->BindAction("AKey", IE_Pressed, this, &AMultiTestCharacter::APress);
	PlayerInputComponent->BindAction("AKey", IE_Released, this, &AMultiTestCharacter::ARelease);
	PlayerInputComponent->BindAction("DKey", IE_Pressed, this, &AMultiTestCharacter::DPress);
	PlayerInputComponent->BindAction("DKey", IE_Released, this, &AMultiTestCharacter::DRelease);

	
}



// Widget Updates
//---------------------------------------------------------------------------------------------------------------------------
void AMultiTestCharacter::UpdateHpWidget(float CurrentHealthRef, float MaxHealthRef)
{
	UpdateHpWidgetServer(CurrentHealthRef,MaxHealthRef);
}

void AMultiTestCharacter::UpdateHpWidgetMulticast_Implementation(float CurrentHealthRef, float MaxHealthRef)
{
	OnHeadWidgetRef->HealthBar->SetPercent(CurrentHealthRef / MaxHealthRef);
	OnHeadWidgetRef->CurrentHealthTxt->SetText(FText::AsNumber(CurrentHealthRef));
}

void AMultiTestCharacter::UpdateHpWidgetServer_Implementation(float CurrentHealthRef, float MaxHealthRef)
{
	UpdateHpWidgetMulticast(CurrentHealthRef,MaxHealthRef);
}
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
void AMultiTestCharacter::UpdateManaWidget(float CurrentManaRef, float MaxManaRef)
{
	UpdateManaWidgetServer(CurrentManaRef,MaxManaRef);
}

void AMultiTestCharacter::UpdateManaWidgetMulticast_Implementation(float CurrentManaRef, float MaxManaRef)
{
	OnHeadWidgetRef->ManaBar->SetPercent(CurrentManaRef / MaxManaRef);
	OnHeadWidgetRef->CurrentManaTxt->SetText(FText::AsNumber(CurrentManaRef));
}

void AMultiTestCharacter::UpdateManaWidgetServer_Implementation(float CurrentManaRef, float MaxManaRef)
{
	UpdateManaWidgetMulticast(CurrentManaRef,MaxManaRef);
}
// Widget Updates
//---------------------------------------------------------------------------------------------------------------------------



void AMultiTestCharacter::SetStartData(float HealthRef, float MaxHealthRef, float ManaRef, float MaxManaRef)
{
	OnHeadWidgetRef->HealthBar->SetPercent(HealthRef / MaxHealthRef);
	OnHeadWidgetRef->ManaBar->SetPercent(ManaRef / MaxManaRef);
	OnHeadWidgetRef->CurrentHealthTxt->SetText(FText::AsNumber(HealthRef));
	OnHeadWidgetRef->CurrentManaTxt->SetText(FText::AsNumber(ManaRef));
	OnHeadWidgetRef->PlayerName->SetText(FText::FromString("Selami"));
}


// Dash
//--------------------------------------------------------------------------------------------------------------------
void AMultiTestCharacter::DashOrTp()
{
	if(PlayerType == EPlayerType::Default)
		return;
	
	if(bIsWeaponEquipped == false ||
		ActiveState != EPlayerActiveState::Empty ||
		bIsDashCd == true ||
		CurrentMana <= DashManaCost ||
		GetCharacterMovement()->IsFalling()
		)
	{
		return;
	}
	
	if(PlayerType == EPlayerType::Assassin)
	{
		if(MageDashCount == 0)
		{
			if(WPressed == true)
			{
				PlayMontage(M_Dash[0]);
			}
			else if(SPressed == true)
			{
				PlayMontage(M_Dash[1]);
			}
			else if(APressed == true)
			{
				PlayMontage(M_Dash[2]);
			}
			else if(DPressed == true)
			{
				PlayMontage(M_Dash[3]);
			}
			else
			{
				PlayMontage(M_Dash[0]);
			}
			MageDashCount++;
			FTimerHandle DashHandler;
			GetWorld()->GetTimerManager().SetTimer(DashHandler,this,&AMultiTestCharacter::ClearDashCd,DashCooldown,false);
		}
		else if(MageDashCount == 1)
		{
			if(WPressed == true)
			{
				PlayMontage(M_Dash[4]);
			}
			else if(SPressed == true)
			{
				PlayMontage(M_Dash[5]);
			}
			else if(APressed == true)
			{
				PlayMontage(M_Dash[6]);
			}
			else if(DPressed == true)
			{
				PlayMontage(M_Dash[7]);
			}
			else
			{
				PlayMontage(M_Dash[4]);
			}
			MageDashCount++;
			bIsDashCd = true;
		}
		
	}
	else if(PlayerType == EPlayerType::Mage)
	{
		// 0 - Front Tp   1- Back Tp   2- Left Tp      3- Right Tp      
		if(WPressed == true)
		{
			DashOrTpServer(0);
		}
		else if(SPressed == true)
		{
			DashOrTpServer(1);
		}
		else if(APressed == true)
		{
			DashOrTpServer(2);
		}
		else if(DPressed == true)
		{
			DashOrTpServer(3);
		}
		else
		{
			DashOrTpServer(0);
		}
	}
	
	CurrentMana -= DashManaCost;
	UpdateManaWidget(CurrentMana,MaxMana);
}

void AMultiTestCharacter::DashOrTpMulticast_Implementation(int TpRot)
{
	if(MageDashCount + 1 == 2)
	{
		FVector NewLoc;
		// 0 - Front Tp   1- Back Tp   2- Left Tp     3- Right Tp      
		switch (TpRot)
		{
		case 0:
			NewLoc = GetActorLocation() + (GetActorForwardVector()* 1200.f);
			break;
		case 1:
			NewLoc = GetActorLocation() + (GetActorForwardVector()* -1200.f);
			break;
		case 2:
			NewLoc = GetActorLocation() + (GetActorRightVector() * -1200.f);
			break;
		case 3:
			NewLoc = GetActorLocation() + (GetActorRightVector() * 1200.f);
			break;
		default:
			break;
		}
		
		FRotator NewRot = GetActorRotation();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),DashVfx,GetActorLocation(),GetActorRotation(),FVector(1.3f));
		SetActorLocationAndRotation(NewLoc,NewRot,true);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),DashVfx,GetActorLocation(),GetActorRotation(),FVector(1.3f));
		bIsDashCd = true;
		MageDashCount = 0;
		FTimerHandle DashHandler;
		GetWorld()->GetTimerManager().SetTimer(DashHandler,this,&AMultiTestCharacter::ClearDashCd,DashCooldown,false);
	}
	else
	{
		FVector NewLoc;
		switch (TpRot)
		{
		case 0:
			NewLoc = GetActorLocation() + (GetActorForwardVector() * 1200.f);
			break;
		case 1:
			NewLoc = GetActorLocation() + (GetActorForwardVector() * -1200.f);
			break;
		case 2:
			NewLoc = GetActorLocation() + (GetActorRightVector() * -1200.f);
			break;
		case 3:
			NewLoc = GetActorLocation() + (GetActorRightVector() * 1200.f);
			break;
		default:
			break;
		}
			
			
		FRotator NewRot = GetActorRotation();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),DashVfx,GetActorLocation(),GetActorRotation(),FVector(1.3f));
		SetActorLocationAndRotation(NewLoc,NewRot,true);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),DashVfx,GetActorLocation(),GetActorRotation(),FVector(1.3f));
		MageDashCount++;
	}
	
}

void AMultiTestCharacter::DashOrTpServer_Implementation(int TpRot)
{
	DashOrTpMulticast(TpRot);
}

void AMultiTestCharacter::ClearDashCd()
{
	bIsDashCd = false;
	MageDashCount = 0;
}
// Dash
//--------------------------------------------------------------------------------------------------------------------




// Equip Animation For Multiplayer
void AMultiTestCharacter::EquipWeapon()
{
	if(PlayerType == EPlayerType::Default)
		return;

	
	if(bIsWeaponEquipped == true)
	{
		EquipWeaponServer(false);
	}
	else
	{
		EquipWeaponServer(true);
	}
	
}


// Replicated PLay Montage
// ---------------------------------------------------------------------------------------------------------------------------
void AMultiTestCharacter::PlayMontage(UAnimMontage* PlayedMontage)
{
	PlayMontageServer(PlayedMontage);
}

void AMultiTestCharacter::PlayMontageServer_Implementation(UAnimMontage* PlayedMontage)
{
	PlayMontageMulticast(PlayedMontage);
}

void AMultiTestCharacter::PlayMontageMulticast_Implementation(UAnimMontage* PlayedMontage)
{
	PlayAnimMontage(PlayedMontage);
}
// Replicated PLay Montage
// ---------------------------------------------------------------------------------------------------------------------------



// Replicated Equip Functions
// ----------------------------------------------------------------------------------------------------------------------------
void AMultiTestCharacter::EquipWeaponServer_Implementation(bool IsEquip)
{
	EquipWeaponMultiCast(IsEquip);
}

void AMultiTestCharacter::EquipWeaponMultiCast_Implementation(bool IsEquip)
{
	if(IsEquip == true)
	{
		PlayAnimMontage(M_EquipAnim);
		bIsWeaponEquipped = true;
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		if(PlayerType == EPlayerType::Assassin)
		{
			GetCharacterMovement()->MaxWalkSpeed = 500.f;
			DaggerMesh->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,"DaggerEquipSocket");
		}
		if(PlayerType == EPlayerType::Mage)
		{
			GetCharacterMovement()->MaxWalkSpeed = 500.f;
			
		}
	}
	else
	{
		PlayAnimMontage(M_UnEquipAnim);
		bIsWeaponEquipped = false;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		if(PlayerType == EPlayerType::Assassin)
		{
			GetCharacterMovement()->MaxWalkSpeed = 900.f;
			DaggerMesh->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,"DaggerUnEquipSocket");
		}
		if(PlayerType == EPlayerType::Mage)
		{
			GetCharacterMovement()->MaxWalkSpeed = 900.f;
			
		}
	}
}
// Replicated Equip Functions
// ----------------------------------------------------------------------------------------------------------------------------



void AMultiTestCharacter::SpawnShadowVfxMulticast_Implementation()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ShadowEndVfx,GetMesh()->GetSocketLocation("spine_01"),GetActorRotation(),FVector(1.7f));
}

void AMultiTestCharacter::SpawnShadowVfxServer_Implementation()
{
	SpawnShadowVfxMulticast();
}



// Cast Skill And Clear Cooldown
//--------------------------------------------------------------------------------------------------------------------------------------------------
void AMultiTestCharacter::CastSkill(int SkillIndex)
{
	
	if(PlayerType == EPlayerType::Default || bIsSilenced == true)
		return;
	
	if(ActiveState != EPlayerActiveState::Empty ||
		GetCharacterMovement()->IsFalling() ||
		bIsSkillCooldown[SkillIndex] == true ||
		CurrentMana <= SkillManaCost[SkillIndex] ||
		bIsWeaponEquipped == false ||
		PlayerType == EPlayerType::Default
		)
	{
		return;
	}	

	CurrentMana -= SkillManaCost[SkillIndex];
	UpdateManaWidget(CurrentMana,MaxMana);
	
	PlayMontage(SkillsAnim[SkillIndex]);
	FTimerDelegate SpawnDelegate;
	FTimerHandle SpawnTimerHandle;
	
	ClearSkillCooldown(SkillIndex);
	SpawnDelegate.BindUFunction(this,FName("CastSkillServer"),SkillIndex);
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle,SpawnDelegate,SkillCastTime[SkillIndex],false);

	if(PlayerType == EPlayerType::Assassin && SkillIndex == 6)
	{
		FTimerHandle ShadowEndHandler;
		GetWorldTimerManager().SetTimer(ShadowEndHandler,this,&AMultiTestCharacter::SpawnShadowVfxServer,9.14f,false);
		
		if(HasAuthority())
		{
			FTimerHandle ShadowHandler;
			FTimerDelegate ShadowDelegate = FTimerDelegate::CreateUObject( this, &AMultiTestCharacter::HideMeshServerAuthority, false,ESlateVisibility::Hidden);
			GetWorldTimerManager().SetTimer(ShadowHandler, ShadowDelegate, 0.37f, false );
			
			FTimerHandle ShadowHandler2;
			GetWorldTimerManager().SetTimer(ShadowHandler2,this,&AMultiTestCharacter::SetMeshVisiblityAuthority,0.41f,false);
			
			FTimerHandle ShadowHandler3;
			FTimerDelegate ShadowDelegate3 = FTimerDelegate::CreateUObject( this, &AMultiTestCharacter::HideMeshServerAuthority, true,ESlateVisibility::Visible);
			GetWorldTimerManager().SetTimer(ShadowHandler3, ShadowDelegate3, 9.3f, false );
		}
		else
		{
			FTimerHandle ClientShadowHandler;
			FTimerDelegate ClientShadowDelegate = FTimerDelegate::CreateUObject( this, &AMultiTestCharacter::HideMeshServerRemote, false,ESlateVisibility::Hidden);
			GetWorldTimerManager().SetTimer(ClientShadowHandler, ClientShadowDelegate, 0.37f, false );

			FTimerHandle ClientShadowHandler2;
			FTimerDelegate ClientShadowDelegate2 = FTimerDelegate::CreateUObject( this, &AMultiTestCharacter::SetMeshVisiblityRemote, true);
			GetWorldTimerManager().SetTimer(ClientShadowHandler2, ClientShadowDelegate2, 0.4f, false );

			FTimerHandle ClientShadowHandler3;
			FTimerDelegate ClientShadowDelegate3 = FTimerDelegate::CreateUObject( this, &AMultiTestCharacter::HideMeshServerRemote, true,ESlateVisibility::Visible);
			GetWorldTimerManager().SetTimer(ClientShadowHandler3, ClientShadowDelegate3, 9.3f, false );
		}
		
	}
}

void AMultiTestCharacter::CastSkillServer_Implementation(int SkillIndex)
{
	if(ActiveState == EPlayerActiveState::Hitted)
	{
		return;
	}
	FVector SpawnLoc;
	FRotator SpawnRot;
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Instigator = this;
	
	if(SkillIndex == 0)
	{
		if(PlayerType == EPlayerType::Mage)
		{
			 SpawnLoc = FVector(GetMesh()->GetSocketLocation("hand_l"));
			 SpawnRot = SetActorRotationToLineTraceHit(SpawnLoc);
		}
		else if(PlayerType == EPlayerType::Assassin)
		{
			SpawnLoc = FVector(GetMesh()->GetSocketLocation("hand_l"));
			SpawnRot = GetFollowCamera()->GetComponentRotation();
		}
	}
	else if(SkillIndex == 1)
	{
		if(PlayerType == EPlayerType::Mage)
		{
			
			SpawnLoc = GetActorLocation();
			SpawnRot = FRotator::ZeroRotator;
		}
		else if(PlayerType == EPlayerType::Assassin)
		{
			SpawnLoc = FVector(GetMesh()->GetSocketLocation("hand_l"));
			SpawnRot = SetActorRotationToLineTraceHit(SpawnLoc);
		}
	}
	else if(SkillIndex == 2)
	{
		if(PlayerType == EPlayerType::Mage)
		{
			SpawnLoc = FVector(GetMesh()->GetSocketLocation("spine_01"));
			SpawnRot = FRotator::ZeroRotator;
		}
		else if(PlayerType == EPlayerType::Assassin)
		{
			SpawnLoc = FVector(GetMesh()->GetSocketLocation("hand_l"));
			SpawnRot = SetActorRotationToLineTraceHit(SpawnLoc);
		}
	}
	else if(SkillIndex == 3)
	{
		if(PlayerType == EPlayerType::Mage)
		{
			SpawnLoc = FVector(GetMesh()->GetSocketLocation("root"));
			SpawnRot = GetActorRotation();
		}
		else if(PlayerType == EPlayerType::Assassin)
		{
			SpawnLoc = GetActorLocation() + ( GetActorForwardVector() * 100.f);
			SpawnRot = GetActorRotation();
		}
	}
	else if(SkillIndex == 4)
	{
		if(PlayerType == EPlayerType::Mage)
		{
			SpawnLoc = FVector(GetMesh()->GetSocketLocation("hand_l"));
			SpawnRot =  SetActorRotationToLineTraceHit(SpawnLoc);
		}
		else if(PlayerType == EPlayerType::Assassin)
		{
			SpawnLoc = GetActorLocation() + ( GetActorForwardVector() * 450.f);
			SpawnRot = FRotator::ZeroRotator;
		}
	}
	else if(SkillIndex == 5)
	{
		if(PlayerType == EPlayerType::Mage)
		{
			SpawnLoc = GetActorLocation() + ( GetActorForwardVector() * 925.f);
			SpawnRot =  GetActorRotation();
		}
		else if(PlayerType == EPlayerType::Assassin)
		{
			SpawnLoc = GetActorLocation() + ( GetActorForwardVector() * 450.f);
			SpawnRot = GetActorRotation();
		}
	}
	else if(SkillIndex == 6)
	{
		if(PlayerType == EPlayerType::Mage)
		{
			SpawnLoc = GetActorLocation() + ( GetActorForwardVector() * 100.f);
			SpawnRot = GetActorRotation();
		}
		else if(PlayerType == EPlayerType::Assassin)
		{
			SpawnLoc = GetActorLocation();
			SpawnRot = FRotator::ZeroRotator;
		}
	}
	
	
	AActor* SpawnlananAc =	GetWorld()->SpawnActor<AActor>(SkillsActor[SkillIndex],SpawnLoc,SpawnRot,Params);
	//SpawnlananAc->SetOwner(this);
}

FRotator AMultiTestCharacter::SetActorRotationToLineTraceHit(FVector StartLoc)
{
	FHitResult Hit;
 
	
	FVector TraceStart = GetFollowCamera()->GetComponentLocation();
	FVector TraceEnd = (
		(GetFollowCamera()->GetComponentLocation()) +
		(GetFollowCamera()->GetForwardVector() * 8000.f)
		);
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Camera, QueryParams);
	
	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, Hit.bBlockingHit ? FColor::Green : FColor::Red, false, 5.0f, 0, 1.0f);
	
	if (Hit.bBlockingHit && IsValid(Hit.GetActor()))
	{
		return UKismetMathLibrary::FindLookAtRotation(StartLoc,Hit.Location);
	}

	return GetFollowCamera()->GetComponentRotation();
}


void AMultiTestCharacter::ClearSkillCooldown_Implementation(int CooldownIndex)
{
	switch (CooldownIndex)
	{
	case 0 :
		bIsSkillCooldown[0] = true;
		Skill1Cd = 0.f;
		if(PlayerMainUi)
		{
			PlayerMainUi->Skill1CooldownText->SetVisibility(ESlateVisibility::Visible);
			PlayerMainUi->Skill1CooldownText->SetText(FText::AsNumber(SkillCooldown[0]));
			PlayerMainUi->Skill1ProgressBar->SetPercent(Skill1Cd / SkillCooldown[0]);	
		}
		GetWorld()->GetTimerManager().SetTimer(Skill1Handle,this,&AMultiTestCharacter::ClearSkill1,1.f,true);
		break;
	case 1:
		bIsSkillCooldown[1] = true;
		Skill2Cd = 0.f;
		if(PlayerMainUi)
		{
			PlayerMainUi->Skill2CooldownText->SetVisibility(ESlateVisibility::Visible);
			PlayerMainUi->Skill2CooldownText->SetText(FText::AsNumber(SkillCooldown[1]));
			PlayerMainUi->Skill2ProgressBar->SetPercent(Skill2Cd / SkillCooldown[1]);
		}
		GetWorld()->GetTimerManager().SetTimer(Skill2Handle,this,&AMultiTestCharacter::ClearSkill2,1.f,true);
		break;
	case 2:
		bIsSkillCooldown[2] = true;
		Skill3Cd = 0.f;
		if(PlayerMainUi)
		{
			PlayerMainUi->Skill3CooldownText->SetVisibility(ESlateVisibility::Visible);
			PlayerMainUi->Skill3CooldownText->SetText(FText::AsNumber(SkillCooldown[2]));
			PlayerMainUi->Skill3ProgressBar->SetPercent(Skill3Cd / SkillCooldown[2]);
		}
		GetWorld()->GetTimerManager().SetTimer(Skill3Handle,this,&AMultiTestCharacter::ClearSkill3,1.f,true);
		break;
	case 3:
		bIsSkillCooldown[3] = true;
		Skill4Cd = 0.f;
		if(PlayerMainUi)
		{
			PlayerMainUi->Skill4CooldownText->SetVisibility(ESlateVisibility::Visible);
			PlayerMainUi->Skill4CooldownText->SetText(FText::AsNumber(SkillCooldown[3]));
			PlayerMainUi->Skill4ProgressBar->SetPercent(Skill4Cd / SkillCooldown[3]);
		}
		GetWorld()->GetTimerManager().SetTimer(Skill4Handle,this,&AMultiTestCharacter::ClearSkill4,1.f,true);
		break;
	case 4:
		bIsSkillCooldown[4] = true;
		Skill5Cd = 0.f;
		if(PlayerMainUi)
		{
			PlayerMainUi->Skill5CooldownText->SetVisibility(ESlateVisibility::Visible);
			PlayerMainUi->Skill5CooldownText->SetText(FText::AsNumber(SkillCooldown[4]));
			PlayerMainUi->Skill5ProgressBar->SetPercent(Skill5Cd / SkillCooldown[4]);
		}
		GetWorld()->GetTimerManager().SetTimer(Skill5Handle,this,&AMultiTestCharacter::ClearSkill5,1.f,true);
		break;
	case 5:
		bIsSkillCooldown[5] = true;
		Skill6Cd = 0.f;
		if(PlayerMainUi)
		{
			PlayerMainUi->Skill6CooldownText->SetVisibility(ESlateVisibility::Visible);
			PlayerMainUi->Skill6CooldownText->SetText(FText::AsNumber(SkillCooldown[5]));
			PlayerMainUi->Skill6ProgressBar->SetPercent(Skill6Cd / SkillCooldown[5]);
		}
		GetWorld()->GetTimerManager().SetTimer(Skill6Handle,this,&AMultiTestCharacter::ClearSkill6,1.f,true);
		break;
	case 6:
		bIsSkillCooldown[6] = true;
		Skill7Cd = 0.f;
		if(PlayerMainUi)
		{
			PlayerMainUi->Skill7CooldownText->SetVisibility(ESlateVisibility::Visible);
			PlayerMainUi->Skill7CooldownText->SetText(FText::AsNumber(SkillCooldown[6]));
			PlayerMainUi->Skill7ProgressBar->SetPercent(Skill7Cd / SkillCooldown[6]);
		}
		GetWorld()->GetTimerManager().SetTimer(Skill7Handle,this,&AMultiTestCharacter::ClearSkill7,1.f,true);
		break;	
	default:
		break;
	}
}

void AMultiTestCharacter::ClearSkill1()
{
	if(Skill1Cd + 1 >= SkillCooldown[0])
	{
		GetWorld()->GetTimerManager().ClearTimer(Skill1Handle);
		if(PlayerMainUi)
		{
			PlayerMainUi->Skill1ProgressBar->SetPercent(1.f);
			PlayerMainUi->Skill1CooldownText->SetVisibility(ESlateVisibility::Hidden);
		}
		
		bIsSkillCooldown[0] = false;
	}
	else
	{
		Skill1Cd += 1.f;
		if(PlayerMainUi)
		{
			PlayerMainUi->Skill1ProgressBar->SetPercent(Skill1Cd / SkillCooldown[0]);
			PlayerMainUi->Skill1CooldownText->SetText(FText::AsNumber(SkillCooldown[0]-Skill1Cd));
		}
		
	}
}

void AMultiTestCharacter::ClearSkill2()
{
	if(Skill2Cd + 1 >= SkillCooldown[1])
	{
		GetWorld()->GetTimerManager().ClearTimer(Skill2Handle);
		if(PlayerMainUi)
		{
			PlayerMainUi->Skill2ProgressBar->SetPercent(1.f);
			PlayerMainUi->Skill2CooldownText->SetVisibility(ESlateVisibility::Hidden);
		}
		
		bIsSkillCooldown[1] = false;
	}
	else
	{
		Skill2Cd += 1.f;
		if(PlayerMainUi)
		{
			PlayerMainUi->Skill2ProgressBar->SetPercent(Skill2Cd / SkillCooldown[1]);
			PlayerMainUi->Skill2CooldownText->SetText(FText::AsNumber(SkillCooldown[1]-Skill2Cd));	
		}
		
	}
}

void AMultiTestCharacter::ClearSkill3()
{
	if(Skill3Cd + 1 >= SkillCooldown[2])
	{
		GetWorld()->GetTimerManager().ClearTimer(Skill3Handle);
		if(PlayerMainUi)
		{
			PlayerMainUi->Skill3ProgressBar->SetPercent(1.f);
			PlayerMainUi->Skill3CooldownText->SetVisibility(ESlateVisibility::Hidden);
		}
		
		bIsSkillCooldown[2] = false;
	}
	else
	{
		Skill3Cd += 1.f;
		if(PlayerMainUi)
		{
			PlayerMainUi->Skill3ProgressBar->SetPercent(Skill3Cd / SkillCooldown[2]);
			PlayerMainUi->Skill3CooldownText->SetText(FText::AsNumber(SkillCooldown[2]-Skill3Cd));
		}
		
	}
}

void AMultiTestCharacter::ClearSkill4()
{
	if(Skill4Cd + 1 >= SkillCooldown[3])
	{
		GetWorld()->GetTimerManager().ClearTimer(Skill4Handle);
		if(PlayerMainUi)
		{
			PlayerMainUi->Skill4ProgressBar->SetPercent(1.f);
			PlayerMainUi->Skill4CooldownText->SetVisibility(ESlateVisibility::Hidden);
		}
		
		bIsSkillCooldown[3] = false;
	}
	else
	{
		Skill4Cd += 1.f;
		if(PlayerMainUi)
		{
			PlayerMainUi->Skill4ProgressBar->SetPercent(Skill4Cd / SkillCooldown[3]);
			PlayerMainUi->Skill4CooldownText->SetText(FText::AsNumber(SkillCooldown[3]-Skill4Cd));
		}
		
	}
}

void AMultiTestCharacter::ClearSkill5()
{
	if(Skill5Cd + 1 >= SkillCooldown[4])
	{
		GetWorld()->GetTimerManager().ClearTimer(Skill5Handle);
		if(PlayerMainUi)
		{
			PlayerMainUi->Skill5ProgressBar->SetPercent(1.f);
			PlayerMainUi->Skill5CooldownText->SetVisibility(ESlateVisibility::Hidden);
		}
		
		bIsSkillCooldown[4] = false;
	}
	else
	{
		Skill5Cd += 1.f;
		if(PlayerMainUi)
		{
			PlayerMainUi->Skill5ProgressBar->SetPercent(Skill5Cd / SkillCooldown[4]);
			PlayerMainUi->Skill5CooldownText->SetText(FText::AsNumber(SkillCooldown[4]-Skill5Cd));
		}
		
	}
}

void AMultiTestCharacter::ClearSkill6()
{
	if(Skill6Cd + 1 >= SkillCooldown[5])
	{
		GetWorld()->GetTimerManager().ClearTimer(Skill6Handle);
		if(PlayerMainUi)
		{
			PlayerMainUi->Skill6ProgressBar->SetPercent(1.f);
			PlayerMainUi->Skill6CooldownText->SetVisibility(ESlateVisibility::Hidden);
		}
		
		bIsSkillCooldown[5] = false;
	}
	else
	{
		Skill6Cd += 1.f;
		if(PlayerMainUi)
		{
			PlayerMainUi->Skill6ProgressBar->SetPercent(Skill6Cd / SkillCooldown[5]);
			PlayerMainUi->Skill6CooldownText->SetText(FText::AsNumber(SkillCooldown[5]-Skill6Cd));
		}
		
	}
}

void AMultiTestCharacter::ClearSkill7()
{
	if(Skill7Cd + 1 >= SkillCooldown[6])
	{
		GetWorld()->GetTimerManager().ClearTimer(Skill7Handle);
		if(PlayerMainUi)
		{
			PlayerMainUi->Skill7ProgressBar->SetPercent(1.f);
			PlayerMainUi->Skill7CooldownText->SetVisibility(ESlateVisibility::Hidden);
		}
		
		bIsSkillCooldown[6] = false;
	}
	else
	{
		Skill7Cd += 1.f;
		if(PlayerMainUi)
		{
			PlayerMainUi->Skill7ProgressBar->SetPercent(Skill7Cd / SkillCooldown[6]);
			PlayerMainUi->Skill7CooldownText->SetText(FText::AsNumber(SkillCooldown[6]-Skill7Cd));
		}
		
	}
}

// Cast Skill And Clear Cooldown
//--------------------------------------------------------------------------------------------------------------------------------------------------



// Dodge
//------------------------------------------------------------------------------------------------------------------------------------------------
void AMultiTestCharacter::DodgeFunction()
{
	if(PlayerType == EPlayerType::Default)
		return;
	
	if(GetCharacterMovement()->IsFalling() || ActiveState != EPlayerActiveState::Empty || bIsDodgeCd == true)
	{
		return;
	}
	
	if(bIsWeaponEquipped == false)
	{
		PlayMontage(M_DodgeAnims[0]);
		bIsDodgeCd = true;
		FTimerHandle DodgeCdClearHandler;
		GetWorld()->GetTimerManager().SetTimer(DodgeCdClearHandler,this,&AMultiTestCharacter::ClearDodgeCd,2.5f,false);
		return;
	}
	
	if(WPressed == true)
	{
		PlayMontage(M_DodgeAnims[0]);
		bIsDodgeCd = true;
		FTimerHandle DodgeCdClearHandler;
		GetWorld()->GetTimerManager().SetTimer(DodgeCdClearHandler,this,&AMultiTestCharacter::ClearDodgeCd,2.5f,false);
	}
	else if(SPressed == true)
	{
		PlayMontage(M_DodgeAnims[1]);
		bIsDodgeCd = true;
		FTimerHandle DodgeCdClearHandler;
		GetWorld()->GetTimerManager().SetTimer(DodgeCdClearHandler,this,&AMultiTestCharacter::ClearDodgeCd,2.5f,false);
	}
	else if(APressed == true)
	{
		PlayMontage(M_DodgeAnims[2]);
		bIsDodgeCd = true;
		FTimerHandle DodgeCdClearHandler;
		GetWorld()->GetTimerManager().SetTimer(DodgeCdClearHandler,this,&AMultiTestCharacter::ClearDodgeCd,2.5f,false);
	}
	else if(DPressed == true)
	{
		PlayMontage(M_DodgeAnims[3]);
		bIsDodgeCd = true;
		FTimerHandle DodgeCdClearHandler;
		GetWorld()->GetTimerManager().SetTimer(DodgeCdClearHandler,this,&AMultiTestCharacter::ClearDodgeCd,2.5f,false);
	}
}

void AMultiTestCharacter::ClearDodgeCd()
{
	bIsDodgeCd = false;
}
// Dodge
//------------------------------------------------------------------------------------------------------------------------------------------------



// Mana Regerenation
//------------------------------------------------------------------------------------------------------------------------------------------------
void AMultiTestCharacter::StartManaRegeneranation()
{
	FTimerHandle ManaRegenTimerHandler;
	GetWorldTimerManager().SetTimer(ManaRegenTimerHandler,this,&AMultiTestCharacter::ManaIncreaseTime,1.f,true);
}

void AMultiTestCharacter::ManaIncreaseTime()
{
	ManaIncreaseTimeServer();
}

void AMultiTestCharacter::ManaIncreaseTimeMultiCast_Implementation()
{
		if(CurrentMana + 1 > MaxMana)
		{
			CurrentMana = MaxMana;
			OnHeadWidgetRef->ManaBar->SetPercent(CurrentMana / MaxMana);
			OnHeadWidgetRef->CurrentManaTxt->SetText(FText::AsNumber(CurrentMana));
		}
		else
		{
			CurrentMana += 1.f;
			OnHeadWidgetRef->ManaBar->SetPercent(CurrentMana / MaxMana);
			OnHeadWidgetRef->CurrentManaTxt->SetText(FText::AsNumber(CurrentMana));
		}
}

void AMultiTestCharacter::ManaIncreaseTimeServer_Implementation()
{
	ManaIncreaseTimeMultiCast();
}
// Mana Regerenation
//------------------------------------------------------------------------------------------------------------------------------------------------



// Set Steam Name And Steam Avatar
//------------------------------------------------------------------------------------------------------------------------------------------------
void AMultiTestCharacter::SetName()
{
	SetNameServer();
}

void AMultiTestCharacter::SetNameMulticast_Implementation()
{
	auto PlayerStateRef = Cast<APlayerState>(GetPlayerState());
	if(PlayerStateRef)
	{
		FString HoldName = PlayerStateRef->GetPlayerName();
		OnHeadWidgetRef->PlayerName->SetText(FText::FromString(HoldName));
		
	}
}

void AMultiTestCharacter::SetNameServer_Implementation()
{
	SetNameMulticast();
}


void AMultiTestCharacter::SetSteamAvatar_Implementation()
{
	// In Bp
}
// Set Steam Name And Steam Avatar
//------------------------------------------------------------------------------------------------------------------------------------------------




// Take Hit And Update Health Widget Interface
// ----------------------------------------------------------------------------------------------------------------------------------------
void AMultiTestCharacter::TakeHit(AActor* AttackerActor,float TakingDamage, UAnimMontage* HitReaction,EPlayerHitEffects HitEffects, UParticleSystem* HitVfx)
{
	if(HitEffects == EPlayerHitEffects::BlindEffect)
	{
		FTimerHandle BlindHandler;
		GetWorldTimerManager().SetTimer(BlindHandler,this,&AMultiTestCharacter::SetBlindEffect,0.7f,false);
	}
	
	if(HasAuthority())
	{
		//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Authority"));
		TakeCustomDamage(TakingDamage);
		if(HitReaction != nullptr)
		{
			PlayMontage(HitReaction);
		}

		if(HitEffects == EPlayerHitEffects::Burn)
		{
			//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Burned!"));
			BurnEffects();
		}
		else if(HitEffects == EPlayerHitEffects::Silence)
		{
			//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Silence!"));
			SilenceEffects();
		}
		else if(HitEffects == EPlayerHitEffects::Slowed)
		{
			//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Silence!"));
			SlowEffect();
		}
		else if(HitEffects == EPlayerHitEffects::LifeSteal)
		{
			//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Silence!"));
			LifeStealEffects(TakingDamage,AttackerActor);
		}
		
	}
	

	if(HitVfx != nullptr)
	{
		SpawnHitVfx(HitVfx);
	}

	

	
}


// Hit Vfx
// -------------------------------------------------------------------------------------------------------------------------------
void AMultiTestCharacter::SpawnHitVfx(UParticleSystem* VfxRef)
{
	SpawnHitVfxServer(VfxRef);
}

void AMultiTestCharacter::SpawnHitVfxMulticast_Implementation(UParticleSystem* VfxRef)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),VfxRef,GetActorLocation(),GetActorRotation(),FVector(1.0f));
}

void AMultiTestCharacter::SpawnHitVfxServer_Implementation(UParticleSystem* VfxRef)
{
	SpawnHitVfxMulticast(VfxRef);
}
// Hit Vfx
// -------------------------------------------------------------------------------------------------------------------------------


void AMultiTestCharacter::UpdateHpNonReplicated(float CurrentHpRef, float MaxHpRef)
{
	OnHeadWidgetRef->CurrentHealthTxt->SetText(FText::AsNumber(CurrentHpRef));
	OnHeadWidgetRef->HealthBar->SetPercent(CurrentHpRef / MaxHpRef);
}




void AMultiTestCharacter::TakeCustomDamage_Implementation(float Damage)
{
	CurrentHp -= Damage;
	UpdateHpNonReplicated(CurrentHp,MaxHp);
	if(CurrentHp <= 0)
	{
		AMultiTestGameMode* GameModeRef = Cast<AMultiTestGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if(GameModeRef && bIsDead == false)
		{
			//PlayAnimMontage(M_DieAnim,1.f);
			bIsDead = true;
			OnHeadWidgetRef->SetVisibility(ESlateVisibility::Collapsed);
			if(PlayerMainUi)
			{
				//PlayerMainUi->RemoveFromParent();
				PlayerMainUi->RemoveFromViewport();
			}
			FTimerHandle DestroyHandle;
			GetWorld()->GetTimerManager().SetTimer(DestroyHandle,this,&AMultiTestCharacter::DestroyCurrentPlayer,0.1f,false);
			//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("GameModeRef İs Valid When Die!"));
			GameModeRef->WhenPlayerDie();
			
		}
		
	}
}

void AMultiTestCharacter::DestroyCurrentPlayer()
{
	//K2_DestroyActor();
	FTimerHandle Handler;
	
	DestroyCurrentPlayerServer();
}

void AMultiTestCharacter::DestroyCurrentPlayerMulticast_Implementation()
{
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetSimulatePhysics(true);
	FTimerHandle ActDestroyHandler;
	//GetWorldTimerManager().SetTimer(ActDestroyHandler,this,&AMultiTestCharacter::DestroyCurrentAct,1.2f,false);
}

void AMultiTestCharacter::DestroyCurrentPlayerServer_Implementation()
{
	DestroyCurrentPlayerMulticast();
}

void AMultiTestCharacter::DestroyCurrentAct()
{
	K2_DestroyActor();
}

// Take Hit And Update Health Widget, Restart Game if player die Interface
// ----------------------------------------------------------------------------------------------------------------------------------------




// Main Widget Add To Viewport When Choisen Class
// ----------------------------------------------------------------------------------------------------------------------------------------
void AMultiTestCharacter::AddMainUiOnViewport()
{
	if(MainPlayerUiWidgetClass)
	{
		PlayerMainUi = Cast<UPlayerMainUI>(CreateWidget(GetWorld(),MainPlayerUiWidgetClass));
		if(PlayerMainUi != nullptr)
		{
			PlayerMainUi->AddToPlayerScreen();
		}
	}
}

void AMultiTestCharacter::SetSkillIcons_Implementation(FName RowName)
{
	// İn Bp
}

// Main Widget Add To Viewport When Choisen Class
// ----------------------------------------------------------------------------------------------------------------------------------------


// Ability Effects
// ----------------------------------------------------------------------------------------------------------------------------------------
void AMultiTestCharacter::BurnEffects()
{
	AttachVfx(EPlayerHitEffects::Burn);
	GetWorld()->GetTimerManager().SetTimer(BurnTimerHandler,this,&AMultiTestCharacter::BurnEffectsMulticast,0.2f,true);
	FTimerHandle ClearHandler;
	GetWorld()->GetTimerManager().SetTimer(ClearHandler,this,&AMultiTestCharacter::ClearBurnEffect,5.f,false);
}

void AMultiTestCharacter::BurnEffectsMulticast_Implementation()
{
	CurrentHp -= 2.f ;
	UpdateHpNonReplicated(CurrentHp,MaxHp);
}

void AMultiTestCharacter::ClearBurnEffect()
{
	GetWorld()->GetTimerManager().ClearTimer(BurnTimerHandler);
}




void AMultiTestCharacter::SilenceEffects()
{
	//AttachVfx(EPlayerHitEffects::Silence);
	bIsSilenced = true;
	ShowSilenceDebuffOnHeadWidget(true);
	
	
	GetWorld()->GetTimerManager().SetTimer(SilenceTimerHandler,this,&AMultiTestCharacter::ClearSilenceEffect,5.f,false);
}

void AMultiTestCharacter::ShowSilenceDebuffOnHeadWidget_Implementation(bool bIsShow)
{
	if(PlayerMainUi)
	{
		if(bIsShow == true)
		{
			PlayerMainUi->Skill1SilenceImg->SetVisibility(ESlateVisibility::Visible);
			PlayerMainUi->Skill2SilenceImg->SetVisibility(ESlateVisibility::Visible);
			PlayerMainUi->Skill3SilenceImg->SetVisibility(ESlateVisibility::Visible);
			PlayerMainUi->Skill4SilenceImg->SetVisibility(ESlateVisibility::Visible);
			PlayerMainUi->Skill5SilenceImg->SetVisibility(ESlateVisibility::Visible);
			PlayerMainUi->Skill6SilenceImg->SetVisibility(ESlateVisibility::Visible);
			PlayerMainUi->Skill7SilenceImg->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			PlayerMainUi->Skill1SilenceImg->SetVisibility(ESlateVisibility::Collapsed);
			PlayerMainUi->Skill2SilenceImg->SetVisibility(ESlateVisibility::Collapsed);
			PlayerMainUi->Skill3SilenceImg->SetVisibility(ESlateVisibility::Collapsed);
			PlayerMainUi->Skill4SilenceImg->SetVisibility(ESlateVisibility::Collapsed);
			PlayerMainUi->Skill5SilenceImg->SetVisibility(ESlateVisibility::Collapsed);
			PlayerMainUi->Skill6SilenceImg->SetVisibility(ESlateVisibility::Collapsed);
			PlayerMainUi->Skill7SilenceImg->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("PlayerMainUi Ref is not valid! cpp 1137"));
	}
	
	if(bIsShow == true)
	{
		OnHeadWidgetRef->PlayerSilenceImage->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		OnHeadWidgetRef->PlayerSilenceImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AMultiTestCharacter::ClearSilenceEffect()
{
	bIsSilenced = false;
	ShowSilenceDebuffOnHeadWidget(false);
}




void AMultiTestCharacter::SlowEffect()
{
	AttachVfx(EPlayerHitEffects::Slowed);
	SlowEffectMulticast(250.f);
	FTimerHandle ClearHandler;
	FTimerDelegate ClearDelegate = FTimerDelegate::CreateUObject( this, &AMultiTestCharacter::SlowEffectMulticast, 500.f );
	GetWorldTimerManager().SetTimer(ClearHandler, ClearDelegate, 5.f, false );
}

void AMultiTestCharacter::SlowEffectMulticast_Implementation(float NewWalkSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewWalkSpeed;
}




void AMultiTestCharacter::LifeStealEffects(float StealingLife, AActor* HoldAttackerActor)
{
	AMultiTestCharacter* AttackerRef = Cast<AMultiTestCharacter>(HoldAttackerActor);
	if(AttackerRef)
	{
		if(AttackerRef->CurrentHp + 30 >= AttackerRef->MaxHp)
		{
			AttackerRef->CurrentHp = AttackerRef->MaxHp;
			AttackerRef->LifeStealHealMulticast(AttackerRef->CurrentHp,AttackerRef->MaxHp);
		}
		else
		{
			AttackerRef->CurrentHp += 30.f;
			AttackerRef->LifeStealHealMulticast(AttackerRef->CurrentHp,AttackerRef->MaxHp);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("AttackerRef İs Not Valid! cpp 1244"));
	}
	//LifeStealEffectsServer(StealingLife,HoldAttackerActor);
}


void AMultiTestCharacter::SetBlindEffect_Implementation()
{
	
}

void AMultiTestCharacter::SetShadowEffectToCamera_Implementation()
{
	
}


// Assassin Skill 7
// -----------------------------------------------------------------------------------------------------------------------
void AMultiTestCharacter::SetMeshVisiblityAuthority()
{
	GetMesh()->SetVisibility(true);
	SetShadowEffectToCamera();
	DaggerMesh->SetVisibility(true);
	OnHeadWidgetRef->SetVisibility(ESlateVisibility::Visible);
}

void AMultiTestCharacter::HideMeshMulticastAuthority_Implementation(bool bIsVisible, ESlateVisibility SlateVisiblity)
{
	GetMesh()->SetVisibility(bIsVisible);
	DaggerMesh->SetVisibility(bIsVisible);
	OnHeadWidgetRef->SetVisibility(SlateVisiblity);
	
}

void AMultiTestCharacter::HideMeshServerAuthority_Implementation(bool bIsVisible, ESlateVisibility SlateVisiblity)
{
	HideMeshMulticastAuthority(bIsVisible,SlateVisiblity);
	
}



void AMultiTestCharacter::SetMeshVisiblityRemote_Implementation(bool bIsVisible)
{
	SetShadowEffectToCamera();
	GetMesh()->SetVisibility(bIsVisible);
	DaggerMesh->SetVisibility(bIsVisible);
}

void AMultiTestCharacter::HideMeshMulticastRemote_Implementation(bool bIsVisible)
{
	GetMesh()->SetVisibility(bIsVisible);
	DaggerMesh->SetVisibility(bIsVisible);
	
}

void AMultiTestCharacter::HideMeshServerRemote_Implementation(bool bIsVisible, ESlateVisibility SlateVisibility)
{
	GetMesh()->SetVisibility(bIsVisible);
	DaggerMesh->SetVisibility(bIsVisible);
	OnHeadWidgetRef->SetVisibility(SlateVisibility);
	
}
// Assassin Skill 7
// -----------------------------------------------------------------------------------------------------------------------

void AMultiTestCharacter::LifeStealHealMulticast_Implementation(float NewHP, float NewMaxHP)
{
	OnHeadWidgetRef->CurrentHealthTxt->SetText(FText::AsNumber(NewHP));
	OnHeadWidgetRef->HealthBar->SetPercent(NewHP / NewMaxHP);
	
}

void AMultiTestCharacter::LifeStealHealServer_Implementation(float NewHP, float NewMaxHP)
{
	OnHeadWidgetRef->CurrentHealthTxt->SetText(FText::AsNumber(NewHP));
	OnHeadWidgetRef->HealthBar->SetPercent(NewHP / NewMaxHP);
}


void AMultiTestCharacter::AttachVfx_Implementation(EPlayerHitEffects EffectsType)
{
	// In Bp
}
// Ability Effects
// ----------------------------------------------------------------------------------------------------------------------------------------


// Camera Shake Triggered In AnimNotify
// ----------------------------------------------------------------------------------------------------------------------------------------
void AMultiTestCharacter::PlayCameraShake_Implementation()
{
	// In Bp
}
// Camera Shake Triggered In AnimNotify
// ----------------------------------------------------------------------------------------------------------------------------------------




// Key Pressed
//--------------------------------------------------------------------------------------------------------------------
void AMultiTestCharacter::WPress()
{
	WPressed = true;
}

void AMultiTestCharacter::WRelease()
{
	WPressed = false;
}

void AMultiTestCharacter::SPress()
{
	SPressed = true;
}

void AMultiTestCharacter::SRelease()
{
	SPressed = false;
}

void AMultiTestCharacter::APress()
{
	APressed = true;
}

void AMultiTestCharacter::ARelease()
{
	APressed = false;
}

void AMultiTestCharacter::DPress()
{
	DPressed = true;
}

void AMultiTestCharacter::DRelease()
{
	DPressed = false;
}
// Key Pressed
//--------------------------------------------------------------------------------------------------------------------


void AMultiTestCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMultiTestCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		AddMovementInput(Direction, Value);
	}
}

void AMultiTestCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AMultiTestCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AMultiTestCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AMultiTestCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}
