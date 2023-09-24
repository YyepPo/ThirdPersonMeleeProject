#include "Inventory/InventoryItemBase.h"

#include "Kismet/GameplayStatics.h"

#include "Components/CapsuleComponent.h"

#include "Inventory/Inventory.h"
#include "Characters/Interaction.h"

#include "SlashCharacter.h"

AInventoryItemBase::AInventoryItemBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Collider = CreateDefaultSubobject<UCapsuleComponent>(FName(TEXT("Capsule Collider")));
	SetRootComponent(Collider);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("Mesh")));
	Mesh->SetupAttachment(GetRootComponent());

}

void AInventoryItemBase::BuyItem_Implementation()
{
	if (Inventory) Inventory->AddItem(this);
}

void AInventoryItemBase::BeginPlay()
{
	Super::BeginPlay();

	SlashCharacter = Cast<ASlashCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (SlashCharacter) Inventory = SlashCharacter->GetInventoryComponent();
}

void AInventoryItemBase::PlaySound(USoundBase* SoundToPlay)
{
	if(SlashCharacter && SoundToPlay)
		UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, SlashCharacter->GetActorLocation());
}

void AInventoryItemBase::Interaction()
{
	UE_LOG(LogTemp, Warning, TEXT("Interacted with inventory item"));
	Collider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PlaySound(InteractionSound);
	OnItemInteractEvent();

	if (Inventory)
	{
		Inventory->AddItem(this);
		Mesh->DestroyComponent();
	}
}

void AInventoryItemBase::UseItem_Implementation()
{
//	if (GetIsMiscItem()) { return; }
	PlaySound(UseSound);
	if (isStockable && CurrentStockItem == 1)
		Destroy();
	else if (!isStockable)
		Destroy();
}

void AInventoryItemBase::OnRemoveItem()
{
	if (!Inventory) { return; }
	Inventory->RemoveItem(this);
}

void AInventoryItemBase::IncrementCurrentStockItem()
{
	CurrentStockItem++;
}

void AInventoryItemBase::DecrementCurrentStockItem()
{
	CurrentStockItem--;
}



