// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "Enemy.generated.h"

class UEnemyHealthBar;
class UDamagePopUpText;
class UPawnSensingComponent;
class UBoxComponent;
class UCapsuleComponent;
class AItem;

UENUM(BlueprintType)
enum class EEnemyLocomotionState : uint8
{
	ELS_Idle UMETA(DisplayName = "Idle"),
	ELS_Walking UMETA(DisplayName = "Walking"),
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Stunned UMETA(DisplayName = "Stunned"),
	EES_Patroling UMETA(DisplayName = "Patroling"),
	EES_InvestigatePatrol UMETA(DisplayName = "InvestigatePatrol"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_Engaged UMETA(DisplayName = "Engaged"),

	ESS_NoState UMETA(DisplayName = "NoState")
};

UENUM(BlueprintType)
enum class EEnemyWeaponState : uint8
{
	EEWS_Equiped UMETA(DisplayName = "Equiped"),
	EEWS_Unequiped UMETA(DisplayName = "Unequiped")
};
UENUM(BlueprintType)
enum  EDeathPose 
{
	EDS_Death1	UMETA(DisplayName = "Death1"),
	EDS_Death2	UMETA(DisplayName = "Death2"),
	EDS_Death3  UMETA(DisplayName = "Death3"),
	EDS_Death4  UMETA(DisplayName = "Death4"),

	EDS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class ELaunchState : uint8
{
	ELS_IsLaunching UMETA(DisplayName = "Start Launch"),
	ELS_IsNotLaunching UMETA(DisplayName = "End Launch"),
};

UCLASS()
class SLASH_API AEnemy : public ACharacterBase
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;
protected:

	virtual void BeginPlay() override;
#pragma region Enums
	UPROPERTY(BlueprintReadOnly)
		ELaunchState LaunchState = ELaunchState::ELS_IsNotLaunching;
	UPROPERTY(BlueprintReadWrite)
		EEnemyState EnemyState = EEnemyState::EES_Patroling;
	UPROPERTY(BlueprintReadWrite)
		EEnemyWeaponState WeaponState = EEnemyWeaponState::EEWS_Unequiped;
	UPROPERTY(BlueprintReadOnly)
		EEnemyLocomotionState StateLocomotion = EEnemyLocomotionState::ELS_Walking;
	UPROPERTY(BlueprintReadOnly)
		TEnumAsByte< EDeathPose >DeathPose;

#pragma endregion Enums

private:
	void InitializeEnemy();
	UPROPERTY(EditAnywhere)
		bool disableDedection;
	UPROPERTY(VisibleAnywhere)
		UPawnSensingComponent* SensingComponent;

	APlayerController* PlayerController;
	UPROPERTY()
		class AAIController* EnemyAIController;

#pragma region TakeDamage
protected:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)override;
	virtual void DirectionalHitReact(const FVector& HitLocation) override;
	virtual void Die() override;
	virtual int32 PlayDeathMontage() override;

public:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
		UEnemyHealthBar* HealthBarWC;

	virtual void GetHit_Implementation(const FVector& HitLocation,AActor* Hitter) override;
	UFUNCTION(BlueprintCallable)
	 void SetIsPlayingHitAnim(bool canPlayHitAnim) {isPlayingHitAnim = canPlayHitAnim; }
private:
	UPROPERTY(EditAnywhere, Category = "Death")
		TSubclassOf<UCameraShakeBase> DeathCamShake;
	UPROPERTY(EditAnywhere, Category = "Death")
		TArray<TSubclassOf<AItem>> ItemsToDrop;
	FVector GetHitLocation;
	bool isPlayingHitAnim = false;

