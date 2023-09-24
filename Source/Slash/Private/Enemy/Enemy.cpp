#include "Enemy/Enemy.h"
#include "AIController.h"
#include "SlashCharacter.h"
#include "Perception/PawnSensingComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "DrawDebugHelpers.h"

#include "Kismet/GameplayStatics.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/AttributeComponent.h"

#include "DrawDebugHelpers.h"

#include "Particles/ParticleSystemComponent.h"
#include "HUD/EnemyHealthBar.h"

#include "Weapon/Weapon.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	LockOnVFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Lock On VFX"));
	LockOnVFX->SetupAttachment(RootComponent);

	HealthBarWC = CreateDefaultSubobject<UEnemyHealthBar>(TEXT("Health Bar HUD"));
	HealthBarWC->SetupAttachment(GetRootComponent());	
	
	SensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Sensing Component"));

	ToBeAssassinatedCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("To Be Assassinated Collider"));
	ToBeAssassinatedCollider->SetupAttachment(GetRootComponent());
	ToBeAssassinatedLocation = CreateDefaultSubobject<USceneComponent>(TEXT("To Be Assassinated Location"));
	ToBeAssassinatedLocation->SetupAttachment(GetRootComponent());
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	Tags.Add("Enemy");
	InitializeEnemy();

	if (SensingComponent) SensingComponent->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);

	ToBeAssassinatedCollider->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnASBeginOverlap);
	ToBeAssassinatedCollider->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnASEndOverlap);
}

void AEnemy::InitializeEnemy()
{
	EnemyAIController = Cast<AAIController>(GetController());
	if (EnemyAIController && CurrentPatrolTarget) MoveTo(CurrentPatrolTarget);

	SpawnWeapon("WeaponBackSocket");
	WeaponState = EEnemyWeaponState::EEWS_Unequiped;
	LockOnVFX->Deactivate();

	if (HealthBarWC) HealthBarWC->SetVisibility(false);

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
}

void AEnemy::AttachWeaponToEnemy()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Equip(GetMesh(), FName("RightHandSocket"),this,this);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsDead() ) { return; }

	if (disableDedection == false)
	{
		if (DedectionLevelAmount > 0)
		{
			LastSeenTime += DeltaTime / slowDeltaTime;
		
			FString message = FString("Last seen time ") + FString::SanitizeFloat(LastSeenTime);
			//GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Red, message);
			if (LastSeenTime > DedectionLevelAmount)
			{
				isOutOfSight = true;
				GEngine->AddOnScreenDebugMessage(3, 2.f, FColor::Yellow, FString("is out of range"));
				if (HealthBarWC)
				{
					if (isOutOfSight == false) { return; }
					DedectionBarPercentage = HealthBarWC->GetDedectionBarPercentage();
					DedectionLevelAmount -= DeltaTime / slowDeltaTime;
					DedectionLevelAmount = FMath::Clamp(DedectionLevelAmount, 0, 1.f);
					HealthBarWC->SetDedectionLevelPercentage(DedectionLevelAmount);
				}
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(4, 2.f, FColor::Yellow, FString("is not out of range"));
				isOutOfSight = false;
			}
		}
		NavigatioAcceptableRadiusAndEngage();
	}


	if (IsInStunnedState()) { return; }
	(EnemyState > EEnemyState::EES_Patroling) ? CheckCombatTarget() : CheckPatrolTarget();
}

void AEnemy::NavigatioAcceptableRadiusAndEngage()
{
	if (!canEngage && NavigationAcceptableRadius != 300.f)
		NavigationAcceptableRadius = 300.f;
	else if (canEngage && NavigationAcceptableRadius != 20.f)
	{
		NavigationAcceptableRadius = 20.f;
		Chase();
	}
}

#pragma region AI
void AEnemy::CheckCombatTarget()
{
	if (!PlayerInRange())
	{
		ClearAttackTimer();
		GetWorldTimerManager().ClearTimer(InvestigateTimerHandle);
		LoseInterest();
	}
	else if (!IsInAttackRange() && !IsInChaseState() && !IsInPatrolingState() && !IsInStunnedState())
	{
		ClearAttackTimer();
		if (!IsInEngagedState())
		{
			Chase();
		}
	}
	else if (CanAttack())
	{
		StartToAttack();
	}
}

