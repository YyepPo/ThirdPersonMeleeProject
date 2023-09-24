// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "Treasure/ItemInteractionInterface.h"
#include "SlashCharacter.generated.h"

class UPlayerSkillComponent;
class USpringArmComponent;
class UCameraComponent;
class UBoxComponent;
class UCapsuleComponent;
class AItem;
class ABaseShield;
class AWeapon;
class AEnemy;
class USlashOverlay;
class IInteraction;
class UInventory;

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_NoWeaponEquipped UMETA(DisplayName = "UnEquipped"),
	ECS_OneHandWeaponEquipped UMETA(DisplayName = "OneHandWeaponEquipped"),
	ECS_ShieldEquipped UMETA(DisplayName = "Shield Equipped"),
	ECS_Hanging UMETA(DisplayName = "Hanging")
};
UENUM(BlueprintType)
enum class EActionState :uint8
{
	//Player is doing something else (is not attacking)
	EAS_Dead UMETA(DisplayName = "Dead"),
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_HitReaction UMETA(DisplayName = "HitReaction"),
	EAS_Attacking UMETA(DisplayName = "Attacking")
};
UENUM(BlueprintType)
enum class EDodgeState : uint8
{
	EDS_HasDodge UMETA(DisplayName = "HasDodge"),
	EDS_Dodging UMETA(DisplayName = "Dodging")
};
UENUM(BlueprintType)
enum class ELockOnState : uint8
{
	ELS_IsLockedOn UMETA(DisplayName = "Is Lock On"),
	ELS_IsLockedOff UMETA(DisplayName = "Is Lock Off")
};

UCLASS()
class SLASH_API ASlashCharacter : public ACharacterBase,public IItemInteractionInterface
{
	GENERATED_BODY()

public:
	ASlashCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit_Implementation(const FVector& HitLocation,AActor* Hitter) override;

	UFUNCTION(BlueprintCallable,BlueprintPure)
	FORCEINLINE UInventory* GetInventoryComponent() { return InventoryClass; }
protected:
	virtual void BeginPlay() override;
	virtual void Die() override;
private:
	void InitiliazeOverlay();
	void InitializeSkillComponents();

#pragma  region Components
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		USpringArmComponent* SpringArm;
private:
	UPROPERTY(VisibleAnywhere)
		UInventory* InventoryClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "TRUE"))
		UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,meta = (AllowPrivateAccess = "TRUE"))
		UPlayerSkillComponent* PlayerSkillComponent;
	UPROPERTY(EditAnywhere, Category = "Movement")
		float TurnRightInterpSpeed;

	float SmoothValue = 0.f;
	UPROPERTY(VisibleAnywhere)
		UBoxComponent* ToInteractWithCharacterCollider;

	//Camera bobing
		float Time;
	UPROPERTY(EditAnywhere)
		float Speed;
	UPROPERTY(EditAnywhere)
		float Amplitude;

#pragma endregion

#pragma region Enums
private:

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "True"), VisibleInstanceOnly)
		ECharacterState CharacterState = ECharacterState::ECS_NoWeaponEquipped;
	UPROPERTY(BlueprintReadWrite,meta = (AllowPrivateAccess = "True"), VisibleInstanceOnly)
		EActionState ActionState = EActionState::EAS_Unoccupied;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "True"), VisibleInstanceOnly)
		EDodgeState DodgeState = EDodgeState::EDS_HasDodge;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "True"), VisibleInstanceOnly)
		ELockOnState LockOnState = ELockOnState::ELS_IsLockedOff;

#pragma endregion Enums

#pragma region Montages

private:
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
		UAnimMontage* DogeAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
		UAnimMontage* WeaponEquipMAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
		UAnimMontage* WeaponUnequipAnim;
#pragma endregion Montages

#pragma region Binding functions

private:
	void Forward(float value);
	void Right(float value);
	void WeaponSinusoidalMovement(float value);//is not a binding function
	void LookUp(float value);
	void LookRight(float value);
	void Jump();

	void DodgeRight();
	void DodgeLeft();
	void DodgeBackwards();

	void LockOn();

	void ShieldHold();
	void ShieldRelease();
	void Block();

	//not binding functions
	void HangMovement(float& value,bool IsRight);
