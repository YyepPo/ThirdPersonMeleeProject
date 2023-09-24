#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Treasure/Treasure.h"

ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("Geometry Collection Component"));
	GeometryCollection->SetupAttachment(RootComponent);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABreakableActor::GetHit_Implementation(const FVector& HitLocation,AActor* Hitter)
{
	//hasBeenHit = true;
	if (GetWorld() && TreasureObjects.Num() > 0 && !hasBeenSpawn)
	{
		FVector SpawnLoc = GetActorLocation();
		SpawnLoc.Z += ZOffSet;
		int32 randomIndex = FMath::RandRange(0, TreasureObjects.Num() - 1);
		GetWorld()->SpawnActor<ATreasure>(TreasureObjects[randomIndex], SpawnLoc, GetActorRotation());
		hasBeenSpawn = true;
	}
}
