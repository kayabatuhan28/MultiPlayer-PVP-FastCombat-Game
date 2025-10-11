// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "Interfaces/PlayerInterface.h"
#include "Widgets/OnHeadHealthWidget.h"
#include "Widgets/PlayerMainUI.h"
#include "MultiTestCharacter.generated.h"


UENUM(BlueprintType)
enum EPlayerType
{
	Default			UMETA(DisplayName = "Default"),
	Assassin		UMETA(DisplayName = "Assassin"),
	Mage			UMETA(DisplayName = "Mage"),
};

UENUM(BlueprintType)
enum EPlayerActiveState
{
	Empty			UMETA(DisplayName = "Empty"),
	Attack			UMETA(DisplayName = "Attack"),
	Dodge			UMETA(DisplayName = "Dodge"),
	Hitted			UMETA(DisplayName = "Hitted"),
};



USTRUCT(BlueprintType)
struct FPlayerDataStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TEnumAsByte<EPlayerType> DT_ChoisenType;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float DT_HealthMax;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float DT_ManaMax;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<UTexture2D*> DT_SkillIcons;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<TSubclassOf<AActor>> DT_Skills;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UAnimMontage*> DT_SkillsAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> DT_SkillsCastTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> DT_SkillsCooldown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> DT_SkillsManaCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* DT_EquipAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* DT_UnEquipAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UAnimMontage*> DT_DodgeAnims;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DT_DashCd;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DT_DashManaCost;
	
};



UCLASS(config=Game)
class AMultiTestCharacter : public ACharacter, public IPlayerInterface
{
	GENERATED_BODY()

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
public:
	
	AMultiTestCharacter();

	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const;
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	//Static Meshes
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Meshes",Replicated)
	UStaticMeshComponent* DaggerMesh;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Meshes",Replicated)
	UStaticMeshComponent* StaffMesh;
	
	//Enums
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enums",Replicated)
	TEnumAsByte<EPlayerType> PlayerType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enums",Replicated)
	TEnumAsByte<EPlayerActiveState> ActiveState;

	//Data Table
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Data Table")
	UDataTable* PlayerDataTable;
	UFUNCTION(BlueprintCallable)
	void SetPlayerData(FName RowName);
	UFUNCTION(NetMulticast,Unreliable)
	void SetPlayerDataMulticast(FName RowName);
	UFUNCTION(Server,Unreliable)
	void SetPlayerDataServer(FName RowName);
	void SetPlayerDataStruct(FPlayerDataStruct* ReceivedData);

	
	// Widget Components
	UPROPERTY(Replicated,EditDefaultsOnly,BlueprintReadWrite,Category="Components")
	UWidgetComponent* OnHeadWidgetComponent;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Components")
	UOnHeadHealthWidget* OnHeadWidgetRef;


	// Main Player Gui
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Player Widget")
	TSubclassOf<UUserWidget> MainPlayerUiWidgetClass;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Player Widget")
	UPlayerMainUI* PlayerMainUi;
	UFUNCTION(BlueprintCallable)
	void AddMainUiOnViewport();
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void SetSkillIcons(FName RowName);

	// Attribute Variables
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Attribute Variables",Replicated)
	float CurrentHp;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Attribute Variables",Replicated)
	float MaxHp;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Attribute Variables",Replicated)
	float CurrentMana;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Attribute Variables",Replicated)
	float MaxMana;
	
	// Integer
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Skills",Replicated)
	int LightAttackIndex;
	
	// Player Skills
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Skills",Replicated)
	TArray<UTexture2D*> SkillIcon;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Skills",Replicated)
	TArray<TSubclassOf<AActor>> SkillsActor;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Skills",Replicated)
	TArray<UAnimMontage* > SkillsAnim;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Skills",Replicated)
	TArray<float> SkillCastTime;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Skills",Replicated)
	TArray<float> SkillCooldown;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Skills",Replicated)
	TArray<float> SkillManaCost;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Skills",Replicated)
	TArray<bool> bIsSkillCooldown;
	

