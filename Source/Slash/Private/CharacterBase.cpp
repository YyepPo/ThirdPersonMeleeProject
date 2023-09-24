// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterBase.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"

#include "Weapon/Weapon.h"
#include "Components/AttributeComponent.h"

#include "Kismet/Gameplaystatics.h"

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attribute Component"));

}

void ACharacterBase::PlayHitSound(FVector HitLocation)
{
	if (HitSound) UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, HitLocation);
}

void ACharacterBase::SpawnHitParticle(FVector HitLocation)
{
	if (BloodVFX)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodVFX, HitLocation);
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACharacterBase::SetUpWeaponCollision(ECollisionEnabled::Type Type)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->GetBoxCollision()->SetCollisionEnabled(Type);
		CurrentWeapon->IgnoreActors.Empty();
	}
}

void ACharacterBase::SpawnWeapon(FName SocketName)
{
	if (StartingWeaponClass)
	{
		AWeapon* StartingWeapon = GetWorld()->SpawnActor<AWeapon>(StartingWeaponClass);
		StartingWeapon->Equip(GetMesh(), SocketName, this, this);
		CurrentWeapon = StartingWeapon;
	}
}

void ACharacterBase::Attack()
{
}

void ACharacterBase::PlatAttackMontage()
{
}

bool ACharacterBase::CanAttack()
{
	return false;
}

void ACharacterBase::DisableCollision()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

int32 ACharacterBase::PlayRandomMontageSection(UAnimMontage* Montage, TArray<FName> SectionNames)
{
	if (SectionNames.Num() <= 0 && !Montage) { return 0 ; }
	int32 randomNum = FMath::RandRange(0, SectionNames.Num() - 1);
	PlayMontageSections(Montage, SectionNames[randomNum]);
	return randomNum;
}

int32 ACharacterBase::PlayDeathMontage()
{
	return PlayRandomMontageSection(DeathMontage,DeathMontageSections);
}

bool ACharacterBase::IsAlive()
{
	return AttributeComponent && AttributeComponent->IsAlive();
}

void ACharacterBase::AttackEnd()
{
}

void ACharacterBase::PlayCameraShake(APlayerController* PlayerController, TSubclassOf< UCameraShakeBase >Shake)
{
	if (!PlayerController) { return; }
	PlayerController->ClientStartCameraShake(Shake);
}

void ACharacterBase::PlayAMontage(UAnimMontage* Montage)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
	}
}

void ACharacterBase::PlayMontageSections(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

void ACharacterBase::Die()
{
	if (DeathSound)
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());

	if (DeathBloodVFX)
		UGameplayStatics::SpawnEmitterAttached(DeathBloodVFX, GetMesh(), "Spine1");
}

void ACharacterBase::AttributeTakeDamage(float DamageAmount)
{
	if (AttributeComponent)
	{
		AttributeComponent->OnTakeDamage(DamageAmount);
	}
}

void ACharacterBase::PlayReactMontage(FName Section)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactAnimMontage)
	{
		AnimInstance->Montage_Play(HitReactAnimMontage);
		AnimInstance->Montage_JumpToSection(Section);
	}
}

void ACharacterBase::DirectionalHitReact(const FVector& HitLocation)
{
	const FVector ForwardVector = GetActorForwardVector();
	//set z axis paralel to ground
	const FVector LowerPoint(HitLocation.X, HitLocation.Y, GetActorLocation().Z);
	// a vector from hit point to actor location
	const FVector HitPoint = (LowerPoint - GetActorLocation()).GetSafeNormal();

	//dot produc allways returns a pozitive number
	//get dot product 
	//A Vector * B Vector = |A|*|B| cos(theta)
	//|A|= 1 |B| = 1 both of the vectors are normalized( = 1 )
	const double CosTheta = FVector::DotProduct(ForwardVector, HitPoint);

	//get acos of costheta
	double Theta = FMath::Acos(CosTheta);

	//convert theta to degres
	Theta = FMath::RadiansToDegrees(Theta);

	//crossproduct helps us to know if a particular angle is pozitive or negativ
	//if crossproduct point down ,theta should be negative
	FVector CrossProduct = FVector::CrossProduct(ForwardVector, HitPoint);
	(CrossProduct.Z < 0) ? Theta *= -1 : Theta *= 1;

	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 200.f, 5.f, FColor::Blue, 5.f);

	FName Section("ReactFront");

	if (Theta >= -45.f && Theta < 45.f)
		Section = FName("ReactBack");
	else if (Theta >= -135.f && Theta < -45.f)
		Section = FName("ReactLeft");
	else if (Theta >= 45.f && Theta < 135.f)
		Section = FName("ReactRight");

	PlayReactMontage(Section);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Blue, FString::Printf(TEXT("Theta is : %f"), Theta));
	}

}

void ACharacterBase::HandleDamage(float DamageAmount)
{
	if (AttributeComponent)
		AttributeComponent->OnTakeDamage(DamageAmount);

}
