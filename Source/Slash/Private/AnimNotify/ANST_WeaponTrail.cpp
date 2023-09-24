#include "AnimNotify/ANST_WeaponTrail.h"
#include "SlashCharacter.h"
#include "Weapon/Weapon.h"
void UANST_WeaponTrail::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{	
	ASlashCharacter* Player = Cast<ASlashCharacter>(MeshComp->GetOwner());
	if (!Player) { return; }

	AWeapon* CurrentWeapon = Player->GetCurrentWeapon();
	if (!CurrentWeapon) { return; }

	if (!CurrentWeapon->GetWeaponTrail()) { return; }
	PSTemplate = CurrentWeapon->GetWeaponTrail();

}
