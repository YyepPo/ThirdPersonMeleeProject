// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HangObject.generated.h"

class UBoxComponent;

UCLASS()
class SLASH_API AHangObject : public AActor
{
	GENERATED_BODY()
	
public:	
	AHangObject();
	UFUNCTION()
		FTransform GetToHangPosition();
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere)
		UBoxComponent* BoxCollider;
	UPROPERTY(VisibleAnywhere)
		USceneComponent* ToHangObject;
};
