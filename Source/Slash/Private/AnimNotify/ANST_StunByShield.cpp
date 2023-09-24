#include "AnimNotify/ANST_StunByShield.h"
#include "Enemy/Enemy.h"

void UANST_StunByShield::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (MeshComp && MeshComp->GetOwner())
		Enemy = Cast<AEnemy>(MeshComp->GetOwner());
}

void UANST_StunByShield::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	if (Enemy)Enemy->SetCanStunnedByShield(true);
}

void UANST_StunByShield::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (Enemy)Enemy->SetCanStunnedByShield(false);
}
