// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashOverlay.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USlashOverlay::SetHealthBarPercentage(float Percent)
{
	if (HealthProgressBar)
		HealthProgressBar->SetPercent(Percent);
}

void USlashOverlay::SetStaminBarPercentage(float Percent)
{
	if (StaminaProgressBar)
		StaminaProgressBar->SetPercent(Percent);
}

void USlashOverlay::SetCointCountText(int32 Coin)
{
	if (CoinCountText)
		CoinCountText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Coin)));
}

void USlashOverlay::SetSoulCountText(int32 Soul)
{
	if (SoulCountText)
		SoulCountText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Soul)));
}

void USlashOverlay::SetAssassinationTextOpacity(float Percent)
{
	if (AssassinationText) AssassinationText->SetOpacity(Percent);
}
