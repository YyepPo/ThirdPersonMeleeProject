// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"

#include "Kismet/GameplayStatics.h"

#include "HitInterface.h"


AWeapon::AWeapon()
{
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	BoxCollision->SetupAttachment(RootComponent);
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	StartPos = CreateDefaultSubobject<USceneComponent>(TEXT("Start Pos"));
	StartPos->SetupAttachment(BoxCollision);
	EndPos = CreateDefaultSubobject<USceneComponent>(TEXT("End Pos"));
	EndPos->SetupAttachment(BoxCollision);
}


void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}


void AWeapon::Equip(USceneComponent* Parent,FName SocketName, APawn* NewInsitagor, APawn* NewOwner)
{
	Super::Equip(Parent, SocketName, NewInsitagor, NewOwner);
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"))) { return; }

	FHitResult BoxHit;
	BoxTrace(BoxHit);

	if(BoxHit.GetActor())
	{
		if (GetOwner()->ActorHasTag(TEXT("Enemy")) && BoxHit.GetActor()->ActorHasTag(TEXT("Enemy"))) { return; }
		if (GetOwner()->ActorHasTag(TEXT("EngageableTarget")) && BoxHit.GetActor()->ActorHasTag(TEXT("EngageableTarget"))) { return; }

		UE_LOG(LogTemp, Warning, TEXT("%s"), *BoxHit.GetActor()->GetActorNameOrLabel());

		OnEnemyHit.Broadcast();

		UGameplayStatics::ApplyDamage(
			BoxHit.GetActor(),
			DamageAmount,
			GetInstigator()->GetController(),
			this,
			UDamageType::StaticClass()
		);

		IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
		if (HitInterface)
		{
			HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint,GetOwner());

			APlayerController* Player = UGameplayStatics::GetPlayerController(this, 0);
			Player->ClientStartCameraShake(OnEnemyHitCameraShake);
			SlowMotion();
		}

		CreateFields(BoxHit.ImpactPoint);
	}

}

void AWeapon::BoxTrace(FHitResult& BoxHit)
{
	const FVector Start = StartPos->GetComponentLocation();
	const FVector End = EndPos->GetComponentLocation();

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);

	for (AActor* IgnoredOne : IgnoreActors)
	{
		IgnoredActors.AddUnique(IgnoredOne);
	}

	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		BoxTraceExtent,
		StartPos->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		IgnoredActors,
		bShowBoxDebug ?	EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		BoxHit,
		true);
	
	IgnoredActors.AddUnique(BoxHit.GetActor());


}
