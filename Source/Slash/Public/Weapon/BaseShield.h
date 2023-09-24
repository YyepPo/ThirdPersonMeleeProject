// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "BaseShield.generated.h"

class UBoxComponent;
class UNiagaraSystem;

UCLASS()
class SLASH_API ABaseShield : public AItem
{
	GENERATED_BODY()
public:
	ABaseShield();
	virtual	void Equip(USceneComponent* Parent, FName SocketName, APawn* NewInsitagor, APawn* NewOwner) override;
	UFUNCTION()
		void SetSheildCollision(ECollisionEnabled::Type newCollision);
	void SpawnShieldSparkEffect(FVector HitLocation);
private:
	UPROPERTY(VisibleAnywhere)
		UBoxComponent* ShieldCollider;
	UPROPERTY(EditDefaultsOnly)
		UNiagaraSystem* ShieldSpark;
	UPROPERTY(EditDefaultsOnly)
		USoundBase* HitSound;
	UPROPERTY(EditDefaultsOnly)
		USoundBase* EquipSound;
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
		void OnShieldOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