#pragma endregion

#pragma region Geters
public:
	UFUNCTION(BlueprintCallable,BlueprintPure) 
		AWeapon* GetCurrentWeapon();
	UCharacterMovementComponent* GetPlayerMovementComponent();
	FORCEINLINE UAttributeComponent* GetPlayerAttributeComponent() { return AttributeComponent; }
	UFUNCTION(BlueprintCallable)
		void HealthBoostAttributeComp(float boostAmount);
	UFUNCTION(BlueprintCallable)
		void StaminaBoostAttributeComp(float boostAmount);
#pragma endregion Geters

	UPROPERTY(VisibleAnywhere)
	AItem* OverlapingItem;

#pragma region SlashOverlay
public:
	void UpdateHealthAndStaminaPerc();
private:
	UPROPERTY()
		USlashOverlay* SlashOverlay;
#pragma endregion SlashOverlay


#pragma region Combat
public:
	UPROPERTY(EditAnywhere, Category = "Attack")
		float OneHandedMovementSpeed = 300.f;
	UPROPERTY(EditAnywhere, Category = "Attack")
		float OnLockOnMovementSpeed = 150.f;
	UPROPERTY(EditAnywhere, Category = "Attack")
		float WrapTargetDistance = 75.f;
	bool GetIsAttacking() { return IsAttacking(); }
	UFUNCTION(BlueprintCallable)
		FVector GetTargetWarpTransform();
	UFUNCTION(BlueprintCallable)
		FVector GetTargetWarpRotation();
	FORCEINLINE bool IsPlayerDead() { return ActionState == EActionState::EAS_Dead; }
	UFUNCTION(BlueprintCallable)
		void ConsumeStaminaAnimNotify();
private:
	UPROPERTY(EditAnywhere, Category = "Stamina Consume")
		float AttackStaminConsumeAmount;
	UPROPERTY(EditAnywhere, Category = "Stamina Consume")
		float DodgeStaminaConsumeAmount;
	UPROPERTY(EditAnywhere, Category = "Stamina Consume")
		float ShieldBlockStaminaConsumeAmount;
	UPROPERTY(EditAnywhere, Category = "Stamina Consume")
		float ShieldHitStaminaConsumeAmount = 10;
	UPROPERTY(EditAnywhere, Category = "Stamina Consume")
		float TimeToStaminaRecharge = 5.f;
	UPROPERTY(EditAnywhere, Category = "Stamina Consume")
		float StaminaOverTimeAmount;
	UPROPERTY(EditAnywhere, Category = "Stamina Consume")
		float StaminaInterpSpeed;
	FTimerHandle StaminaTimerHandle;
	void StaminaTimer();
	void ConsumeStamina(float ConsumeAmount);

	APlayerController* Controller;
	ABaseShield* CurrentShield;
	bool isHoldingShieldKey;
	bool canStunn;
	UPROPERTY(EditDefaultsOnly, Category = "Shield Block")
		UAnimMontage* ShieldBlockOnMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Shield Block")
		UAnimMontage* ShieldHitReaction;
	UPROPERTY(EditDefaultsOnly, Category = "Shield Block")
		TSubclassOf<	UCameraShakeBase >BlockShake;

	UFUNCTION()
		FORCEINLINE	bool IsAttacking() { return ActionState == EActionState::EAS_Attacking && !IsUpperBody; }
	virtual bool CanAttack() override;
	virtual void Attack() override;
	virtual void AttackEnd() override; //anim notify
	bool IsCombatTargetValid();
	FORCEINLINE bool IsBlocking() { return CurrentShield && isHoldingShieldKey; }
protected:
	virtual void PlatAttackMontage() override;
	UFUNCTION(BlueprintCallable)
		void HitReactEnd();

	UFUNCTION(BlueprintCallable)
		FORCEINLINE bool SetCanStunn(bool newBool);
	UFUNCTION(BlueprintCallable)
		FORCEINLINE bool GetCanStunn() { return canStunn; }
	UFUNCTION(BlueprintImplementableEvent)
		void ShieldSlowMotion();
#pragma endregion Combat

