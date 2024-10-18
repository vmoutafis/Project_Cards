// Fill out your copyright notice in the Description page of Project Settings.


#include "CZGameInstance.h"

UCZGameInstance::UCZGameInstance()
{
	for (int i = 0; i < NUM_PRIM_ATTRIBS; ++i)
		PrimaryAttributes[i] = 1;
}

int UCZGameInstance::GetPrimaryAttribute(const TEnumAsByte<EPrimaryAttributes> attribute)
{
	if (attribute >= NUM_PRIM_ATTRIBS)
		return -1;

	return PrimaryAttributes[attribute.GetIntValue()];
}

void UCZGameInstance::SetPrimaryAttribute(const TEnumAsByte<EPrimaryAttributes> attribute, const int value,
	const bool increment)
{
	if (attribute >= NUM_PRIM_ATTRIBS)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attribute does not exist, cannot set value."))
		return;
	}

	if (!increment)
		PrimaryAttributes[attribute.GetIntValue()] = value;
	else
		PrimaryAttributes[attribute.GetIntValue()] += value;
	
	OnPrimaryAttributeUpdated(attribute, PrimaryAttributes[attribute.GetIntValue()]);
	Delegate_OnPrimaryAttributeUpdated.Broadcast(attribute, PrimaryAttributes[attribute.GetIntValue()]);
}

void UCZGameInstance::ResetPrimaryAttributes()
{
	SetPrimaryAttribute(PA_Strength, 1);
	SetPrimaryAttribute(PA_Intellect, 1);
	SetPrimaryAttribute(PA_Vitality, 1);
	SetPrimaryAttribute(PA_Dexterity, 1);
	SetPrimaryAttribute(PA_Stamina, 1);
}

int UCZGameInstance::GetTotalAssignedAttributePoints()
{
	int total = 0;

	for (int i = 0; i < NUM_PRIM_ATTRIBS; ++i)
		total += PrimaryAttributes[i] - 1;
	
	return total;
}