void AEnemy::MoveTo(AActor* NewDestination)
{
	if (!EnemyAIController && !NewDestination) { return; }
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(NewDestination);
	MoveRequest.SetAcceptanceRadius(NavigationAcceptableRadius);

	EnemyAIController->MoveTo(MoveRequest);
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	//UE_LOG(LogTemp, Warning, TEXT("There is a pawn"));

	bool bShouldChase = !IsDead() && 
		!IsInChaseState() &&
		SeenPawn->ActorHasTag("EngageableTarget") &&
		!IsInAttackingState() && !IsInEngagedState() && !IsInStunnedState() && IsInEquippedState();
	
	if (bShouldChase)
	{	
		if (disableDedection == false)
		{
			SpotedTime	+= UGameplayStatics::GetWorldDeltaSeconds(this);
			DedectionLevelAmount = SpotedTime * DedectionLevelIncreaseSpeedAmount;
			DedectionLevelAmount = FMath::Clamp(DedectionLevelAmount, 0, 1.f);
			if (HealthBarWC)
			{
				HealthBarWC->SetDedectionLevelPercentage(DedectionLevelAmount);
			}
			//UE_LOG(LogTemp, Warning, TEXT("Last Seen Time  %f"), DedectionLevelAmount);
			FString message = FString("SpotedTime is : ") + FString::SanitizeFloat(DedectionLevelAmount);
			isOutOfSight = false;
		//	GEngine->AddOnScreenDebugMessage(2, 1.f, FColor::Blue, message);

			
			if (DedectionLevelAmount < 1.f) { return; }
		}

		if (HealthBarWC) HealthBarWC->DisableDedectionBar();

		//UE_LOG(LogTemp, Warning, TEXT("Chase"));
		CombatTarget = SeenPawn;

		ClearPatrolTimer();
		GetWorldTimerManager().ClearTimer(PatrolTimeHandle);
		GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
		Chase();
		SetEnemyStateToChasing();
	}
	PlayerIsSpotted = true;
}

//range
bool AEnemy::IsInRangeOf(AActor* Target, float Radius)
{
	if (!Target) { return false; }
	float DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Length();
	return DistanceToTarget <= Radius;
}

bool AEnemy::PlayerInRange()
{
	return IsInRangeOf(CombatTarget, PlayerRange);
}

//states

bool AEnemy::IsInChaseState()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsInPatrolingState()
{
	return EnemyState == EEnemyState::EES_Patroling;
}

bool AEnemy::IsInAttackingState()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsInEngagedState()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

bool AEnemy::IsInStunnedState()
{
	return EnemyState == EEnemyState::EES_Stunned;
}

bool AEnemy::IsInEquippedState()
{
	return WeaponState == EEnemyWeaponState::EEWS_Equiped;
}

bool AEnemy::IsInUnequippedState()
{
	return WeaponState == EEnemyWeaponState::EEWS_Unequiped;
}

void AEnemy::SetEnemyStateToChasing()
{
	EnemyState = EEnemyState::EES_Chasing;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

#pragma region Patrol

bool AEnemy::IsInRangeOfTargetPoint()
{
	return IsInRangeOf(CurrentPatrolTarget, AcceptableRadius);
}

void AEnemy::CheckPatrolTarget()
{
	if (IsInRangeOfTargetPoint())
	{
		CurrentPatrolTarget = ChoosePatrolTarget();
		SetPatrolTimer();
	}
}

void AEnemy::Chase()
{
	if (!CombatTarget) { return; }
	if (IsInStunnedState()) { return; }

	SetEnemyStateToChasing();
	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	MoveTo(CombatTarget);
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	if(!IsInEngagedState()) EnemyState = EEnemyState::EES_Patroling;
	GetWorldTimerManager().SetTimer(InvestigateTimerHandle, this, &AEnemy::InvestigateTimer, InvestigationTime);
	PlayerIsSpotted = false;
}

void AEnemy::InvestigateTimer()
{
	MoveTo(CurrentPatrolTarget);
}

void AEnemy::PatrolTimeFinished()
{
	MoveTo(CurrentPatrolTarget);
}

void AEnemy::SetPatrolTimer()
{
	GetWorldTimerManager().SetTimer(PatrolTimeHandle, this, &AEnemy::PatrolTimeFinished, WaitTime);
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimeHandle);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != CurrentPatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	int32 Num = ValidTargets.Num();
	if (Num > 0)
	{
		const int32 randomInt = FMath::RandRange(0, Num - 1);
		AActor* NewTarget = ValidTargets[randomInt];
		return  NewTarget;
	}
	return nullptr;
}
#pragma endregion Patrol

