// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarUW.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;

UCLASS()
class SLASH_API UHealthBarUW : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;


	UPROPERTY(meta = (BindWidget),BlueprintReadWrite)
		UProgressBar* HealthBar;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
		UTextBlock* DamageText;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
		UProgressBar* DedectionLevelBar;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
		UImage* DedectionLevelImage;
	void StoreWidgetAnimation();

	UWidgetAnimation* GetAnimationByName(FName AnimationName)const;

	void PlayWidgetAnimation();

private:
	TMap<FName, UWidgetAnimation*> AnimationMap;
	UWidgetAnimation* ColorChangeAnimation;

	

};
