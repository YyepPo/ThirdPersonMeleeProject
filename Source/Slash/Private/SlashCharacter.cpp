#include "SlashCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"

#include "DrawDebugHelpers.h"

#include "Components/PlayerSkillComponent.h"
#include "Components/AttributeComponent.h"

#include "Camera/CameraComponent.h"
#include "Particles/ParticleSystemComponent.h"

#include "Enemy/Enemy.h"
#include "HUD/EnemyHealthBar.h"
#include "HUD/SlashHud.h"
#include "HUD/SlashOverlay.h"

#include "Treasure/Treasure.h"
#include "Treasure/Soul.h"
#include "Treasure/ItemInteractionInterface.h"

#include "Characters/Interaction.h"

#include "Inventory/Inventory.h"

#include "Weapon/Weapon.h"
#include "Weapon/BaseShield.h"

#include "HangObject.h"


ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	InventoryClass = CreateDefaultSubobject<UInventory>(FName(TEXT("Inventory")));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	LockOnCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Lock On Collider"));
	LockOnCollider->SetupAttachment(GetRootComponent());

	ToInteractWithCharacterCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction Collider"));
	ToInteractWithCharacterCollider->SetupAttachment(GetRootComponent());

	InitializeSkillComponents();
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	Controller = UGameplayStatics::GetPlayerController(this,0);

	//SpawnWeapon("Right_Hand_Socket");
	//if(CurrentWeapon) EquipWeapon(CurrentWeapon);
	LockOnCollider->OnComponentBeginOverlap.AddDynamic(this, &ASlashCharacter::LockOnColliderBeginOverlap);
	LockOnCollider->OnComponentEndOverlap.AddDynamic(this, &ASlashCharacter::LockOnColliderEndOverlap);

	QSkillCollider->OnComponentBeginOverlap.AddDynamic(this, &ASlashCharacter::OnQSkillBeginOverlap);
	QSkillCollider->OnComponentEndOverlap.AddDynamic(this, &ASlashCharacter::OnQSkillEndOverlap);

	CSkillCollider->OnComponentBeginOverlap.AddDynamic(this, &ASlashCharacter::OnCSkillBeginOverlap);
	CSkillCollider->OnComponentEndOverlap.AddDynamic(this, &ASlashCharacter::OnCSkillEndOverlap);

	ToInteractWithCharacterCollider->OnComponentBeginOverlap.AddDynamic(this, &ASlashCharacter::OnInteractionColliderBeginOverlap);
	ToInteractWithCharacterCollider->OnComponentEndOverlap.AddDynamic(this, &ASlashCharacter::OnInteractionColliderEndOverlap);


	Tags.Add("EngageableTarget");
	
	InitiliazeOverlay();

	if (!GetMesh()) { return; }
	GetMesh()->HideBoneByName(FName("weapon"), EPhysBodyOp::PBO_None);
}

void ASlashCharacter::InitiliazeOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		ASlashHud* SlashHud = Cast<ASlashHud>(PlayerController->GetHUD());
		if (SlashHud)
		{
			SlashOverlay = SlashHud->GetSlashOverlay();
			if (SlashOverlay && AttributeComponent)
			{
				UpdateHealthAndStaminaPerc();
				SlashOverlay->SetCointCountText(0);
				SlashOverlay->SetSoulCountText(0);
				SlashOverlay->SetAssassinationTextOpacity(0.f);
			}
		}
	}
}

void ASlashCharacter::UpdateHealthAndStaminaPerc()
{
	SlashOverlay->SetHealthBarPercentage(AttributeComponent->GetHealthPercentage());
	SlashOverlay->SetStaminBarPercentage(AttributeComponent->GetStaminPercentage());
}

void ASlashCharacter::InitializeSkillComponents()
{
	PlayerSkillComponent = CreateDefaultSubobject<UPlayerSkillComponent>(TEXT("Skill Component"));

	QSkillCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Q Skill Collider"));
	QSkillCollider->SetupAttachment(GetRootComponent());

	CSkillCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("C Skill Collider"));
	CSkillCollider->SetupAttachment(GetRootComponent());
	CSkillVFXLoc = CreateDefaultSubobject<USceneComponent>(TEXT("C SkillVFX location"));
	CSkillVFXLoc->SetupAttachment(GetRootComponent());

	SkillThreeBeamSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Beam SpawnPoint"));
	SkillThreeBeamSpawnPoint->SetupAttachment(RootComponent);
}

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpringArmZoomIn(DeltaTime);
	SpringArmZoomOut(DeltaTime);

	DeltaTimeTracker = DeltaTime;
	Time += DeltaTime;

	OnLockedOn();

	SkillCoolDowns(DeltaTime);
}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (IsBlocking()) { return 0; }
	HandleDamage(DamageAmount);
	if (SlashOverlay && AttributeComponent)
	{
		SlashOverlay->SetHealthBarPercentage(AttributeComponent->GetHealthPercentage());
	}
	return DamageAmount;
}

