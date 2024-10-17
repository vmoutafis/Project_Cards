// Fill out your copyright notice in the Description page of Project Settings.


#include "CardEffects/CZEffect_Damage.h"

#include "CZCard.h"
#include "Kismet/GameplayStatics.h"

UCZEffect_Damage::UCZEffect_Damage()
{
	Damage = 1;
	bEmpowerValueOnApply = true;
	bAddPowerToDamage = true;
}

FString UCZEffect_Damage::GetDescription() const
{
	FString FinalDes = Super::GetDescription();
	
	return FinalDes = FinalDes.Replace(TEXT("<DAMAGE>"), *FString::FromInt(GetScaledDamage()));
}

int UCZEffect_Damage::GetScaledDamage() const
{
	int FinalDamage = Damage;
	
	if (bAddPowerToDamage)
		FinalDamage += CurrentEffectPower;
	
	if (!IsValid(GetSourceStats()) || EffectAttribute == PA_None)
		return FinalDamage;
	
	return FinalDamage * GetSourceStats()->GetPrimaryAttribute(EffectAttribute);
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
