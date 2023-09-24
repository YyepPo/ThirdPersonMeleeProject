// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarUW.h"

void UHealthBarUW::NativeConstruct()
{
	Super::NativeConstruct();

	StoreWidgetAnimation();

	ColorChangeAnimation = GetAnimationByName(TEXT("ColorChange"));
}

void UHealthBarUW::StoreWidgetAnimation()
{
	AnimationMap.Empty();

	FProperty* Prop = GetClass()->PropertyLink;
	
	while (Prop)
	{
		//only deal with object property
		if (Prop->GetClass() == FObjectProperty::StaticClass())
		{
			FObjectProperty* ObjProp = Cast<FObjectProperty>(Prop);

			//only get properties that are widget animations
			if (ObjProp->PropertyClass == UWidgetAnimation::StaticClass())
			{
				UObject* Obj = ObjProp->GetObjectPropertyValue_InContainer(this);
				UWidgetAnimation* WidgetAnimation = Cast<UWidgetAnimation>(Obj);

				if (WidgetAnimation && WidgetAnimation->MovieScene)
				{
					FName AnimName = WidgetAnimation->MovieScene->GetFName();
					AnimationMap.Add(AnimName, WidgetAnimation);
				}
			}

		}
		Prop = Prop->PropertyLinkNext;
	}
}

UWidgetAnimation* UHealthBarUW::GetAnimationByName(FName AnimationName) const
{
	UWidgetAnimation* const* WidgetAnim = AnimationMap.Find(AnimationName);
	if (WidgetAnim)
	{
		return *WidgetAnim;
	}
	return nullptr;
}

void UHealthBarUW::PlayWidgetAnimation()
{
	if (ColorChangeAnimation)
	{
		PlayAnimation(ColorChangeAnimation, 0.f, 1.f, EUMGSequencePlayMode::Forward, 1.f);
	}
}
