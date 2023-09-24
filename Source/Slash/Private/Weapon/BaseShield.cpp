#include "Weapon/BaseShield.h"

#include "Components/BoxComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "Kismet/GameplayStatics.h"

#include "Enemy/Enemy.h"

ABaseShield::ABaseShield()
{
	ShieldCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Shield Collider"));
	ShieldCollider->SetupAttachment(GetRootComponent());
}

void ABaseShield::BeginPlay()
{
	Super::BeginPlay();
	ShieldCollider->OnComponentBeginOverlap.AddDynamic(this, &ABaseShield::OnShieldOverlap);
	ShieldCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseShield::Equip(USceneComponent* Parent, FName SocketName, APawn* NewInsitagor, APawn* NewOwner)
{
	Super::Equip(Parent, SocketName, NewInsitagor, NewOwner);
	UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
}

void ABaseShield::OnShieldOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr || (OtherActor == this && OtherActor == GetOwner())) { return; }
	AEnemy* Enemy = Cast<AEnemy>(OtherActor);
	if (Enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("Shield has collided with %s"), *Enemy->GetActorNameOrLabel());
	}
}

void ABaseShield::SetSheildCollision(ECollisionEnabled::Type newCollision)
{
	if (!ShieldCollider) { return; }
	ShieldCollider->SetCollisionEnabled(newCollision);
}

void ABaseShield::SpawnShieldSparkEffect(FVector HitLocation)
{
	if (!GetOwner()) { return; }
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ShieldSpark, HitLocation,GetOwner()->GetActorForwardVector().Rotation());
	UGameplayStatics::PlaySoundAtLocation(this,HitSound,GetActorLocation());
}