	// Boolean State Variables
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Boolean States",Replicated)
	bool bIsWeaponEquipped;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Replicated)
	bool bIsDead;

	// On Head Widget Functions	Update Health Mana
	UFUNCTION(BlueprintCallable)
	void UpdateHpWidget(float CurrentHealthRef, float MaxHealthRef);
	UFUNCTION(NetMulticast,Unreliable)
	void UpdateHpWidgetMulticast(float CurrentHealthRef, float MaxHealthRef);
	UFUNCTION(Server,Unreliable)
	void UpdateHpWidgetServer(float CurrentHealthRef, float MaxHealthRef);

	
	UFUNCTION(BlueprintCallable)
	void UpdateManaWidget(float CurrentManaRef, float MaxManaRef);
	UFUNCTION(NetMulticast,Unreliable)
	void UpdateManaWidgetMulticast(float CurrentManaRef, float MaxManaRef);
	UFUNCTION(Server,Unreliable)
	void UpdateManaWidgetServer(float CurrentManaRef, float MaxManaRef);

	// Animations
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Animations",Replicated)
	UAnimMontage* M_EquipAnim;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Animations",Replicated)
	UAnimMontage* M_UnEquipAnim;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Animations",Replicated)
	TArray<UAnimMontage*> M_DodgeAnims;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Animations",Replicated)
	UAnimMontage* M_DieAnim;
	
	UFUNCTION(BlueprintCallable)
	void SetStartData(float HealthRef,float MaxHealthRef, float ManaRef, float MaxManaRef);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

	// Dash Or Tp
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Dash",Replicated)
	TArray<UAnimMontage*> M_Dash;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Dash",Replicated)
	UParticleSystem* DashVfx;
	UFUNCTION()
	void DashOrTp();
	UFUNCTION(NetMulticast,Unreliable)
	void DashOrTpMulticast(int TpRot);
	UFUNCTION(Server,Unreliable)
	void DashOrTpServer(int TpRot);
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Dash",Replicated)
	float DashManaCost;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Dash",Replicated)
	float DashCooldown;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Dash",Replicated)
	bool bIsDashCd;
	void ClearDashCd();
	int MageDashCount;
	

	// EquipWeapon
	UFUNCTION()
	void EquipWeapon();
	UFUNCTION(Server,Unreliable)
	void EquipWeaponServer(bool IsEquip);
	UFUNCTION(NetMulticast,Unreliable)
	void EquipWeaponMultiCast(bool IsEquip);

	// Replication Play Montages
	UFUNCTION(BlueprintCallable)
	void PlayMontage(UAnimMontage* PlayedMontage);
	UFUNCTION(Server,Unreliable)
	void PlayMontageServer(UAnimMontage* PlayedMontage);
	UFUNCTION(NetMulticast,Unreliable)
	void PlayMontageMulticast(UAnimMontage* PlayedMontage);

	
	// Cast Skills
	DECLARE_DELEGATE_OneParam(FCastSkillDelegate, int);
	UFUNCTION(BlueprintCallable)
	void CastSkill(int SkillIndex);
	UFUNCTION(Server,Unreliable)
	void CastSkillServer(int SkillIndex);

	// Dodge
	void DodgeFunction();
	bool bIsDodgeCd;
	void ClearDodgeCd();
	

	// For Skill Cooldowns
	// ----------------------------------------------------------------------------------------------------------------------------------
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void ClearSkillCooldown(int CooldownIndex);

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category= "SkillCd")
	float Skill1Cd;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category= "SkillCd")
	float Skill2Cd;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category= "SkillCd")
	float Skill3Cd;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category= "SkillCd")
	float Skill4Cd;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category= "SkillCd")
	float Skill5Cd;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category= "SkillCd")
	float Skill6Cd;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category= "SkillCd")
	float Skill7Cd;

	void ClearSkill1();
	void ClearSkill2();
	void ClearSkill3();
	void ClearSkill4();
	void ClearSkill5();
	void ClearSkill6();
	void ClearSkill7();

	FTimerHandle Skill1Handle;
	FTimerHandle Skill2Handle;
	FTimerHandle Skill3Handle;
	FTimerHandle Skill4Handle;
	FTimerHandle Skill5Handle;
	FTimerHandle Skill6Handle;
	FTimerHandle Skill7Handle;
	// For Skill Cooldowns
	// ----------------------------------------------------------------------------------------------------------------------------------


	// For Skill Cooldowns
	// ----------------------------------------------------------------------------------------------------------------------------------
	UFUNCTION()
	void StartManaRegeneranation();
	UFUNCTION()
	void ManaIncreaseTime();
	UFUNCTION(NetMulticast,Unreliable)
	void ManaIncreaseTimeMultiCast();
	UFUNCTION(Server,Unreliable)
	void ManaIncreaseTimeServer();
	// For Skill Cooldowns
	// ----------------------------------------------------------------------------------------------------------------------------------


	// Set Steam Name
	UFUNCTION(BlueprintCallable)
	void SetName();
	UFUNCTION(NetMulticast,Unreliable)
	void SetNameMulticast();
	UFUNCTION(Server,Unreliable)
	void SetNameServer();
	// Steam Avatar Set
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void SetSteamAvatar();

	// Destroy Actor With Timer
	void DestroyCurrentPlayer();
	UFUNCTION(NetMulticast,Reliable)
	void DestroyCurrentPlayerMulticast();
	UFUNCTION(Server,Reliable)
	void DestroyCurrentPlayerServer();
	void DestroyCurrentAct();
	

	
	//Interfaces
	UFUNCTION(BlueprintCallable)
	virtual void TakeHit(AActor* AttackerActor,float TakingDamage, UAnimMontage* HitReaction, EPlayerHitEffects HitEffects, UParticleSystem* HitVfx) override;

	// Hit Vfx
	UFUNCTION()
	void SpawnHitVfx(UParticleSystem* VfxRef);
	UFUNCTION(Server,Reliable)
	void SpawnHitVfxServer(UParticleSystem* VfxRef);
	UFUNCTION(NetMulticast,Reliable)
	void SpawnHitVfxMulticast(UParticleSystem* VfxRef);

	// Taking Damage
	UFUNCTION(NetMulticast,Reliable)
	void TakeCustomDamage(float Damage);
	void UpdateHpNonReplicated(float CurrentHpRef, float MaxHpRef);

	UFUNCTION()
	FRotator SetActorRotationToLineTraceHit(FVector StartLoc);

	// Ability Effects

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void AttachVfx(EPlayerHitEffects EffectsType);
	
	UFUNCTION()
	void BurnEffects();
	UFUNCTION(NetMulticast,Reliable)
	void BurnEffectsMulticast();
	UFUNCTION()
	void ClearBurnEffect();
	FTimerHandle BurnTimerHandler;

	UFUNCTION()
	void SilenceEffects();
	UFUNCTION(NetMulticast,Unreliable)
	void ShowSilenceDebuffOnHeadWidget(bool bIsShow);
	UFUNCTION()
	void ClearSilenceEffect();
	FTimerHandle SilenceTimerHandler;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Ability Effects",Replicated)
	bool bIsSilenced;

	UFUNCTION()
	void SlowEffect();
	UFUNCTION(NetMulticast,Reliable)
	void SlowEffectMulticast(float NewWalkSpeed);
	FTimerHandle SlowTimerHandler;

	UFUNCTION()
	void LifeStealEffects(float StealingLife,AActor* HoldAttackerActor);
	UFUNCTION(NetMulticast,Reliable)
	void LifeStealHealMulticast(float NewHP, float NewMaxHP);
	UFUNCTION(Server,Reliable)
	void LifeStealHealServer(float NewHP, float NewMaxHP);

	// For Blind Effect
	UFUNCTION(BlueprintNativeEvent)
	void SetBlindEffect();
	

	// AssassinSkill 7 Shadow Skill
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void SetShadowEffectToCamera();
	// -- when player by server - Authority
	void SetMeshVisiblityAuthority();
	UFUNCTION(NetMulticast,Reliable)
	void HideMeshMulticastAuthority(bool bIsVisible,ESlateVisibility SlateVisiblity);
	UFUNCTION(Server,Reliable)
	void HideMeshServerAuthority(bool bIsVisible,ESlateVisibility SlateVisiblity);
	// -- when player by client - remote
	UFUNCTION(Client,Reliable)
	void SetMeshVisiblityRemote(bool bIsVisible);
	UFUNCTION(NetMulticast,Unreliable)
	void HideMeshMulticastRemote(bool bIsVisible);
	UFUNCTION(Server,Reliable)
	void HideMeshServerRemote(bool bIsVisible,ESlateVisibility SlateVisibility);

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Shadow Vfx")
	UParticleSystem* ShadowEndVfx;
	UFUNCTION(NetMulticast,Reliable)
	void SpawnShadowVfxMulticast();
	UFUNCTION(Server,Reliable)
	void SpawnShadowVfxServer();

	// For Camera Shake
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void PlayCameraShake();
	
	// Directional Key Check
	UPROPERTY(BlueprintReadWrite,Replicated)
	bool WPressed;
	void WPress();
	void WRelease();

	UPROPERTY(BlueprintReadWrite,Replicated)
	bool SPressed;
	void SPress();
	void SRelease();

	UPROPERTY(BlueprintReadWrite,Replicated)
	bool APressed;
	void APress();
	void ARelease();

	UPROPERTY(BlueprintReadWrite,Replicated)
	bool DPressed;
	void DPress();
	void DRelease();

	
protected:
	
	void MoveForward(float Value);
	
	void MoveRight(float Value);
	
	void TurnAtRate(float Rate);
	
	void LookUpAtRate(float Rate);
	
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
	
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	


	
	
};

