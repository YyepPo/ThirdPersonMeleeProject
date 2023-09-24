// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EnemyHealthBar.h"
#include "HUD/HealthBarUW.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Blueprint/UserWidget.h"


void UEnemyHealthBar::SetHealthBarPercentage(float Percentage)
{
	GetHealthBarWidget();

	if (HealthBarWidget && HealthBarWidget->HealthBar)
	{
		HealthBarWidget->HealthBar->SetPercent(Percentage);
	}
}

void UEnemyHealthBar::SetDamageText(float Damage)
{
	GetHealthBarWidget();

	if (HealthBarWidget && HealthBarWidget->DamageText)
	{
		FString a = FString::SanitizeFloat(Damage);
		FText damage = FText::FromString(a);
		HealthBarWidget->DamageText->SetText(damage);
		HealthBarWidget->PlayWidgetAnimation();
	}
}

void UEnemyHealthBar::SetDedectionLevelPercentage(float percentage)
{
	GetHealthBarWidget();

	if (HealthBarWidget && HealthBarWidget->DedectionLevelBar)
	{
		HealthBarWidget->DedectionLevelBar->SetPercent(percentage);
	}
}

float UEnemyHealthBar::GetDedectionBarPercentage()
{
	GetHealthBarWidget();

	if (HealthBarWidget && HealthBarWidget->DedectionLevelBar)
	{
		return HealthBarWidget->DedectionLevelBar->Percent;
	}
	return 0.f;
}

void UEnemyHealthBar::DisableDedectionBar()
{
	GetHealthBarWidget();

	if (HealthBarWidget && HealthBarWidget->DedectionLevelBar && HealthBarWidget->DedectionLevelImage)
	{
		HealthBarWidget->DedectionLevelBar->SetVisibility(ESlateVisibility::Hidden);
		HealthBarWidget->DedectionLevelImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

UHealthBarUW* UEnemyHealthBar::GetHealthBarWidget()
{
	if (HealthBarWidget == nullptr)
	{
		HealthBarWidget = Cast<UHealthBarUW>(GetUserWidgetObject());
		return HealthBarWidget;
	}
	return nullptr;
}