#pragma region Equip
private:
	//binding  functions
	void ToEquip();
	void EquipAnim();
	void EquipShield(ABaseShield* Shield);

	bool bWeaponIsAttachedToBack;

	UFUNCTION()
		bool CanDisarm();
	UFUNCTION()
		bool CanArm();
	void Disarm();
	void Arm();
	UFUNCTION()
		void PlayEquipAnim(UAnimMontage* MontageName);
	UPROPERTY(EditAnywhere, Category = "Equip")
		float sinSpeed;
	UPROPERTY(EditAnywhere, Category = "Equip")
		float WeaponEquipedAmplitude;
public:
	void EquipWeapon(AWeapon* Weapon);
	UFUNCTION(BlueprintCallable)
		void AttachWeaponToRightHand();
	UFUNCTION(BlueprintCallable)
		void AttachWeaponToBack();
private:
	
#pragma endregion Equip

#pragma region Dodge
private:
	UPROPERTY(EditAnywhere,Category = "Dodge")
		float DodgeSpeed;
	UPROPERTY(EditAnywhere, Category = "Dodge")
		float ForwardDashSpeed;
	UFUNCTION()
		FORCEINLINE	bool IsDodging() { return DodgeState == EDodgeState::EDS_Dodging; }
	bool CanPerformDodge();
	int32 DashCounter = 0;

#pragma endregion Dodge

#pragma region CameraAndSpringArm
private:
	UPROPERTY(EditAnywhere, Category = "SpringArm")
		float ZoomInAmount;
	UPROPERTY(EditAnywhere, Category = "SpringArm")
		float ZoomOutAmount;
	UPROPERTY(EditAnywhere, Category = "SpringArm")
		float InterpSpeed;
	UPROPERTY(EditAnywhere,Category = "SpringArm")
		float OnMoveRightSpringArmSocketOffSet;
	float DeltaTimeTracker;

	UFUNCTION()
		void CameraBobing(float	SinusoidalSpeed);
	bool IsVelocityEqaulTo(float Velocity);
	void SpringArmZoomIn(float Delta);
	void SpringArmZoomOut(float Delta);
	UFUNCTION()
		void ZoomInOut(float deltaTime, float ZoomInOutAmount);

#pragma endregion CameraAndSpringArm

#pragma region Skills

private:
	void SkillCoolDowns(float deltaTime);

#pragma region SkillOne
protected:
	UFUNCTION()
		void OnQSkillBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnQSkillEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
private:
	void UseSkillOne();//binding function

	UPROPERTY(VisibleAnywhere)
		UCapsuleComponent* QSkillCollider;
	float QSkillCDRemainingTime = 0.f;
#pragma endregion SkillOne

#pragma region SkillTwo
protected:
	UFUNCTION()
		void OnCSkillBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnCSkillEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
private:
	UPROPERTY(VisibleAnywhere)
		UBoxComponent* CSkillCollider;
	UPROPERTY(VisibleAnywhere)
		USceneComponent* CSkillVFXLoc;

	void UseSkillTwo();
public:
	UFUNCTION()
		USceneComponent* GetCSkllVFXLoc() { return CSkillVFXLoc; }
#pragma endregion SkillTwo

#pragma region SkillThree

private:
	void UseSkillThree();
	UPROPERTY(VisibleAnywhere)
		USceneComponent* SkillThreeBeamSpawnPoint;
public: 
	UFUNCTION(BlueprintCallable)
		void GetSkillThreeBehaviour();
	USceneComponent* GetSkillThreeBeamSpawnPoint() { return SkillThreeBeamSpawnPoint; }
	UFUNCTION(BlueprintImplementableEvent)
		void OnSkillThreeUsed();
#pragma endregion SkillThree

#pragma endregion Skills

#pragma region UpperBody
private:
	bool IsUpperBody;
public:
	UFUNCTION(BlueprintCallable)
		void  SetUpperBody(bool NewUpperBody) { IsUpperBody = NewUpperBody; }
	UFUNCTION(BlueprintCallable)
		bool GetIsUpperBody() { return IsUpperBody; }
#pragma endregion UpperBody

#pragma region LockOnToEnemy
public:
	UFUNCTION(BlueprintImplementableEvent)
		void OnLockOnEvent();
	UFUNCTION(BlueprintImplementableEvent)
		void OnLockOffEvent();