void ASlashCharacter::GetHit_Implementation(const FVector& HitLocation,AActor* Hitter)
{
	ActionState = EActionState::EAS_HitReaction;

	SetUpWeaponCollision(ECollisionEnabled::NoCollision);

	if (!IsAlive())
	{
		PlayHitSound(HitLocation);
		Die();
		return;
	}

	if (!Hitter || IsBlocking()) 
	{ 
		PlayAMontage(ShieldHitReaction);
		CurrentShield->SpawnShieldSparkEffect(HitLocation);
		ConsumeStamina(ShieldHitStaminaConsumeAmount);
		return; 
	}
	PlayHitSound(HitLocation);
	SpawnHitParticle(HitLocation);
	DirectionalHitReact(Hitter->GetActorLocation());
}

void ASlashCharacter::Die()
{
	Super::Die();
	Tags.Add("Dead");
	ActionState = EActionState::EAS_Dead;
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("Forward"), this, &ASlashCharacter::Forward);
	PlayerInputComponent->BindAxis(FName("Right"), this, &ASlashCharacter::Right);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ASlashCharacter::LookUp);
	PlayerInputComponent->BindAxis(FName("LookRight"), this, &ASlashCharacter::LookRight);

	PlayerInputComponent->BindAction(FName("Jump"), EInputEvent::IE_Pressed, this, &ASlashCharacter::Jump);

	PlayerInputComponent->BindAction(FName("Equip"), EInputEvent::IE_Pressed, this, &ASlashCharacter::ToEquip);
	PlayerInputComponent->BindAction(FName("EquipAnim"), EInputEvent::IE_Pressed, this, &ASlashCharacter::EquipAnim);

	PlayerInputComponent->BindAction(FName("Attack"), EInputEvent::IE_Pressed, this, &ASlashCharacter::Attack);

	PlayerInputComponent->BindAction(FName("DodgeRight"), EInputEvent::IE_Pressed, this, &ASlashCharacter::DodgeRight);
	PlayerInputComponent->BindAction(FName("DodgeLeft"), EInputEvent::IE_Pressed, this, &ASlashCharacter::DodgeLeft);
	PlayerInputComponent->BindAction(FName("DodgeBackwards"), EInputEvent::IE_Pressed, this, &ASlashCharacter::DodgeBackwards);

	PlayerInputComponent->BindAction(FName("LockOn"), EInputEvent::IE_Pressed, this, &ASlashCharacter::LockOn);
	PlayerInputComponent->BindAction(FName("LockOff"), EInputEvent::IE_Pressed, this, &ASlashCharacter::LockOff);
	
	PlayerInputComponent->BindAction(FName("SkillOne"), EInputEvent::IE_Pressed, this, &ASlashCharacter::UseSkillOne);
	PlayerInputComponent->BindAction(FName("SkillTwo"), EInputEvent::IE_Pressed, this, &ASlashCharacter::UseSkillTwo);
	PlayerInputComponent->BindAction(FName("SkillThree"), EInputEvent::IE_Pressed, this, &ASlashCharacter::UseSkillThree);

	PlayerInputComponent->BindAction(FName("Assassinate"), EInputEvent::IE_Pressed, this, &ASlashCharacter::Assassinate);

	PlayerInputComponent->BindAction(FName("ShieldHold"), EInputEvent::IE_Pressed, this, &ASlashCharacter::ShieldHold);
	PlayerInputComponent->BindAction(FName("ShieldRelease"), EInputEvent::IE_Released, this, &ASlashCharacter::ShieldRelease);

	PlayerInputComponent->BindAction(FName("ShieldBlock"), EInputEvent::IE_Pressed, this, &ASlashCharacter::Block);

	PlayerInputComponent->BindAction(FName("Hang"), EInputEvent::IE_Pressed, this, &ASlashCharacter::Hang);
}

#pragma region InputBindigs

void ASlashCharacter::Forward(float value)
{
	if (IsPlayerDead()) { return; }

	if (IsAttacking()) { return; }
	if (IsDodging()) { return; }
	if (CharacterState == ECharacterState::ECS_Hanging) { return; }

	WeaponSinusoidalMovement(value);

	const FRotator ControlRotation = GetControlRotation();
	const FRotator Direction(0.f, ControlRotation.Yaw, 0.f);
	FVector ForwardVector = FRotationMatrix(Direction).GetUnitAxis(EAxis::X);

	AddMovementInput(ForwardVector * value);

	(value > 0) ? CameraBobing(Speed) : CameraBobing(0);
}

