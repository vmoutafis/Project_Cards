// Fill out your copyright notice in the Description page of Project Settings.


#include "CardEffects/CZEffect_Armour.h"

UCZEffect_Armour::UCZEffect_Armour()
{
	Armour = 1;
	EffectAttribute = PA_None;
	bEmpowerValueOnApply = true;
}

int UCZEffect_Armour::GetScaledArmour() const
{
	if (!IsValid(GetSourceStats()) || EffectAttribute == PA_None)
		return Armour + CurrentEffectPower;
	
	return (Armour + CurrentEffectPower) * GetSourceStats()->GetPrimaryAttribute(EffectAttribute);
}

FString UCZEffect_Armour::GetDescription() const
{
	FString FinalDes = Super::GetDescription();
	
	return FinalDes = FinalDes.Replace(TEXT("<ARMOUR>"), *FString::FromInt(GetScaledArmour()));
}

void UCZEffect_Armour::OnEffectActivated()
{
	Super::OnEffectActivated();

	if (!IsValid(GetSource()))
		return;

	GetSourceStats()->SetSecondaryAttribute(SA_Armour, GetScaledArmour(), true);
}
