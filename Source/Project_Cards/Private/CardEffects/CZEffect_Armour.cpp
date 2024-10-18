// Fill out your copyright notice in the Description page of Project Settings.


#include "CardEffects/CZEffect_Armour.h"

void UCZEffect_Armour::OnEffectActivated()
{
	Super::OnEffectActivated();

	if (!IsValid(GetSource()))
		return;

	GetSourceStats()->SetSecondaryAttribute(SA_Armour, GetScaledEffectPower(), true);
}
