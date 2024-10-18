// Fill out your copyright notice in the Description page of Project Settings.


#include "CZStatsComponent.h"

#include "CZGameInstance.h"

// Sets default values for this component's properties
UCZStatsComponent::UCZStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	for (int32 i = 0; i < NUM_PRIM_ATTRIBS; ++i)
		PrimaryAttributes[i] = 1;
		
	for (int32 i = 0; i < NUM_SECOND_ATTRIBS; ++i)
		SecondaryAttributes[i] = 0;

	bInitGameInstancePrimaryAttributes = false;
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

	// increment or set
	if (!increment)
		SecondaryAttributes[index] = value;
	else
		SecondaryAttributes[index] += value;

	// clamp maximum id needed
	if (clampMax > -1)
		SecondaryAttributes[index] = FMath::Min(clampMax, SecondaryAttributes[index]);

	// clamp minimum at 0 as no stats should go below anyway
	SecondaryAttributes[index] = FMath::Max(0, SecondaryAttributes[index]);
	
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

	if (!bInitGameInstancePrimaryAttributes)
		for (int i = 0; i < DefaultPrimaryAttributes.Num(); ++i)
			PrimaryAttributes[DefaultPrimaryAttributes[i].Attribute] = DefaultPrimaryAttributes[i].Value;
	else
	{
		if (const auto GI = Cast<UCZGameInstance>(GetWorld()->GetGameInstance()))
		{
			for (int i = 0; i < NUM_PRIM_ATTRIBS; ++i)
				PrimaryAttributes[i] = GI->PrimaryAttributes[i];
		}
	}

	int extraHealth = 0;
	int extraEnergy = 0;
	
	for (const auto item : DefaultSecondaryAttributes)
	{
		switch (item.Attribute)
		{
		case SA_MaxHealth :
			extraHealth = (GetPrimaryAttribute(PA_Vitality) - 1) * 5;
			SetSecondaryAttribute(item.Attribute, item.Value + extraHealth);
			continue;
		case SA_Health :
			SetSecondaryAttribute(item.Attribute, item.Value + extraHealth);
			continue;
		case SA_MaxEnergy :
			extraEnergy = FMath::FloorToInt(static_cast<float>(GetPrimaryAttribute(PA_Stamina)) / 5.0f);
			SetSecondaryAttribute(item.Attribute, item.Value + extraEnergy);
			continue;
		case SA_Energy :
			SetSecondaryAttribute(item.Attribute, item.Value + extraEnergy);
			continue;
			default:
				break;
		}
		
		SetSecondaryAttribute(item.Attribute, item.Value);
	}

	OnStatsInitialised();
	Delegate_OnStatsInitialised.Broadcast();
}
