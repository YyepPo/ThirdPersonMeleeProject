// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HitInterface.h"
#include "CharacterBase.generated.h"

class AItem;
class AWeapon;
class UAttributeComponent;
class BoxComponent;


UCLASS()
class SLASH_API ACharacterBase : public ACharacter,public IHitInterface
{
	GENERATED_BODY()

public:
	ACharacterBase();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void SetUpWeaponCollision(ECollisionEnabled::Type Type);

	UPROPERTY()
		AWeapon* CurrentWeapon;
	void SpawnWeapon(FName SocketName);
	UPROPERTY(EditAnywhere, Category = "Weapon")
		TSubclassOf<AWeapon> StartingWeaponClass;
private:

	UPROPERTY(EditAnywhere, Category = "Combat")
		TArray<FName> DeathMontageSections;

	UPROPERTY(EditAnywhere, Category = "Visual Effects")
		UParticleSystem* BloodVFX;
	UPROPERTY(EditAnywhere, Category = "Sounds")
		USoundBase* HitSound;
protected:

	void PlayHitSound(FVector HitLocation);
	void SpawnHitParticle(FVector HitLocation);


	virtual void BeginPlay() override;
	virtual void Attack();
	//can return int32
	virtual void PlatAttackMontage();
	virtual bool CanAttack();

	void DisableCollision();

	int32 PlayRandomMontageSection(UAnimMontage* Montage, TArray<FName>SectionNames);
	virtual int32 PlayDeathMontage();

	bool IsAlive();

	UFUNCTION(BlueprintCallable)
		virtual void AttackEnd();
	UFUNCTION(BlueprintCallable)
		void PlayCameraShake(APlayerController* PlayerController,TSubclassOf< UCameraShakeBase >Shake);
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
		UAnimMontage* AttackAnimMontage;

	virtual void PlayAMontage(UAnimMontage* Montage);
	virtual void PlayMontageSections(UAnimMontage* Montage, const FName& SectionName);

	virtual void Die();
	virtual void AttributeTakeDamage(float DamageAmount);
	virtual void PlayReactMontage(FName Section);
	virtual void DirectionalHitReact(const FVector& HitLocation);
	virtual void HandleDamage(float DamageAmount);

	UPROPERTY(VisibleAnywhere)
		UAttributeComponent* AttributeComponent;
	UPROPERTY(EditAnywhere, Category = "Montage")
		UAnimMontage* HitReactAnimMontage;
	UPROPERTY(EditAnywhere, Category = "Montage")
		UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Death")
		USoundBase* DeathSound;
	UPROPERTY(EditAnywhere, Category = "Death")
		UParticleSystem* DeathBloodVFX;
		
};
