#include "HangObject.h"
#include "Components/BoxComponent.h"
AHangObject::AHangObject()
{
	PrimaryActorTick.bCanEverTick = false;
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(FName(TEXT("Box Collider")));
	SetRootComponent(BoxCollider);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("Static Mesh")));
	Mesh->SetupAttachment(GetRootComponent());

	ToHangObject = CreateDefaultSubobject<USceneComponent>(FName(TEXT("Hang Position")));
	ToHangObject->SetupAttachment(GetRootComponent());
}

FTransform AHangObject::GetToHangPosition()
{
	return ToHangObject->GetComponentTransform();
}

void AHangObject::BeginPlay()
{
	Super::BeginPlay();
}
