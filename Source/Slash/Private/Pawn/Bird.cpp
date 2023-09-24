

#include "Pawn/Bird.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

#include "DrawDebugHelpers.h"

ABird::ABird()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	RootComponent = CapsuleCollider;
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	Mesh->SetupAttachment(CapsuleCollider);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	PawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Pawn Floating Component"));

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ABird::BeginPlay()
{
	Super::BeginPlay();

	DefaultMovementSpeed = PawnMovement->GetMaxSpeed();
}


void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ZoomInOutBehaviour(DeltaTime);
	IsEnemyInVision(HasSeenEnemy,EnemiesStaticMesh);
}

void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(FName("Forward"), this, &ABird::Forward);
	PlayerInputComponent->BindAxis(FName("Right"), this, &ABird::Right);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ABird::LookUp);
	PlayerInputComponent->BindAxis(FName("LookRight"), this, &ABird::LookRight);
	
	PlayerInputComponent->BindAxis(FName("Up"), this, &ABird::MoveUp);

	PlayerInputComponent->BindAction(FName("RightClickPressed"), EInputEvent::IE_Pressed, this, &ABird::RightClickPressed);
	PlayerInputComponent->BindAction(FName("RightClickReleased"), EInputEvent::IE_Released, this, &ABird::RightClickReleased);

	PlayerInputComponent->BindAction(FName("HighlightEnemyFromBird"), EInputEvent::IE_Pressed, this, &ABird::HighlightEnemy);
}

void ABird::Forward(float value)
{
	AddMovementInput(GetActorForwardVector() , value);
}

void ABird::Right(float value)
{
	AddMovementInput(GetActorRightVector() , value);
}

void ABird::LookUp(float value)
{
	AddControllerPitchInput(value);
}

void ABird::LookRight(float value)
{
	AddControllerYawInput(value);
}

void ABird::MoveUp(float value)
{
	AddMovementInput(GetActorUpVector(), value);
}

void ABird::RightClickPressed()
{
	IsHoldingRightClick = true;
}

void ABird::RightClickReleased()
{
	IsHoldingRightClick = false;
}

void ABird::HighlightEnemy()
{
	if (HasSeenEnemy && EnemiesStaticMesh != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(2, 1.f, FColor::Red,"Pointing at an enemy");	
		EnemiesStaticMesh->SetRenderCustomDepth(true);
	}
}

void ABird::ZoomInOutBehaviour(float DeltaSeconds)
{
	if (IsHoldingRightClick)
	{
		float ZoomIn = FMath::FInterpTo(SpringArm->TargetArmLength, ZoomInAmount, DeltaSeconds, InterpSpeed);
		SpringArm->TargetArmLength = ZoomIn;
		PawnMovement->MaxSpeed = ZoomInMovementSpeed;
	}
	else
	{
		float ZoomOut = FMath::FInterpTo(SpringArm->TargetArmLength, ZoomOutAmount, DeltaSeconds, InterpSpeed);
		SpringArm->TargetArmLength = ZoomOut;
		PawnMovement->MaxSpeed = DefaultMovementSpeed;
	}
}

void ABird::IsEnemyInVision(bool & RefHasHit,UStaticMeshComponent* & RefEnemiesStaticMesh)
{
	FVector StartLoc = GetActorLocation();
	FVector EndLoc = StartLoc + GetActorForwardVector() * RayCastLength;

	if (GetWorld())
	{
		DrawDebugLine(GetWorld(), GetActorLocation(), EndLoc, FColor::Blue, false,-1.f);
	}

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	FHitResult HitResult;
	bool hasHit = GetWorld()->LineTraceSingleByChannel(HitResult,
		StartLoc,
		EndLoc,
		ECC_GameTraceChannel1,
		Params);

	if (hasHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor == nullptr) { return; }
		GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Yellow, *HitActor->GetActorNameOrLabel());

		UStaticMeshComponent* HitStaticMesh = Cast<UStaticMeshComponent>(HitActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (HitStaticMesh == nullptr) {
			GEngine->AddOnScreenDebugMessage(2, 1.f, FColor::Red, "There is no Staticmesh Component to render highlight");
			return;
		}
		RefEnemiesStaticMesh = HitStaticMesh;
	}
	RefHasHit = hasHit;
}
