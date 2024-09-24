// Fill out your copyright notice in the Description page of Project Settings.


#include "CZEffectAsset.h"

UCZEffectAsset::UCZEffectAsset()
{
	m_target = nullptr;
	m_source = nullptr;
	EffectName = "Effect Name";
	EffectAttribute = PA_Strength;
}

void UCZEffectAsset::ActivateEffect(AActor* target, AActor* source)
{
	m_target = target;
	m_source = source;
	
	OnEffectActivated();

	Delegate_OnActivated.Broadcast(this);
}

void UCZEffectAsset::EndEffect()
{
	OnEffectEnded();

	Delegate_OnEnded.Broadcast(this);
}

FString UCZEffectAsset::GetDescription() const
{
	return FString("This is an effect called: ") + EffectName;
}

UCZStatsComponent* UCZEffectAsset::GetSourceStats() const
{
	if (!IsValid(GetSource()))
		return {};

	if (const auto stats = GetSource()->GetComponentByClass<UCZStatsComponent>())
		return stats;
	
	return {};
}

UCZStatsComponent* UCZEffectAsset::GetTargetStats() const
{
	if (!IsValid(GetTarget()))
		return {};

	if (const auto stats = GetTarget()->GetComponentByClass<UCZStatsComponent>())
		return stats;
	
	return {};
}
