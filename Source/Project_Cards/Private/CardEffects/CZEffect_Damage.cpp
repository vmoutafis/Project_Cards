// Fill out your copyright notice in the Description page of Project Settings.


#include "CardEffects/CZEffect_Damage.h"

#include "CZCard.h"
#include "Kismet/GameplayStatics.h"

UCZEffect_Damage::UCZEffect_Damage()
{
	Damage = 1;
}

FString UCZEffect_Damage::GetDescription() const
{
	return FString("Deal ") + FString::FromInt(GetScaledDamage()) + FString(" Damage.");
}

int UCZEffect_Damage::GetScaledDamage() const
{
	if (!IsValid(GetSourceStats()))
		return Damage;
	
	return Damage * GetSourceStats()->GetPrimaryAttribute(EffectAttribute);
}

void UCZEffect_Damage::OnEffectActivated()
{
	Super::OnEffectActivated();

	if (!IsValid(GetTarget()) || !IsValid(GetSource()))
		return;

	FHitResult hit;
	hit.Location = GetSource()->GetActorLocation();
	hit.ImpactPoint = hit.Location;
	hit.Normal = -GetSource()->GetActorForwardVector();
	hit.ImpactNormal = hit.Normal;

	AActor* causer = GetSource();
	
	if (Cast<ACZCard>(causer))
		causer = causer->GetOwner();
	
	UGameplayStatics::ApplyPointDamage(
		GetTarget(),
		static_cast<float>(GetScaledDamage()),
		GetSource()->GetActorForwardVector(),
		hit,
		nullptr,
		causer,
		UDamageType::StaticClass());
}
