// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IceBeam.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
UCLASS()
class SLASH_API AIceBeam : public AActor
{
	GENERATED_BODY()
	
public:	
	AIceBeam();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(VisibleAnywhere)
		USphereComponent* Col;
	UPROPERTY(VisibleAnywhere)
		UProjectileMovementComponent* ProjectileMovement;
	UPROPERTY(EditAnywhere)
		float DamageAmount;
	UPROPERTY(EditAnywhere)
		float ProjectileSpeed;
	UPROPERTY(EditAnywhere)
		UParticleSystem* HitParticle;
	UPROPERTY(EditAnywhere)
		UParticleSystem* BeamVFX;
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* mesh;
	UPROPERTY(EditAnywhere)
		USoundBase* BeamSFX;
	UPROPERTY(EditAnywhere)
		USoundBase* HitSFX;
	APlayerController* PlayerController;
};
