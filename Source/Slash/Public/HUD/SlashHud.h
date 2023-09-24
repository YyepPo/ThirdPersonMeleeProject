// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SlashHud.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API ASlashHud : public AHUD
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditDefaultsOnly, Category = "Slash")
		TSubclassOf<class USlashOverlay> SlashOverlayClass;

	UPROPERTY()
		USlashOverlay* SlashOverlay;
public:
	FORCEINLINE USlashOverlay* GetSlashOverlay()const { return SlashOverlay; }
};