#pragma region Combat
bool AEnemy::IsInAttackRange()
{
	return IsInRangeOf(CombatTarget, AttackRadius);
}

FVector AEnemy::GetTranslationWrapTarget()
{
	if (CombatTarget == nullptr) { return FVector(); }
	
	const FVector Location = GetActorLocation();
	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();

	FVector Distance = (CombatTargetLocation - Location).GetSafeNormal();
	Distance *= WrapTargetDistance;

	return CombatTargetLocation + Distance;
}

FVector AEnemy::GetRotationWrapTarget()
{
	if (CombatTarget == nullptr) { return FVector(); }
	return CombatTarget->GetActorLocation();
}

bool AEnemy::CanAttack()
{
	return !IsDead() && IsInAttackRange() && !IsInAttackingState() &&
		EnemyState != EEnemyState::ESS_NoState && !IsInEngagedState() && !IsInStunnedState();
}

void AEnemy::StartToAttack()
{
	SetEnemyStateToAttacking();
	ClearAttackTimer();
	SetAttackTimer();
}

void AEnemy::SetEnemyStateToAttacking()
{
	EnemyState = EEnemyState::EES_Attacking;
}

void AEnemy::Attack()
{
	Super::Attack();
	if (IsInStunnedState()) { return; }
	if (CombatTarget && CombatTarget->ActorHasTag("Dead")) { CombatTarget = nullptr; return; }
	EnemyState = EEnemyState::EES_Engaged;
	SetIsPlayingHitAnim(false);
	PlatAttackMontage();
}

void AEnemy::PlatAttackMontage()
{
	Super::PlatAttackMontage();
	PlayRandomMontageSection(AttackAnimMontage, AttackMontageSections);
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::ESS_NoState;
	Chase();
}

void AEnemy::SetAttackTimer()
{
	float WaitToAttack = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AEnemy::Attack, WaitToAttack);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimerHandle);
}

void AEnemy::StopAttackMontage()
{
	UAnimInstance* Instance = GetMesh()->GetAnimInstance();
	if (Instance)
		Instance->Montage_Stop(.25f, AttackAnimMontage);
}
#pragma endregion Combat

#pragma endregion AI

#pragma region TakeDamage

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (IsDead()) { return 0; }

	HandleDamage(DamageAmount);
	
	if (IsInAttackRange())
		EnemyState = EEnemyState::EES_Attacking;
	else if (!IsInAttackRange())
		EnemyState = EEnemyState::EES_Chasing;

	OnStunned();

	CombatTarget = EventInstigator->GetPawn();

	if (IsInPatrolingState() && LaunchState != ELaunchState::ELS_IsLaunching)
	{
		Chase();
	}
	return DamageAmount;
}

void AEnemy::OnStunned()
{
	if (isStunnedByIce)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), IceSpikeBreakVFX, GetActorLocation());
		hasBeenHit = true;
		GetWorldTimerManager().SetTimer(HasBeenHitTimerHandle, this, &AEnemy::HasBeenHitTimer, .5f);
		GetCharacterMovement()->MaxWalkSpeed = 0.f;
		if (PlayerController)
			PlayerController->ClientStartCameraShake(StunHitCamShake);
		isStunnedByIce = false;
	}
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (HealthBarWC)
	{
		HealthBarWC->SetHealthBarPercentage(AttributeComponent->GetHealthPercentage());
		HealthBarWC->SetDamageText(DamageAmount);
	}
}

