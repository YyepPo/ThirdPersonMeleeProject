// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "EnemyHealthBar.generated.h"

class UHealthBarUW;

UCLASS()
class SLASH_API UEnemyHealthBar : public UWidgetComponent
{
	GENERATED_BODY()
public:
		void SetHealthBarPercentage(float Percentage);
		void SetDamageText(float Damage);

		void SetDedectionLevelPercentage(float percentage);
		float GetDedectionBarPercentage();
		void DisableDedectionBar();
		UHealthBarUW* GetHealthBarWidget();

		UHealthBarUW* HealthBarWidget;
protected:
};
