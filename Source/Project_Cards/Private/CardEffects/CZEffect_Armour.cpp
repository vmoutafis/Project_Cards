// Fill out your copyright notice in the Description page of Project Settings.


#include "CardEffects/CZEffect_Armour.h"

int UCZEffect_Armour::GetScaledArmour() const
{
	if (!IsValid(GetSourceStats()) || EffectAttribute == PA_None)
		return EffectPower;
	
	return EffectPower * GetSourceStats()->GetPrimaryAttribute(EffectAttribute);
}

void UCZEffect_Armour::OnEffectActivated()
{
	Super::OnEffectActivated();

	if (!IsValid(GetSource()))
		return;

	GetSourceStats()->SetSecondaryAttribute(SA_Armour, GetScaledArmour(), true);
}
