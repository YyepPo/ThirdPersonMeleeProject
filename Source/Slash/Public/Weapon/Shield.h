// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Shield.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API AShield : public AItem
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
public:
	AShield();
};