	void OnStunned();
	void HandleDamage(float DamageAmount) override;
#pragma endregion TakeDamage

#pragma region AnimMontages
private:
	UPROPERTY(EditAnywhere, Category = "Montage")
		UAnimMontage* OnQSHitLaunchMontage;
	UPROPERTY(EditAnywhere, Category = "Chase")
		UAnimMontage* WeaponEquipMontage;
#pragma endregion AnimMontages

#pragma region VFX
	UPROPERTY(EditAnywhere, Category = "Visual Effects")
		UParticleSystemComponent* LockOnVFX;

#pragma endregion VFX

#pragma region Navigation&&AI
public:
	UPROPERTY(BlueprintReadWrite,Category = "Combat",VisibleAnywhere)
		AActor* CombatTarget;
private:
	bool isAbleToSeePlayer = true;
	//curent patrol target
	UPROPERTY(EditInstanceOnly,Category = "AI Navigation")
		AActor* CurrentPatrolTarget;
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
		TArray<AActor*> PatrolTargets;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
		float NormalSpeed;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
		float ChaseSpeed;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
		double AcceptableRadius;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
		float PlayerRange;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
		float AttackRadius;
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
		float WaitTime;
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
		float SphereRadius;
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
		float NavigationAcceptableRadius;
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
		float InvestigationTime;
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
		float InvestigateRightDirection;
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
		float InvestigationTargetRange;
	bool PlayerIsSpotted;

	float LastSeenTime;
	float SpotedTime;
	float DedectionLevelAmount = 0.f;
	float DedectionBarPercentage;
	float DedectionLevelRef;
	UPROPERTY(VisibleAnywhere)
		bool isOutOfSight;
	UPROPERTY(EditAnywhere, Category = "AI Navigation | Dedection")
		float slowDeltaTime = 5.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation | Dedection")
		float DedectionLevelIncreaseSpeedAmount;
	UPROPERTY(EditAnywhere, Category = "AI Navigation | Dedection")
		float DedectionLevelDecreaseSpeedAmount;
		
public:
	FORCEINLINE bool IsPlayerSpotted() { return PlayerIsSpotted; }
	void  SetNavigationAcceptableRadius(float newValue) { NavigationAcceptableRadius = newValue; }
#pragma region Combat
public:
	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWrapTarget();
	UFUNCTION(BlueprintCallable)
	FVector GetRotationWrapTarget();

protected:
	virtual bool CanAttack() override;
private:
//	UPROPERTY(EditAnywhere, Category = "Combat")
//		TSubclassOf<class AWeapon> Weapon;
	FTimerHandle AttackTimerHandle;
	UPROPERTY(EditAnywhere, Category = "Combat")
		TArray<FName> AttackMontageSections;
	UPROPERTY(EditAnywhere, Category = "Combat")
		float AttackMin = .5f;
	UPROPERTY(EditAnywhere, Category = "Combat")
		float AttackMax = 1.3f;
	UPROPERTY(EditAnywhere, Category = "Combat")
		double WrapTargetDistance = 75.f;
	UFUNCTION(BlueprintCallable)
		void AttachWeaponToEnemy();

	bool IsInAttackRange();
	void StartToAttack();
	void CheckCombatTarget();
	void SetEnemyStateToAttacking();
	virtual void Attack();
	virtual void PlatAttackMontage();
	void AttackEnd();
	void SetAttackTimer();
	void ClearAttackTimer();
	void StopAttackMontage();

#pragma endregion Combat

#pragma region Patrol
protected:
	AActor* ChoosePatrolTarget();

private:
	FTimerHandle PatrolTimeHandle;
	FTimerHandle InvestigateTimerHandle;

	bool IsInRangeOfTargetPoint();
	void CheckPatrolTarget();
	void Chase();
	void LoseInterest();
	void InvestigateTimer();
	void PatrolTimeFinished();
	void SetPatrolTimer();
	void ClearPatrolTimer();

	void SetEnemyStateToChasing();
#pragma endregion Patrol

	//main functions
	UFUNCTION()
		void PawnSeen(APawn* SeenPawn);
	void MoveTo(AActor* NewDestination);
	bool IsInRangeOf(AActor* Target, float Radius);
	bool PlayerInRange();

	//states
	bool IsInChaseState();
	bool IsInPatrolingState();
	bool IsInAttackingState();
	bool IsInEngagedState();
	bool IsInStunnedState();
	bool IsInEquippedState();
	bool IsInUnequippedState();

#pragma endregion Navigation&&AI

#pragma region Launch

public:
	//void LaunchCharacter(FVector direction, float launchForce);
	UFUNCTION(BlueprintCallable)
		void SetLaunchState(ELaunchState NewState) { LaunchState = NewState; }
private:
	UCharacterMovementComponent* CharacterMovement;