void ASlashCharacter::Right(float value)
{
	if (IsPlayerDead()) { return; }

	if (IsAttacking()) { return; }
	if (IsDodging()) { return; }

	HangMovement(value,true);
	HangMovement(value, false);

	WeaponSinusoidalMovement(value);

	const FRotator ControlRotation = GetControlRotation();
	const FRotator Direction(0.f, ControlRotation.Yaw, 0.f);

	const FVector RightDirection = FRotationMatrix(Direction).GetUnitAxis(EAxis::Y);
	
	SmoothValue = FMath::FInterpTo(SmoothValue, value, DeltaTimeTracker, TurnRightInterpSpeed);
	AddMovementInput(RightDirection,SmoothValue);

	if (SmoothValue> 0) ZoomInOut(DeltaTimeTracker, OnMoveRightSpringArmSocketOffSet);
}

void ASlashCharacter::WeaponSinusoidalMovement(float value)
{
	if (value > 0 && bWeaponIsAttachedToBack && CurrentWeapon)
	{
		float Sinusoidal = WeaponEquipedAmplitude * FMath::Sin(Time * sinSpeed);
		CurrentWeapon->AddActorLocalOffset(FVector(0.f, 0.f, Sinusoidal));
	}
}

void ASlashCharacter::HangMovement(float& value, bool IsRight)
{
	if (CharacterState == ECharacterState::ECS_Hanging) {

		FHitResult HitResult;

		(IsRight) ? SideWaysShpereDistance = +SideWaysShpereDistance : SideWaysShpereDistance = -SideWaysShpereDistance;

		FVector MyLoc = GetActorLocation();
		FVector UpVector = GetActorUpVector() * UpVectorDistance;
		FVector ForwardVector = GetActorForwardVector() * ForwardVectorDistance;
		FVector RightVector = MyLoc + UpVector + ForwardVector + GetActorRightVector() * SideWaysShpereDistance;

		FCollisionShape Sphere = FCollisionShape::MakeSphere(SphereRadius);

		bool bHasHit = GetWorld()->SweepSingleByChannel(HitResult,
			MyLoc,
			RightVector,
			FQuat::Identity,
			ECollisionChannel::ECC_GameTraceChannel4,
			Sphere);
		DrawDebugSphere(GetWorld(), RightVector, SphereRadius, 12, FColor::Green, false);

		if (!bHasHit && IsRight)
		{
			value = FMath::Clamp(value, -1.f, 0.f);
		}
		else if (!bHasHit && !IsRight)
		{
			value = FMath::Clamp(value, 0,1.f);
		}
	}
}

void ASlashCharacter::LookUp(float value)
{
	if (LockOnState == ELockOnState::ELS_IsLockedOn) { return; }
	AddControllerPitchInput(value);
}

void ASlashCharacter::LookRight(float value)
{
	if (LockOnState == ELockOnState::ELS_IsLockedOn) { return; }
	if (CharacterState == ECharacterState::ECS_Hanging) { return; }
	AddControllerYawInput(value);
}

void ASlashCharacter::Jump()
{
	SetUpperBody(false);

	if (CharacterState == ECharacterState::ECS_Hanging)
	{
		FHitResult HitResult;

		FVector MyLoc = GetActorLocation();
		FVector EndLoc = MyLoc + HangUpSpherePosition;

		FCollisionShape Sphere = FCollisionShape::MakeSphere(SphereRadius);

		DrawDebugSphere(GetWorld(), EndLoc, 20.f, 12, FColor::Blue, false);

		bool hasHit = GetWorld()->SweepSingleByChannel(HitResult,
			MyLoc,
			EndLoc,
			FQuat::Identity,
			ECollisionChannel::ECC_Visibility,
			Sphere);

		if (hasHit)
		{
			HangUpHitPoint = HitResult.ImpactPoint;
			PlayAMontage(HangToCrouchMontage);
		}

		return;
	}
	
	ACharacter::Jump();
}

void ASlashCharacter::LockOn()
{
	LockOnToEnemy();

}
//block behaviour
void ASlashCharacter::ShieldHold()
{
	isHoldingShieldKey = true;
	if (!CurrentShield) { return; }
	if (!isHoldingShieldKey) { return; }
	CharacterState = ECharacterState::ECS_ShieldEquipped;
}

void ASlashCharacter::ShieldRelease()
{
	isHoldingShieldKey = false;
	
	if (CurrentWeapon)
	{
		CharacterState = ECharacterState::ECS_OneHandWeaponEquipped;
		GetCharacterMovement()->MaxWalkSpeed = OnLockOnMovementSpeed;
	}
	else
		CharacterState = ECharacterState::ECS_NoWeaponEquipped;
}

