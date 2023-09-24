// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditAnywhere, Category = "Attributes")
		float MaxHealth = 100;
	UPROPERTY(EditAnywhere, Category = "Attributes")
		float Health;
	UPROPERTY(EditAnywhere, Category = "Attributes")
		float MaxStamina = 200;
	UPROPERTY(EditAnywhere, Category = "Attributes")
		float Stamina;

	UPROPERTY(VisibleAnywhere, Category = "Coin")
		int32 CurrentCoinAmount;
	UPROPERTY(VisibleAnywhere, Category = "Soul")
		int32 CurrentSoulAmount;
public:
	//health
	UFUNCTION()
		void OnTakeDamage(float Damage);
	UFUNCTION()
		float GetHealthPercentage();
		bool IsAlive();
	UFUNCTION()
			void HealthBoost(float boostAmount);
	//Stamina
	UFUNCTION()
		float GetStaminPercentage();
	UFUNCTION()
		float GetStaminaAmount() { return Stamina; }
	UFUNCTION()
		void UseStamina(float UseAmount);
	UFUNCTION()
		void IncreaseStaminaOverTime(float IncreaseAmount,float DeltaTime,float InterpSpeed);
	UFUNCTION()
		void StaminaBoost(float boostAmount);
	//picking up item
	void AddSoul(int32 SoulAmount);
	FORCENOINLINE int32 GetCurrentSoulAmount() { return CurrentSoulAmount; }

	void AddCoin(int32 CoinAmount);
	FORCENOINLINE int32 GetCurrentCoinAmount() { return CurrentCoinAmount; }

};
