// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/IceBeam.h"
#include "Components/SphereComponent.h"
#include "Enemy/Enemy.h"
#include "Kismet/Gameplaystatics.h"
#include "GameFramework/ProjectileMovementComponent.h"

AIceBeam::AIceBeam()
{
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(mesh);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->MaxSpeed = ProjectileSpeed;

	Col = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	Col->SetupAttachment(GetRootComponent());

	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
}

void AIceBeam::BeginPlay()
{
	Super::BeginPlay();
	
	Col->OnComponentBeginOverlap.AddDynamic(this, &AIceBeam::OnBeginOverlap);

	if (BeamVFX && BeamSFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamVFX, GetActorLocation(),GetActorRotation());
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), BeamSFX, GetActorLocation());
	}
}

void AIceBeam::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor && (OtherActor == this || OtherActor == GetOwner())) { return; }
	UE_LOG(LogTemp, Warning, TEXT("On begin overlap has ben excecuted"));
	AEnemy* Enemy = Cast<AEnemy>(OtherActor);
	if (!Enemy) { return; }

	if (HitParticle && HitSFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, Enemy->GetActorLocation(), GetActorRotation());
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSFX, GetActorLocation());
	}

	UGameplayStatics::ApplyDamage(Enemy,
		DamageAmount,
		PlayerController,
		this,
		UDamageType::StaticClass());

	Destroy();
}

void AIceBeam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

