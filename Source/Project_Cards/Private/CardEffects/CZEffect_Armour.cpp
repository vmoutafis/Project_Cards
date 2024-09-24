// Fill out your copyright notice in the Description page of Project Settings.


#include "CardEffects/CZEffect_Armour.h"

UCZEffect_Armour::UCZEffect_Armour()
{
	Armour = 1;
}

FString UCZEffect_Armour::GetDescription() const
{
	return FString("Gain ") + FString::FromInt(Armour) + FString(" Armour.");
}

void UCZEffect_Armour::OnEffectActivated()
{
	Super::OnEffectActivated();

	if (!IsValid(GetSource()))
		return;

	
}