void ASlashCharacter::Block()
{
	if (!CurrentShield) { return; }
	if ( !isHoldingShieldKey) { return; }

	PlayAMontage(ShieldBlockOnMontage);
	ConsumeStamina(ShieldBlockStaminaConsumeAmount);

	CurrentShield->SetSheildCollision(ECollisionEnabled::QueryOnly);
	SetCanStunn(true);
	
	if (CurrentEnemy && CurrentEnemy->GetCanStunnedByShield() && canStunn)
	{
		CurrentEnemy->SetIsStuneedByShield(true);
		PlayCameraShake(Controller, BlockShake);
		ShieldSlowMotion();
	}

}

bool ASlashCharacter::SetCanStunn(bool newBool)
{
	if (!CurrentShield) { return false; }
	CurrentShield->SetSheildCollision(ECollisionEnabled::NoCollision);
	return canStunn = newBool;
}

#pragma endregion InputBindings

#pragma region Geters

AWeapon* ASlashCharacter::GetCurrentWeapon()
{
	if (!CurrentWeapon)
	{
		return nullptr;
	}
	return CurrentWeapon;
}

UCharacterMovementComponent* ASlashCharacter::GetPlayerMovementComponent()
{
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp == nullptr) { return nullptr; }

	return MovementComp;
}

void ASlashCharacter::HealthBoostAttributeComp(float boostAmount)
{
	if (AttributeComponent && SlashOverlay)
	{
		AttributeComponent->HealthBoost(boostAmount);
		SlashOverlay->SetHealthBarPercentage(AttributeComponent->GetHealthPercentage());
	}
}

void ASlashCharacter::StaminaBoostAttributeComp(float boostAmount)
{
	if (AttributeComponent && SlashOverlay)
	{
		AttributeComponent->StaminaBoost(boostAmount);
		SlashOverlay->SetStaminBarPercentage(AttributeComponent->GetStaminPercentage());
	}
}

#pragma endregion Geters

#pragma region Combat

bool ASlashCharacter::IsCombatTargetValid()
{
	if (CurrentEnemy == nullptr) { return false; }

	return CurrentEnemy != nullptr;
}

FVector ASlashCharacter::GetTargetWarpTransform()
{
	if (!IsCombatTargetValid()) { return FVector(); }

	const FVector MyLocation = GetActorLocation();
	const FVector TargetLocation = CurrentEnemy->GetActorLocation();

	FVector Distance = (TargetLocation - MyLocation).GetSafeNormal();
	Distance *= WrapTargetDistance;

	return TargetLocation + Distance;
}

FVector ASlashCharacter::GetTargetWarpRotation()
{
	if (!IsCombatTargetValid()) { return FVector(); }

	return CurrentEnemy->GetActorLocation();
}

void ASlashCharacter::ConsumeStaminaAnimNotify()
{
	ConsumeStamina(AttackStaminConsumeAmount);
}

bool ASlashCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied && ActionState != EActionState::EAS_HitReaction
		&& (CharacterState == ECharacterState::ECS_OneHandWeaponEquipped || CharacterState == ECharacterState::ECS_ShieldEquipped) 
		&& !IsDodging();
}
//input binding
void ASlashCharacter::Attack()
{
	Super::Attack();

	if (CanAttack())
	{
		PlatAttackMontage();
		CalculateClosestEnemy();
		ConsumeStamina(AttackStaminConsumeAmount);
		ActionState = EActionState::EAS_Attacking;
		if (AttributeComponent && AttributeComponent->GetStaminaAmount() != 200.f)
			GetWorldTimerManager().SetTimer(StaminaTimerHandle, this, &ASlashCharacter::StaminaTimer, TimeToStaminaRecharge,true);
	}
}

void ASlashCharacter::StaminaTimer()
{
	if (!AttributeComponent) { return; }
	UE_LOG(LogTemp, Warning, TEXT("Stamina Timer"));
	AttributeComponent->IncreaseStaminaOverTime(StaminaOverTimeAmount, DeltaTimeTracker, StaminaInterpSpeed);
	ConsumeStamina(0);
}

void ASlashCharacter::ConsumeStamina(float ConsumeAmount)
{
	if (AttributeComponent && SlashOverlay) {
		AttributeComponent->UseStamina(ConsumeAmount);
		SlashOverlay->SetStaminBarPercentage(AttributeComponent->GetStaminPercentage());
	}
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::PlatAttackMontage()
{
	Super::PlatAttackMontage();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AttackAnimMontage)
	{
		FName Section;
		(LockOnState == ELockOnState::ELS_IsLockedOn) ? Section = "Attack1" : Section = "Attack2";

		if (CharacterState == ECharacterState::ECS_OneHandWeaponEquipped)
		{
			SetUpperBody(true);
			AnimInstance->Montage_Play(AttackAnimMontage);
			AnimInstance->Montage_JumpToSection(Section, AttackAnimMontage);
		}
	}
}

void ASlashCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

#pragma endregion Combat

#pragma region Arm and Unarm
//binding function
void ASlashCharacter::ToEquip()
{
	AWeapon* OverlapingWeapon = Cast<AWeapon>(OverlapingItem);
	if (OverlapingWeapon != nullptr) EquipWeapon(OverlapingWeapon);	

	ABaseShield* OverlapingShield = Cast<ABaseShield>(OverlapingItem);
	if (OverlapingShield)
	{
		EquipShield(OverlapingShield);
	}

	if (CurrentCharacterInteraction) CurrentCharacterInteraction->Interaction();

	AddSouls(Cast<ASoul>(OverlapingItem));
	AddGold(Cast<ATreasure>(OverlapingItem));
}

void ASlashCharacter::EquipAnim()
{
	if (CanDisarm()) Disarm();
	else if (CanArm()) Arm();
}

void ASlashCharacter::Disarm()
{
	SetUpperBody(true);
	PlayEquipAnim(WeaponUnequipAnim);
	CharacterState = ECharacterState::ECS_NoWeaponEquipped;
}

void ASlashCharacter::Arm()
{
	SetUpperBody(true);
	PlayEquipAnim(WeaponEquipMAnim);
	if (CurrentWeapon && CurrentWeapon->GetIsOneHandedWeapon())
		CharacterState = ECharacterState::ECS_OneHandWeaponEquipped;
}

void ASlashCharacter::EquipWeapon(AWeapon* Weapon)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	if (Weapon->GetIsOneHandedWeapon())
	{
		CharacterState = ECharacterState::ECS_OneHandWeaponEquipped;
		Weapon->Equip(GetMesh(), FName("Right_Hand_Socket"), this, this);
		GetPlayerMovementComponent()->MaxWalkSpeed = OneHandedMovementSpeed;
	}

	if (CurrentWeapon == nullptr) CurrentWeapon = Weapon;
}

void ASlashCharacter::EquipShield(ABaseShield* Shield)
{
	if (!Shield) { return; }
	Shield->Equip(GetMesh(), TEXT("ShieldSocket"), this, this);
	CurrentShield = Shield;
}

void ASlashCharacter::AttachWeaponToRightHand()
{
	if (!GetMesh()) { return; }
	CurrentWeapon->Equip(GetMesh(), FName("Right_Hand_Socket"),this,this);
	bWeaponIsAttachedToBack = false;
}

void ASlashCharacter::AttachWeaponToBack()
{
	if (!GetMesh()) { return; }
	CurrentWeapon->Equip(GetMesh(), FName("ArmEquip"),this,this);
	bWeaponIsAttachedToBack = true;
}

bool ASlashCharacter::CanDisarm()
{
	return 
		CharacterState == ECharacterState::ECS_OneHandWeaponEquipped &&
		ActionState == EActionState::EAS_Unoccupied && DodgeState == EDodgeState::EDS_HasDodge;
}

bool ASlashCharacter::CanArm()
{
	return  CharacterState == ECharacterState::ECS_NoWeaponEquipped && CurrentWeapon &&
		ActionState == EActionState::EAS_Unoccupied && DodgeState == EDodgeState::EDS_HasDodge;
}

void ASlashCharacter::PlayEquipAnim(UAnimMontage* MontageName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && MontageName)
	{
		AnimInstance->Montage_Play(MontageName);
	}
}

#pragma endregion Arm and Unarm

#pragma region Dodge
//refactor the entire dodge function
void ASlashCharacter::DodgeRight()
{
	if (!CanPerformDodge()) { return; }
	PlayMontageSections(DogeAnim, FName("Right"));
	ConsumeStamina(DodgeStaminaConsumeAmount);
}

void ASlashCharacter::DodgeLeft()
{
	if (!CanPerformDodge()) { return; }
	PlayMontageSections(DogeAnim, FName("Left"));
	ConsumeStamina(DodgeStaminaConsumeAmount);
}

void ASlashCharacter::DodgeBackwards()
{
	if (!CanPerformDodge()) { return; }
	PlayMontageSections(DogeAnim, FName("Backwards"));
	ConsumeStamina(DodgeStaminaConsumeAmount);
}

bool ASlashCharacter::CanPerformDodge()
{
	if (IsAttacking() || IsDodging()) { return false; }
	DodgeState = EDodgeState::EDS_Dodging;
	return true;
}

