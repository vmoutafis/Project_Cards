// Fill out your copyright notice in the Description page of Project Settings.


#include "CZStatsComponent.h"

// Sets default values for this component's properties
UCZStatsComponent::UCZStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	for (int32 i = 0; i < NUM_PRIM_ATTRIBS; ++i)
		PrimaryAttributes[i] = 1;
	
	// ...
}

int UCZStatsComponent::GetPrimaryAttribute(TEnumAsByte<EPrimaryAttributes> attribute)
{
	if (attribute >= NUM_PRIM_ATTRIBS)
		return -1;

	return PrimaryAttributes[attribute.GetIntValue()];
}

void UCZStatsComponent::SetPrimaryAttribute(TEnumAsByte<EPrimaryAttributes> attribute, const int value, const bool increment)
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

int UCZStatsComponent::SetSecondaryAttribute(const TEnumAsByte<ESecondaryAttributes> attribute, const int value,
	const bool increment, const int clampMax)
{
	if (attribute >= NUM_SECOND_ATTRIBS)
		return -1;

	const uint8 index = attribute.GetIntValue();
	
	if (!increment)
		SecondaryAttributes[index] = value;
	else
		SecondaryAttributes[index] += value;

	if (SecondaryAttributes[index] < 0)
		SecondaryAttributes[index] = 0;

	if (clampMax > -1 && SecondaryAttributes[index] > clampMax)
		SecondaryAttributes[index] = clampMax;
	
	OnSecondaryAttributeUpdated(attribute, SecondaryAttributes[index]);
	Delegate_OnSecondaryAttributeUpdated.Broadcast(attribute, SecondaryAttributes[index]);

	return SecondaryAttributes[index];
}

int UCZStatsComponent::GetSecondaryAttribute(const TEnumAsByte<ESecondaryAttributes> attribute) const
{
	if (attribute >= NUM_SECOND_ATTRIBS)
		return -1;

	return SecondaryAttributes[attribute.GetIntValue()];
}

void UCZStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	for (const auto item : DefaultPrimaryAttributes)
		SetPrimaryAttribute(item.Attribute, item.Value);

	for (const auto item : DefaultSecondaryAttributes)
		SetSecondaryAttribute(item.Attribute, item.Value);

	OnStatsInitialised();
	Delegate_OnStatsInitialised.Broadcast();
}
