// Fill out your copyright notice in the Description page of Project Settings.


#include "CardEffects/CZEffect_Damage.h"
#include "CZCard.h"
#include "Kismet/GameplayStatics.h"

UCZEffect_Damage::UCZEffect_Damage()
{
	DamageType = UDamageType::StaticClass();
}

int UCZEffect_Damage::GetScaledDamage() const
{
	if (!IsValid(GetSourceStats()) || EffectAttribute == PA_None)
		return EffectPower;
	
	return EffectPower * GetSourceStats()->GetPrimaryAttribute(EffectAttribute);
}

void UCZEffect_Damage::OnEffectActivated()
{
	Super::OnEffectActivated();

	if (!IsValid(GetTarget()) || !IsValid(GetSource()))
		return;

	ApplyDamage(GetScaledDamage());
}

void UCZEffect_Damage::ApplyDamage(const int Damage)
{
	FHitResult hit;
	hit.Location = GetHitLocation();

	if (TurnEffectType == TUT_None)
		hit.Location = GetTarget()->GetActorLocation();
	
	hit.ImpactPoint = hit.Location;
	hit.Normal = -GetSource()->GetActorForwardVector();
	hit.ImpactNormal = hit.Normal;

	AActor* causer = GetSource();
	
	if (Cast<ACZCard>(causer))
		causer = causer->GetOwner();
	
	UGameplayStatics::ApplyPointDamage(
		GetTarget(),
		static_cast<float>(Damage),
		GetSource()->GetActorForwardVector(),
		hit,
		nullptr,
		causer,
		DamageType);
}