	UPROPERTY(EditInstanceOnly, Category = "Launch")
		FVector LaunchForce;
	UPROPERTY(EditInstanceOnly, Category = "Launch")
		float LaunchForceInterpSpeed;

#pragma endregion Launch

#pragma region Stunned
private:
	bool canStunnedByShield;
	bool isStunnedByShield;
	UPROPERTY(EditAnywhere,Category = "Stunned")
		UAnimMontage* FreezeAnim;
	UPROPERTY(EditAnywhere, Category = "Stunned")
		UAnimMontage* StunShieldAnim;
	UPROPERTY(EditAnywhere, Category = "Stunned")
		UParticleSystem* IceSpikeBreakVFX;
	UPROPERTY(EditAnywhere, Category = "Stunned")
		TSubclassOf<UCameraShakeBase> StunHitCamShake;
	UPROPERTY(EditAnywhere, Category = "Stunned")
		float StunTime;
	UPROPERTY(EditAnywhere, Category = "Stunned")
		float ShieldStunTime;

	bool isStunnedByIce;
	bool hasBeenHit;

	FTimerHandle HasBeenHitTimerHandle;
	FTimerHandle StunnedTimerHandle;
	FTimerHandle ShieldStunnTimerHandle;

	void HasBeenHitTimer();
	void StunnedTimer();
	void ShieldStunnedTimer();
public:
	//stun by shield
	UFUNCTION()
		FORCEINLINE bool GetCanStunnedByShield() { return canStunnedByShield; }
	UFUNCTION()
		FORCEINLINE void SetCanStunnedByShield(bool newBool) { canStunnedByShield = newBool; }
	UFUNCTION(BlueprintCallable ,BlueprintPure)
		FORCEINLINE bool GetIsStuneedByShield() { return isStunnedByShield; }
	UFUNCTION(BlueprintCallable)
		void SetIsStuneedByShield(bool newIsStunnedByShield);
	//stun by shield
	void LaunchCharacter(FVector direction, float launchForceAmount);

	UPROPERTY(BlueprintReadOnly)
		EEnemyLocomotionState EnemyLocomotionState = EEnemyLocomotionState::ELS_Idle;
	void SetIsStunned(bool NewIsStunned) { isStunnedByIce = NewIsStunned; }
	UFUNCTION(BlueprintCallable)
		bool GetIsStunnedByIce() { return isStunnedByIce; }
	bool GetHasBeenHit() { return hasBeenHit; }
#pragma endregion Stunned

public:
	// hit and death
	UFUNCTION()
		FORCEINLINE UParticleSystemComponent* GetLockOnVFX() { return LockOnVFX; }
	bool IsDead();
	UFUNCTION(BlueprintCallable)
		void Stun();

#pragma region ToBeAssassinated
protected:
	UFUNCTION()
		void OnASBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnASEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
private:
	UPROPERTY(VisibleAnywhere)
		UBoxComponent* ToBeAssassinatedCollider;
	UPROPERTY(VisibleAnywhere)
		USceneComponent* ToBeAssassinatedLocation;
	UPROPERTY(EditAnywhere, Category = "Assassination")
		UAnimMontage* BeignAssassinatedAnim;
	bool HasBeenAssassinated;

	void BeignAssassianted(AActor* OtherActor,AEnemy* Enemy,float OpacityPercent);
public:
	USceneComponent* GetAssassinationLocation() { return ToBeAssassinatedLocation; }
	void PlayBeignAssassinatedAnimation();
	UFUNCTION(BlueprintCallable,BlueprintPure)
		bool GetHasBeenAssassinated() { return HasBeenAssassinated; }
#pragma endregion ToBeAssassinated

#pragma region AttackAtOnce
private:
	bool canEngage;
	void NavigatioAcceptableRadiusAndEngage();
public:
	void SetCanEngage(bool newBool) { canEngage = newBool; }
#pragma endregion AttackAtOnce

};
