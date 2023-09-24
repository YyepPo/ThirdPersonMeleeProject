// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AN_ForwardDash.h"
#include "SlashCharacter.h"

void UAN_ForwardDash::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	ASlashCharacter* Player = Cast<ASlashCharacter>(MeshComp->GetOwner());
	if (Player == nullptr) {
		GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Red, "there is a controller");
		return;
	}

	Player->LaunchCharacter(Player->GetActorForwardVector() * Speed, true, false);
}
