#include "Characters/ShopOwner.h"

#include "Components/CapsuleComponent.h"

#include "Characters/Interaction.h"
#include "Kismet/GameplayStatics.h"
#include "SlashCharacter.h"
#include "Blueprint/UserWidget.h"

AShopOwner::AShopOwner()
{
	PrimaryActorTick.bCanEverTick = false;

	CapsuleCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	SetRootComponent(CapsuleCollider);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	Mesh->SetupAttachment(GetRootComponent());
}

void AShopOwner::BeginPlay()
{
	Super::BeginPlay();
	Player = Cast<ASlashCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	PlayerController = UGameplayStatics::GetPlayerController(this,0);

	PlayerInventory = CreateWidget(PlayerController, PlayerInventoryUI);
	ShopInventory = CreateWidget(PlayerController, ShopInventoryUI);
	if (PlayerInventory && ShopInventory)
	{
		PlayerInventory->AddToViewport(0);
		PlayerInventory->SetVisibility(ESlateVisibility::Hidden);
		ShopInventory->AddToViewport(0);
		ShopInventory->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AShopOwner::Interaction()
{
	UE_LOG(LogTemp, Warning, TEXT("Interaction With shop owner"));
	if (!Player && !PlayerController && !PlayerInventory) { return; }

	Player->DisableInput(PlayerController);
	PlayerController->SetShowMouseCursor(true);
	PlayerInventory->SetVisibility(ESlateVisibility::Visible);
	ShopInventory->SetVisibility(ESlateVisibility::Visible);
}


