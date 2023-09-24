// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Bird.generated.h"

class UCapsuleComponent;
class UCameraComponent;
class USpringArmComponent;
class UFloatingPawnMovement;
UCLASS()
class SLASH_API ABird : public APawn
{
	GENERATED_BODY()

public:
	ABird();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
		UCapsuleComponent* CapsuleCollider;
	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* Mesh;
	UPROPERTY(EditAnywhere)
		USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere)
		UCameraComponent* Camera;
	UPROPERTY(EditAnywhere)
		UFloatingPawnMovement* PawnMovement;

#pragma region Input Bindings

	//movement
	void Forward(float value);
	void Right(float value);
	void LookUp(float value);
	void LookRight(float value);

	void MoveUp(float value);

	//zoom in and out
	void RightClickPressed();
	void RightClickReleased();

	//check enemy to highlight
	void HighlightEnemy();

#pragma endregion

#pragma region Zoom in and out behaviour

	void ZoomInOutBehaviour(float DeltaSeconds);
	bool IsHoldingRightClick;

	UPROPERTY(EditAnywhere,Category = "Zoom In & Out")
		float ZoomInAmount;
	UPROPERTY(EditAnywhere, Category = "Zoom In & Out")
		float ZoomOutAmount;
	UPROPERTY(EditAnywhere, Category = "Zoom In & Out")
		float InterpSpeed;
	UPROPERTY(EditAnywhere, Category = "Zoom In & Out")
		float ZoomInMovementSpeed;
	float DefaultMovementSpeed;
#pragma endregion

#pragma region Enemy spoting

	void IsEnemyInVision(bool &RefHasHit, UStaticMeshComponent* &RefEnemiesStaticMesh);

	UPROPERTY(EditAnywhere)
		float RayCastLength;

public:
	UPROPERTY(BlueprintReadWrite)
		bool HasSeenEnemy;
	UPROPERTY(BlueprintReadWrite)
		UStaticMeshComponent* EnemiesStaticMesh;
#pragma endregion
};
