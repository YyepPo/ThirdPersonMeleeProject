// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState_Trail.h"
#include "ANST_WeaponTrail.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UANST_WeaponTrail : public UAnimNotifyState_Trail
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration) override;


};