private:
	UPROPERTY(EditAnywhere, Category = "Lock On To Target")
		float ToLookRotationInterpSpeed;

	UPROPERTY(VisibleAnywhere,Category = "Lock On To Target")
		AEnemy* CurrentEnemy;
	UPROPERTY(VisibleAnywhere, Category = "Lock On To Target")
	TArray<AEnemy*> Enemies;
	int32 TargetIndex = 0;

	bool CanDisengage = false;

	UPROPERTY(EditAnywhere, Category = "Lock On To Target")
		float MaxEngageDistance;
	UPROPERTY(EditAnywhere, Category = "Lock On To Target")
		float CameraFovOnLock;
	UPROPERTY(EditAnywhere, Category = "Lock On To Target")
		float FovInterpSpeed;
	UPROPERTY(EditAnywhere, Category = "Lock On To Target")
		float CameraFov0ffLock;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "TRUE"));
		UCapsuleComponent* LockOnCollider;

	bool IsLockedOn = false;

	UFUNCTION()
		void OnLockedOn();
	UFUNCTION()
		void LockOnToEnemy();
	UFUNCTION()
		void SwapEnemies();
	UFUNCTION()
		void EngageLock();
	UFUNCTION()
		void DisengageLock();
	void LockOff();//binding function
	UFUNCTION()
		void LockOnParticle(bool cked);
	UFUNCTION()
		void SetLockOnState(ELockOnState State);
	UFUNCTION()
		void LockOnColliderBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void LockOnColliderEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

#pragma endregion LockOnToEnemy
	
#pragma region Assassination
public: 
	void SetAssassinationTarget(AEnemy* NewTarget);
	UFUNCTION(BlueprintCallable)
		FTransform UpdateASTargetTransform();
	void SetAssassinationOpacity(float Percent);
private:
	UPROPERTY(EditAnywhere, Category = "Assassination")
		UAnimMontage* AssassianteMontage;
	UPROPERTY(VisibleAnywhere ,Category = "Assassination")
		AEnemy* AssassinationTarget;
	UPROPERTY(EditAnywhere, Category = "Assassination")
		double ASLength;
	//input binding
	void Assassinate();
#pragma endregion Assassination

private:
	bool CanInteractWithItem();
	IInteraction* CurrentCharacterInteraction;
public:
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddSouls(class ASoul* Soul) override;
	virtual void AddGold(class ATreasure* Gold) override;
	void IncreaseGold(float GoldAmount);
protected:
	UFUNCTION()
		void OnInteractionColliderBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnInteractionColliderEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

#pragma region EnemiesAttackAtOnce
private:
	AEnemy* ClosestEnemy;
	UPROPERTY(VisibleAnywhere)
	TArray <AEnemy*> NotClosestEnemies;
	AEnemy* NotClosestEnemy;
	void CalculateClosestEnemy();
	FVector DistanceForClosestEnemy;
	FVector DistanceBtwEnemies;
public:
	bool GetClosestEnemy(AEnemy* close) { return ClosestEnemy == close; }
#pragma endregion EnemiesAttackAtOnce

#pragma region Hang
private:
	void Hang(); // input binding

	UPROPERTY(EditDefaultsOnly, Category = "Hang")
		UAnimMontage* ToHangMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Hang")
		UAnimMontage* HangToCrouchMontage;

	UPROPERTY(EditAnywhere,Category = "Hang")
		float UpVectorLength;
	UPROPERTY(EditAnywhere, Category = "Hang")
		float SphereRadius;
	UPROPERTY(EditAnywhere, Category = "Hang")
		float SideWaysShpereDistance;
	UPROPERTY(EditAnywhere, Category = "Hang")
		float UpVectorDistance;
	UPROPERTY(EditAnywhere, Category = "Hang")
		float ForwardVectorDistance;
	UPROPERTY(EditAnywhere, Category = "Hang")
		FVector HangPosition;
	UPROPERTY(EditAnywhere, Category = "Hang")
		FVector HangUpSpherePosition;
	FTransform HitPoint;
	FVector HangUpHitPoint;
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FTransform GetHangTransform();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FVector GetHangUpHitPoint();
#pragma endregion Hang
};