#pragma endregion Dodge

#pragma region CameraAndSpringArm

void ASlashCharacter::CameraBobing(float SinusoidalSpeed)
{
	if (!Camera) { return; }
	float sin = Amplitude * FMath::Sin(SinusoidalSpeed * Time);
	FVector OffSet(0.f, 0.f, sin);
	Camera->AddLocalOffset(OffSet);
}

bool ASlashCharacter::IsVelocityEqaulTo(float Velocity)
{
	if (LockOnState == ELockOnState::ELS_IsLockedOn) { return false; }
	return GetPlayerMovementComponent()->Velocity.Length() == Velocity;
}

void ASlashCharacter::SpringArmZoomIn(float Delta)
{
	if(IsVelocityEqaulTo(0.f)) ZoomInOut(Delta, ZoomInAmount);
}

void ASlashCharacter::SpringArmZoomOut(float Delta)
{
	if(!IsVelocityEqaulTo(0.f)) ZoomInOut(Delta, ZoomOutAmount);
}

void ASlashCharacter::ZoomInOut(float deltaTime, float ZoomInOutAmount)
{
	if (LockOnState == ELockOnState::ELS_IsLockedOn) { return; }

	float zoomInOut = FMath::FInterpTo(SpringArm->TargetArmLength, ZoomInOutAmount, deltaTime, InterpSpeed);
	SpringArm->TargetArmLength = zoomInOut;
}

#pragma endregion CameraAndSpringArm

#pragma region SkillUse

void ASlashCharacter::SkillCoolDowns(float deltaTime)
{
	if (QSkillCDRemainingTime > 0)
		QSkillCDRemainingTime -= deltaTime;
}

#pragma region SkillOne

void ASlashCharacter::OnQSkillBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && (OtherActor == this || OtherActor == GetOwner())) { return; }
	AEnemy* OverlapingEnemies = Cast<AEnemy>(OtherActor);
	if (OverlapingEnemies)
	{
		PlayerSkillComponent->Enemies.AddUnique(OverlapingEnemies);
	}
}

void ASlashCharacter::OnQSkillEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AEnemy* OverlapingEnemies = Cast<AEnemy>(OtherActor);
	if (OverlapingEnemies)
	{
		PlayerSkillComponent->Enemies.Remove(OverlapingEnemies);
	}
}

void ASlashCharacter::UseSkillOne()
{
	if (!PlayerSkillComponent) { return; }

	if(QSkillCDRemainingTime <= 0)
	{ 
		if (CanAttack())
			PlayerSkillComponent->SkillOne();
		
		QSkillCDRemainingTime = PlayerSkillComponent->GetQSkillCooldown();

		ActionState = EActionState::EAS_Attacking;
	}
}

#pragma endregion SkillOne

#pragma region SkillTwo
void ASlashCharacter::OnCSkillBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this && OtherActor == GetOwner()) { return; }
	AEnemy* newEnemy = Cast<AEnemy>(OtherActor);
	if (newEnemy && PlayerSkillComponent)
		PlayerSkillComponent->SetSkillsTwoNewEnemy(newEnemy);
}

void ASlashCharacter::OnCSkillEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AEnemy* newEnemy = Cast<AEnemy>(OtherActor);
	if (newEnemy)
		PlayerSkillComponent->SetSkillsTwoNewEnemy(nullptr);
}

void ASlashCharacter::UseSkillTwo()
{
	if (!PlayerSkillComponent) { return; }
	PlayerSkillComponent->SkillTwo();
}

#pragma endregion SkillTwo

#pragma region SkillThree

void ASlashCharacter::UseSkillThree()
{
	if (!PlayerSkillComponent) { return; }
	PlayerSkillComponent->SkillThree();
	CanDisengage = true;
	OnSkillThreeUsed();
	DisengageLock();
}

void ASlashCharacter::GetSkillThreeBehaviour()
{
	if (!PlayerSkillComponent) { return; }
	PlayerSkillComponent->SkillThreeBehaviour();
}

#pragma endregion SkillThree

#pragma endregion SkillUse

#pragma region LockOnToEnemy

void ASlashCharacter::OnLockedOn()
{
	if (IsLockedOn)
	{
		if (CurrentEnemy && CurrentEnemy->IsDead())
		{
			if (Enemies.Num() > 0)
			{
				Enemies.Remove(CurrentEnemy);
				CanDisengage = false;
				int32 randomInt = FMath::RandRange(0, Enemies.Num() - 1);
				CurrentEnemy = Enemies[randomInt];
			}
			else
				CanDisengage = true;

			if (TargetIndex <= 0)
			{
				DisengageLock();
				return;
			}
		}
		EngageLock();
	}
}