void AEnemy::DirectionalHitReact(const FVector& HitLocation)
{
	Super::DirectionalHitReact(HitLocation);
	SetIsPlayingHitAnim(true);
}
	
void AEnemy::GetHit_Implementation(const FVector& HitLocation,AActor* Hitter)
{
	SetUpWeaponCollision(ECollisionEnabled::NoCollision);

	PlayHitSound(HitLocation);
	SpawnHitParticle(HitLocation);

	ClearPatrolTimer();
	ClearAttackTimer();
	StopAttackMontage();
	SetUpWeaponCollision(ECollisionEnabled::NoCollision);

	if (!IsAlive())
	{
		Die();
		return;
	}
	
	if (isPlayingHitAnim) { return; }
	if (!Hitter) { return; }
	DirectionalHitReact(Hitter->GetActorLocation());
}

void AEnemy::Die()
{
	Super::Die();


	if (ItemsToDrop.Num() > 0)
	{
		int32 randomNum = FMath::RandRange(0, ItemsToDrop.Num() - 1);
		GetWorld()->SpawnActor<AItem>(ItemsToDrop[randomNum],GetActorLocation(),GetActorRotation());
	}

	DisableCollision();
	PlayDeathMontage();
	ClearAttackTimer();

	if (PlayerController)
		PlayerController->ClientStartCameraShake(DeathCamShake);

	if (HealthBarWC)
		HealthBarWC->SetVisibility(false);
	EnemyState = EEnemyState::EES_Dead;
}

int32 AEnemy::PlayDeathMontage()
{
	const int32 Selection = Super::PlayDeathMontage();
	TEnumAsByte<EDeathPose> Pose(Selection);
	if (Pose < EDeathPose::EDS_MAX)
	{
		DeathPose = Pose;
	}
	return Selection;
}
//refactor
void AEnemy::Stun()
{	
	SetIsStuneedByShield(true);
}

void AEnemy::HasBeenHitTimer()
{
	hasBeenHit = false;
}


void AEnemy::SetIsStuneedByShield(bool newIsStunnedByShield)
{
	EnemyState = EEnemyState::EES_Stunned;
	isStunnedByShield = newIsStunnedByShield;

	if (StunShieldAnim) PlayAMontage(StunShieldAnim);

	GetWorldTimerManager().SetTimer(ShieldStunnTimerHandle, this, &AEnemy::ShieldStunnedTimer, ShieldStunTime);
}

void AEnemy::ShieldStunnedTimer()
{
	isStunnedByShield = false;
	SetUpWeaponCollision(ECollisionEnabled::NoCollision);
	AttackEnd();
}


#pragma endregion TakeDamage

#pragma region Launch
void AEnemy::LaunchCharacter(FVector direction,float launchForce)
{
	EnemyState = EEnemyState::EES_Stunned;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && OnQSHitLaunchMontage)
		AnimInstance->Montage_Play(OnQSHitLaunchMontage);

	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp)
		MovementComp->Launch(direction * launchForce);

}
#pragma endregion Launch

#pragma region ToBeAssassinated
void AEnemy::BeignAssassianted(AActor* OtherActor,AEnemy* Enemy, float OpacityPercent)
{
	if (IsPlayerSpotted()) { return; }
	ASlashCharacter* Player = Cast<ASlashCharacter>(OtherActor);
	if (!Player) { return; }
	if (IsDead())
	{
		Player->SetAssassinationOpacity(0.f);
		return;
	}
	Player->SetAssassinationTarget(Enemy);
	Player->SetAssassinationOpacity(OpacityPercent);
}

void AEnemy::OnASBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && (OtherActor == this || OtherActor == GetOwner())) { return; }
	BeignAssassianted(OtherActor,this,1.f);
}

void AEnemy::OnASEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	BeignAssassianted(OtherActor,nullptr,0.f);
}

void AEnemy::PlayBeignAssassinatedAnimation()
{
	if (IsPlayerSpotted()) { return; }
	PlayAMontage(BeignAssassinatedAnim);
	EnemyState = EEnemyState::EES_Dead;
	HasBeenAssassinated = true;
}

#pragma endregion ToBeAssassinated
