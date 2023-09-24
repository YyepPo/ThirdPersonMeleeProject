// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	Stamina = MaxStamina;
}

bool UAttributeComponent::IsAlive()
{
	return Health > 0.f;
}

void UAttributeComponent::HealthBoost(float boostAmount)
{
	Health = FMath::Clamp(Health + boostAmount, 0.f, MaxHealth);
}

void UAttributeComponent::OnTakeDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
}

float UAttributeComponent::GetHealthPercentage()
{
	return Health/MaxHealth;
}

float UAttributeComponent::GetStaminPercentage()
{
	return Stamina / MaxStamina;
}

void UAttributeComponent::UseStamina(float UseAmount)
{
	Stamina = FMath::Clamp(Stamina - UseAmount, 0.f, MaxStamina);
}

void UAttributeComponent::IncreaseStaminaOverTime(float IncreaseAmount, float DeltaTime, float InterpSpeed)
{
	Stamina = FMath::FInterpTo(Stamina, 200.f, DeltaTime, InterpSpeed);
}

void UAttributeComponent::StaminaBoost(float boostAmount)
{
	Stamina = FMath::Clamp(Stamina + boostAmount, 0.f, MaxStamina);
}

void UAttributeComponent::AddCoin(int32 CoinAmount)
{
	CurrentCoinAmount += CoinAmount;
}

void UAttributeComponent::AddSoul(int32 SoulAmount)
{
	CurrentSoulAmount += SoulAmount;
}
