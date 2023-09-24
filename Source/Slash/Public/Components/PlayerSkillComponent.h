// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerSkillComponent.generated.h"

class ASlashCharacter;
class UCapsuleComponent;
class UNiagaraSystem;
class AEnemy;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UPlayerSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerSkillComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:

	ASlashCharacter* SlashCharacter;
	UAnimInstance* AnimInstance;
	APlayerController* PlayerController;
#pragma region SkillOne
private:
	UPROPERTY(EditAnywhere, Category = "Skill One")
		UAnimMontage* SkillOneAnim;
	UPROPERTY(EditAnywhere, Category = "Skill One")
		FVector LaunchForce;
	FVector LunchForceDefaultValue;
	UPROPERTY(EditAnywhere, Category = "Skill One")
		float InterpSpeed;
	UPROPERTY(EditAnywhere, Category = "Skill One")
		float SkillOneDamageAmount;
	UPROPERTY(EditAnywhere, Category = "Skill One")
		UParticleSystem* WhrilWind;
	UPROPERTY(EditAnywhere, Category = "Skill One")
		UParticleSystem* SegmentCrumble;
	UPROPERTY(EditAnywhere, Category = "Skill One")
		UNiagaraSystem* IceSpikeVFX;
	UPROPERTY(EditAnywhere, Category = "Skill One")
		float QSkillCoolDownTime;
	UPROPERTY(EditAnywhere, Category = "Skill One")
		float Launchfloat;
public:
	TArray< AEnemy*> Enemies;
	UFUNCTION(BlueprintCallable)
		void SkillOneBehaviour();
	UFUNCTION(BlueprintCallable)
		float GetQSkillCooldown() { return QSkillCoolDownTime; }
#pragma endregion SkillOne

#pragma region SkillTwo
private:
	UPROPERTY(EditAnywhere, Category = "Skill Two")
		UAnimMontage* SkillTwoAnim;
	UPROPERTY(EditAnywhere, Category = "Skill Two")
		UParticleSystem* LongIceSpikeVFX;
	UPROPERTY(EditAnywhere, Category = "Skill Two")
		UParticleSystem* StunSpikeVFX;
	UPROPERTY(EditAnywhere, Category = "Skill Two")
		FVector LongIceSpawnLoc;
	UPROPERTY(EditAnywhere, Category = "Skill Two")
		float CSkillDamageAmount;
	UPROPERTY(EditAnywhere, Category = "Skill Two")
		FName SocketName;
	UPROPERTY(EditAnywhere, Category = "Skill Two")
		float StunTime;
	UPROPERTY(EditAnywhere, Category = "Skill Two")
		USoundWave* IceBreakSFX;
	bool a;
	AEnemy* SkillTwoEnemy;
	UParticleSystemComponent* AttachedVFX;
public:
	UFUNCTION(BlueprintCallable)
		void SkillTwoBehaviour();
	UFUNCTION()
		void SetSkillsTwoNewEnemy(AEnemy* NewEnemy) { SkillTwoEnemy = NewEnemy; }
#pragma endregion SkillTwo

#pragma region SkillThree
private:
	UPROPERTY(EditAnywhere, Category = "Skill Three")
		float globalTimeDilationValue;
	UPROPERTY(EditAnywhere, Category = "Skill Three")
		float customTimeDilationValue;
	UPROPERTY(EditAnywhere, Category = "Skill Three")
		float LineLength;
	UPROPERTY(EditAnywhere, Category = "Skill Three")
		float SlowMotionLength;
	TArray<AEnemy*> SkillThreeEnemies;
	UPROPERTY(EditAnywhere, Category = "Skill Three")
		FKey LeftMouseButton;
	UPROPERTY(EditAnywhere, Category = "Skill Three")
		TArray<TSubclassOf<AActor>> ProjectileBeam;
	FTimerHandle SlowMotionTimerHandle;
	FVector Distancee;
	void SlowMotionTimer();
	bool SkillThreeHasBegan;
public:
	void SkillThreeBehaviour();
	UPROPERTY(EditAnywhere,BlueprintReadWrite ,Category = "Skill Three")
		int32 MaxShotPoint;
		
#pragma endregion SkillThree
public:
	void SkillOne();
	void SkillTwo();
	void SkillThree();
};
