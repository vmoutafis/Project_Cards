// Fill out your copyright notice in the Description page of Project Settings.


#include "CZStatsComponent.h"

// Sets default values for this component's properties
UCZStatsComponent::UCZStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	for (int32 i = 0; i < NUM_ATTRIBUTES; ++i)
		PrimaryAttributes[i] = 1;
	
	// ...
}

int UCZStatsComponent::GetPrimaryAttribute(TEnumAsByte<EPrimaryAttributes> attribute)
{
	if (attribute < NUM_ATTRIBUTES)
		return PrimaryAttributes[attribute];

	return -1;
}

void UCZStatsComponent::SetPrimaryAttribute(TEnumAsByte<EPrimaryAttributes> attribute, const int value)
{
	if (attribute >= NUM_ATTRIBUTES)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attribute does not exist, cannot set value."))
		return;
	}

	PrimaryAttributes[attribute] = value;
}
