// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

#include "Kismet/GameplayStatics.h"

#include "Components/CapsuleComponent.h"

#include "Treasure/ItemInteractionInterface.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "Blueprint/UserWidget.h"


AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = ItemMesh;

	CapsuleCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	CapsuleCollider->SetupAttachment(ItemMesh);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	Widget = CreateWidget(PlayerController, InteractionWidget);
	if (Widget && PlayerController)
	{
		Widget->AddToViewport();
		Widget->SetVisibility(ESlateVisibility::Collapsed);
	}

	CapsuleCollider->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnBeginOverlap);
	CapsuleCollider->OnComponentEndOverlap.AddDynamic(this, &AItem::OnEndOverlap);
}


void AItem::OnItemInteraction()
{
	if(CollectVFX) 	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, CollectVFX, GetActorLocation());

	UGameplayStatics::PlaySoundAtLocation(
		this,
		CollectSound,
		GetActorLocation()
	);
	Destroy();
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	Time += DeltaTime;
}

float AItem::GetSinusoidalValue()
{
	if (ItemState == EItemState::EIS_Equipped) { return 0; }

	float Sinusoidal = Amplitude * FMath::Sin(Time * Speed);
	return Sinusoidal;
}

float AItem::GetCosinusoidalValue()
{
	if (ItemState == EItemState::EIS_Equipped) { return 0; }

	float Cosinusoidal = Amplitude * FMath::Cos(Time * Speed);
	return Cosinusoidal;
}

void AItem::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IItemInteractionInterface* PickUpInterface = Cast<IItemInteractionInterface>(OtherActor);
	if (PickUpInterface == nullptr) { return; }
	
	PickUpInterface->SetOverlappingItem(this);
	if (Widget)Widget->SetVisibility(ESlateVisibility::Visible);
}

void AItem::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IItemInteractionInterface* PickUpInterface = Cast<IItemInteractionInterface>(OtherActor);
	if (PickUpInterface == nullptr) { return; }

	PickUpInterface->SetOverlappingItem(nullptr);
	if (Widget)Widget->SetVisibility(ESlateVisibility::Hidden);
}

void AItem::Equip(USceneComponent* Parent, FName SocketName, APawn* NewInsitagor, APawn* NewOwner)
{
	ItemState = EItemState::EIS_Equipped;

	SetOwner(NewOwner);
	SetInstigator(NewInsitagor);
	AttachMeshToSocket(Parent, SocketName);

	UCapsuleComponent* SphereCollision = GetCapsuleComponent();
	if (SphereCollision) SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItem::AttachMeshToSocket(USceneComponent* Parent, const FName& SocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Parent, TransformRules, SocketName);
}


