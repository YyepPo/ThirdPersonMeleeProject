
#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

class UBoxComponent;
class UCameraShakeBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnemyHasBeenHitSignature);//FPickedUpItemSignature);

UCLASS()
class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
	virtual	void Equip(USceneComponent* Parent, FName SocketName,APawn* NewInsitagor,APawn* NewOwner) override;
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
		void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
private:
	UPROPERTY(EditAnywhere)
		UBoxComponent* BoxCollision;
	UPROPERTY(EditAnywhere)
		USceneComponent* StartPos;
	UPROPERTY(EditAnywhere)
		USceneComponent* EndPos;

	UPROPERTY(EditAnywhere)
		float DamageAmount;

	UPROPERTY(EditAnywhere)
		bool IsOneHandedWeapon = true;

	UPROPERTY(EditAnywhere)
		float LaunchForce;
	UPROPERTY(EditAnywhere)
		float LaunchForceInterpSpeed;
	/**
	Camera Shakes
	*/
	UPROPERTY(EditAnywhere, Category = "Camera Shake")
		TSubclassOf<UCameraShakeBase> OnEnemyHitCameraShake;

	//Trail
	UPROPERTY(EditAnywhere, Category = Trail)
		TObjectPtr<UParticleSystem> WeaponTrail;

	void BoxTrace(FHitResult& BoxHit );
	UPROPERTY(EditAnywhere)
		FVector BoxTraceExtent = FVector(5.f);
	UPROPERTY(EditAnywhere)
		bool bShowBoxDebug = false;
public:
	FORCEINLINE	UBoxComponent* GetBoxCollision() { return BoxCollision; }
	FORCEINLINE UParticleSystem* GetWeaponTrail() { return WeaponTrail; }

	TArray<AActor*> IgnoreActors;

	UFUNCTION(BlueprintImplementableEvent)
		void CreateFields(const FVector& FieldLocation);
	UFUNCTION(BlueprintImplementableEvent)
		void SlowMotion();
	UFUNCTION()
		FORCEINLINE bool GetIsOneHandedWeapon() { return IsOneHandedWeapon; }

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers ")
		FEnemyHasBeenHitSignature OnEnemyHit;
};