void ASlashCharacter::LockOnToEnemy()
{
	if (IsLockedOn)
	{
		if (TargetIndex >= Enemies.Num() - 1)
			DisengageLock();
		else
			SwapEnemies();	
	}
	else
	{
		if (Enemies.Num() > 0)
		{
			CurrentEnemy = Enemies[TargetIndex];
			IsLockedOn = true;
			EngageLock();
		}
	}
}

void ASlashCharacter::SwapEnemies()
{
	if (Enemies.Num() < 0) { return; }

	TargetIndex++;
	CurrentEnemy = Enemies[TargetIndex];
	EngageLock();
}

void ASlashCharacter::EngageLock()
{
	if (Enemies.Num() < 0) { return; }

	if (CurrentEnemy)
	{
		FVector MyLoc = GetActorLocation();
		FVector EnemyLoc = CurrentEnemy->GetActorLocation();
		FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(MyLoc, EnemyLoc);

		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

		IsLockedOn = true;

		if (!PlayerController) { return; }

		FRotator SmoothLookRotation = FMath::RInterpTo(GetControlRotation(), LookRotation, DeltaTimeTracker, ToLookRotationInterpSpeed);
		PlayerController->SetControlRotation(SmoothLookRotation);

		SetActorRotation(SmoothLookRotation);

		float DistanceBtwEnemy = FVector::Dist(CurrentEnemy->GetActorLocation(), GetActorLocation());

		float smothvalue = FMath::FInterpTo(Camera->FieldOfView, CameraFovOnLock, DeltaTimeTracker, InterpSpeed);
		Camera->SetFieldOfView(smothvalue);

		LockOnParticle(true);
		CurrentEnemy->HealthBarWC->SetVisibility(true);
		SetLockOnState(ELockOnState::ELS_IsLockedOn);
		GetCharacterMovement()->MaxWalkSpeed = OnLockOnMovementSpeed;

		OnLockOnEvent();

		if (DistanceBtwEnemy > MaxEngageDistance)
		{
			CanDisengage = true;
			DisengageLock();
		}
	}
	else
	{
		CanDisengage = true;
		DisengageLock();
	}
}

void ASlashCharacter::DisengageLock()
{
	if (Enemies.Num() < 0) { return; }
	if (!CanDisengage) { return; }

	TargetIndex = 0;
	IsLockedOn = false;

	Camera->SetFieldOfView(CameraFov0ffLock);

	GetCharacterMovement()->MaxWalkSpeed = OneHandedMovementSpeed;

	LockOnParticle(false);
	OnLockOffEvent();
	SetLockOnState(ELockOnState::ELS_IsLockedOff);
}

void ASlashCharacter::LockOff()
{
	CanDisengage = true;
	DisengageLock();
}

void ASlashCharacter::LockOnParticle(bool isLocked)
{
	if (CurrentEnemy)
	{
		AEnemy* enemy = Cast<AEnemy>(CurrentEnemy);
		if (enemy)
		{
			UParticleSystemComponent* lockOnVFX = enemy->GetLockOnVFX();
			(isLocked) ? lockOnVFX->Activate() : lockOnVFX->Deactivate();
		}
	}
}

void ASlashCharacter::SetLockOnState(ELockOnState State)
{
	LockOnState = State;
}

void ASlashCharacter::LockOnColliderBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor == this || OtherActor == GetOwner()) && OtherActor != nullptr) { return; }

	AEnemy* Enemy = Cast<AEnemy>(OtherActor);
	if (!Enemy) { return; }

	Enemies.AddUnique(Enemy);

	if (Enemies.Num() < 0 && ClosestEnemy != nullptr) return;
	ClosestEnemy = Enemies[0];
	ClosestEnemy->SetCanEngage(true);
}

void ASlashCharacter::LockOnColliderEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AEnemy* Enemy = Cast<AEnemy>(OtherActor);
	if (!Enemy) { return; }
	Enemies.Remove(Enemy);
	NotClosestEnemies.Remove(Enemy);
}

#pragma endregion LockOnToEnemy

#pragma region Assassination
void ASlashCharacter::SetAssassinationTarget(AEnemy* NewTarget)
{
	AssassinationTarget = NewTarget;
}

FTransform ASlashCharacter::UpdateASTargetTransform()
{
	if (!AssassinationTarget) { return FTransform(); }
	FTransform	Location = AssassinationTarget->GetAssassinationLocation()->GetComponentTransform();
	return Location;
}

void ASlashCharacter::SetAssassinationOpacity(float Percent)
{
	if (SlashOverlay) SlashOverlay->SetAssassinationTextOpacity(Percent);
}

