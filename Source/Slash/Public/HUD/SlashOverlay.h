// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashOverlay.generated.h"

class UProgressBar;
class UTextBlock;
UCLASS()
class SLASH_API USlashOverlay : public UUserWidget
{
	GENERATED_BODY()
private:
	UPROPERTY(meta = (BindWidget))
		UProgressBar* HealthProgressBar;
	UPROPERTY(meta = (BindWidget))
		UProgressBar* StaminaProgressBar;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* CoinCountText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* SoulCountText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* AssassinationText;
public:
	void SetHealthBarPercentage(float Percent);
	void SetStaminBarPercentage(float Percent);
	void SetCointCountText(int32 Coin);
	void SetSoulCountText(int32 Soul);
	void SetAssassinationTextOpacity(float Percent);
};
