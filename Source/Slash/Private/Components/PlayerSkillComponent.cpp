// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PlayerSkillComponent.h"

#include "SlashCharacter.h"
#include "Enemy/Enemy.h"

#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "HitInterface.h"


UPlayerSkillComponent::UPlayerSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
}

void UPlayerSkillComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SlashCharacter = Cast<ASlashCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if(SlashCharacter)
		AnimInstance =  SlashCharacter->GetMesh()->GetAnimInstance();

	LunchForceDefaultValue = LaunchForce;
}

void UPlayerSkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (SkillTwoEnemy && SkillTwoEnemy->GetHasBeenHit() && AttachedVFX)
	{
		//AttachedVFX->DestroyComponent();
		if (a == false)
		{
			UGameplayStatics::PlaySoundAtLocation(this, IceBreakSFX, SkillTwoEnemy->GetActorLocation());
			a = true;
		}
	}
}

#pragma region SkillOne

void UPlayerSkillComponent::SkillOne()
{
	AnimInstance->Montage_Play(SkillOneAnim);	
}

void UPlayerSkillComponent::SkillOneBehaviour()
{
	LaunchForce = LunchForceDefaultValue;

	FVector OwnerLoc;
	if (GetOwner())
		OwnerLoc = GetOwner()->GetActorLocation();

	FVector IceSpikeLoc(OwnerLoc.X, OwnerLoc.Y, OwnerLoc.Z / 2);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WhrilWind, OwnerLoc);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SegmentCrumble, OwnerLoc);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, IceSpikeVFX, IceSpikeLoc);

	if (Enemies.Num() > 0)
	{
		for (int32 i = 0; i < Enemies.Num(); i++)
		{
			Enemies[i]->GetCharacterMovement()->MaxWalkSpeed = 0.f;

			//UE_LOG(LogTemp, Warning, TEXT("%s"), *Enemies[i]->GetActorNameOrLabel());
			FVector PlayerLoc = SlashCharacter->GetActorLocation();
			FVector EnemiesLoc = Enemies[i]->GetActorLocation();
			FVector Distance = EnemiesLoc - PlayerLoc;

			DrawDebugLine(GetWorld(), PlayerLoc, PlayerLoc+Distance, FColor::Red, true);

			FVector LaunchDirection = Distance;
			LaunchDirection.Z = 1.f;
			Enemies[i]->LaunchCharacter(LaunchDirection, Launchfloat);
			LaunchForce = FMath::VInterpTo(LaunchForce, FVector::Zero(), UGameplayStatics::GetWorldDeltaSeconds(this), InterpSpeed);

			UGameplayStatics::ApplyDamage(
				Enemies[i],
				SkillOneDamageAmount,
				GetOwner()->GetInstigatorController(),
				GetOwner(),
				UDamageType::StaticClass()
			);
		}
	}
}
#pragma endregion SkillOne

#pragma region SkillTwo
void UPlayerSkillComponent::SkillTwo()
{
	if (!SlashCharacter && !SkillTwoAnim) { return; }
		
	SlashCharacter->SetUpperBody(true);
	AnimInstance->Montage_Play(SkillTwoAnim);
}

void UPlayerSkillComponent::SkillTwoBehaviour()
{
	if (!SlashCharacter) { return; }

	if (LongIceSpikeVFX)
	{
		UParticleSystemComponent* A	=UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), LongIceSpikeVFX,SlashCharacter->GetCSkllVFXLoc()->GetComponentLocation());
		A->SetRelativeRotation(FQuat(SlashCharacter->GetCSkllVFXLoc()->GetComponentRotation()));
	}

	if (!SkillTwoEnemy) { return; }

	SkillTwoEnemy->Stun();

	if (StunSpikeVFX)
		AttachedVFX = UGameplayStatics::SpawnEmitterAttached(StunSpikeVFX, SkillTwoEnemy->GetMesh(),SocketName);

	UGameplayStatics::ApplyDamage(
		SkillTwoEnemy,
		CSkillDamageAmount,
		GetOwner()->GetInstigatorController(),
		GetOwner(),
		UDamageType::StaticClass()
	);		
}

#pragma endregion SkillTwo

#pragma region SkillThree

void UPlayerSkillComponent::SkillThree()
{
	if (!SlashCharacter) { return; }

	SkillThreeHasBegan = true;
	GetOwner()->GetWorldTimerManager().SetTimer(SlowMotionTimerHandle, this, &UPlayerSkillComponent::SlowMotionTimer, SlowMotionLength);

	if (MaxShotPoint <= 0) { return; }
	UGameplayStatics::SetGlobalTimeDilation(this, globalTimeDilationValue);
}

void UPlayerSkillComponent::SkillThreeBehaviour()
{
	if (!SlashCharacter) { return; }
	if (!SkillThreeHasBegan) { return; }

	if(MaxShotPoint <= 0){return;}

	UGameplayStatics::SetGlobalTimeDilation(this, globalTimeDilationValue);

	if (PlayerController && PlayerController->IsInputKeyDown(LeftMouseButton))
	{
		MaxShotPoint--;

		APawn* Owner = Cast<APawn>(GetOwner());
		if (!Owner) { return; }
		AController* Controller = Owner->GetController();
		if (!Controller) { return; }

		FVector Location;
		FRotator Rotation;
		Controller->GetPlayerViewPoint(Location, Rotation);
		FVector StartPoint = Location;
		FVector Direction = Rotation.Vector();
		FVector EndPoint = Location + Direction * LineLength;
		//DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Red, true);

		FCollisionQueryParams param;

		FHitResult HitResult;
		bool hasHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			StartPoint,
			EndPoint,
			ECC_GameTraceChannel2,
			param);

		if (hasHit)
		{
			AEnemy* tracedEnemy = Cast<AEnemy>(HitResult.GetActor());
			if (tracedEnemy)
			{
				SkillThreeEnemies.AddUnique(tracedEnemy);
				if (SkillThreeEnemies.Num() > 0)
				{
					for (int32 i = 0; i < SkillThreeEnemies.Num(); i++)
					{
						UE_LOG(LogTemp, Warning, TEXT("%s"), *SkillThreeEnemies[i]->GetActorNameOrLabel());
						FVector	PlayerPos = GetOwner()->GetActorLocation();
						FVector EnemiesPos = SkillThreeEnemies[i]->GetActorLocation();
						Distancee = (EnemiesPos - PlayerPos) * LineLength;
					//	DrawDebugLine(GetWorld(), PlayerPos, Distancee, FColor::Yellow, true);					
					}
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AAAA"));
	}
}

void UPlayerSkillComponent::SlowMotionTimer()
{
	if (SkillThreeEnemies.Num() > 0)
	{
		for (int32 i = 0; i < SkillThreeEnemies.Num(); i++)
		{
			FVector EnemiesPos = SkillThreeEnemies[i]->GetActorLocation();
			FVector NewDis = (EnemiesPos - GetOwner()->GetActorLocation()) * LineLength;
			GetWorld()->SpawnActor<AActor>(ProjectileBeam[i],
				GetOwner()->GetActorLocation(), NewDis.Rotation());

			UGameplayStatics::ApplyDamage(SkillThreeEnemies[i], 100.f, GetOwner()->GetInstigatorController(), GetOwner(), UDamageType::StaticClass());
		}
	}

	UGameplayStatics::SetGlobalTimeDilation(this, 1.f);
	SkillThreeEnemies.Empty();
	MaxShotPoint = 3;
	SkillThreeHasBegan = false;	
}


#pragma endregion SkillThree