void ASlashCharacter::Assassinate()
{
	if (!AssassinationTarget) { return; }
	if (AssassinationTarget && AssassinationTarget->IsDead()) { return; }
	if (LockOnState == ELockOnState::ELS_IsLockedOn) LockOnState = ELockOnState::ELS_IsLockedOff;

	AssassinationTarget->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AssassinationTarget->PlayBeignAssassinatedAnimation();

	PlayAMontage(AssassianteMontage);
}


#pragma endregion Assassination

#pragma region Overlay
void ASlashCharacter::SetOverlappingItem(AItem* Item)
{
	OverlapingItem = Item;
}

bool ASlashCharacter::CanInteractWithItem()
{
	return OverlapingItem && AttributeComponent && SlashOverlay;
}

void ASlashCharacter::AddSouls(ASoul* Soul)
{
	if (Soul && CanInteractWithItem())
	{
		AttributeComponent->AddSoul(Soul->GetSoulAmount());
		SlashOverlay->SetSoulCountText(AttributeComponent->GetCurrentSoulAmount());
		Soul->UseItem();
	}
}

void ASlashCharacter::AddGold(ATreasure* Gold)
{
	if (Gold && CanInteractWithItem())
	{
		IncreaseGold(Gold->GetGoldAmount());
		SlashOverlay->SetCointCountText(AttributeComponent->GetCurrentCoinAmount());
		Gold->UseItem();
	}
}

void ASlashCharacter::IncreaseGold(float GoldAmount)
{
	AttributeComponent->AddCoin(GoldAmount);
}

void ASlashCharacter::OnInteractionColliderBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IInteraction* CharacterToInteractWith = Cast<IInteraction>(OtherActor);
	if (CharacterToInteractWith)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enes"));
		CurrentCharacterInteraction = CharacterToInteractWith;
	}
}

void ASlashCharacter::OnInteractionColliderEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IInteraction* CharacterToInteractWith = Cast<IInteraction>(OtherActor);
	if (CharacterToInteractWith)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASDASDA"));
		CurrentCharacterInteraction = nullptr;
	}
}


#pragma endregion Overlay

void ASlashCharacter::CalculateClosestEnemy()
{
	if (Enemies.Num() < 0) { return; }
	
	for (int32 i = 0; i < Enemies.Num(); i++)
	{
		if (ClosestEnemy)
		{
			DistanceForClosestEnemy = ClosestEnemy->GetActorLocation() - GetActorLocation();
		}
		FVector EnemiesLocations = Enemies[i]->GetActorLocation();
		FVector MyLoc = GetActorLocation();
		DistanceBtwEnemies = (EnemiesLocations - MyLoc);
		if (DistanceBtwEnemies.Size() < DistanceForClosestEnemy.Size())
		{
			ClosestEnemy = Enemies[i];
		}
	}
	if (!ClosestEnemy) { return; }

	ClosestEnemy->SetCanEngage(true);

	UE_LOG(LogTemp, Warning, TEXT("Closest Enemy is %s"), *ClosestEnemy->GetActorNameOrLabel());
	UE_LOG(LogTemp, Warning, TEXT("Closest Enemy distance %f"),DistanceForClosestEnemy.Size());
	UE_LOG(LogTemp, Warning, TEXT(" Enemies distance %f"), DistanceBtwEnemies.Size());
}

#pragma region Hang
void ASlashCharacter::Hang()
{
	FHitResult HitResult;

	FVector MyLoc = GetActorLocation();
	FVector EndLoc = MyLoc + GetActorUpVector() * UpVectorLength;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(SphereRadius);

	bool hasHit = GetWorld()->SweepSingleByChannel(HitResult,
		MyLoc, EndLoc,
		FQuat::Identity,
		ECC_GameTraceChannel4,
		Sphere);

	if (hasHit)
	{
		if (!HitResult.GetActor()) { return; }
		AHangObject* HangObject = Cast<AHangObject>(HitResult.GetActor());
		if (!HangObject) { return; }

		GetPlayerMovementComponent()->bOrientRotationToMovement = false;
		GetPlayerMovementComponent()->bUseControllerDesiredRotation = false;
		GetPlayerMovementComponent()->SetMovementMode(EMovementMode::MOVE_Flying);

		HitPoint = HangObject->GetToHangPosition();

		PlayAMontage(ToHangMontage);

		CharacterState = ECharacterState::ECS_Hanging;	
	}
}
FTransform ASlashCharacter::GetHangTransform()
{
	return HitPoint;
}
FVector ASlashCharacter::GetHangUpHitPoint()
{
	return HangUpHitPoint;
}
#pragma endregion Hang