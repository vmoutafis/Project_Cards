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

AActor* UCZEffectAsset::GetSourceOwner() const
{
	if (!IsValid(GetSource()))
		return {};
	
	return GetSource()->GetOwner();
}

UCZStatsComponent* UCZEffectAsset::GetSourceStats() const
{
	if (!IsValid(GetSource()))
		return {};

	if (!IsValid(GetSourceOwner()))
		return {};
	
	TArray<UActorComponent*> components;
	GetSource()->GetOwner()->GetComponents(UCZStatsComponent::StaticClass(), components);

	if (components.IsEmpty())
		return {};
	
	if (const auto stats = Cast<UCZStatsComponent>(components[0]))
		return stats;
	
	return {};
}

UCZStatsComponent* UCZEffectAsset::GetTargetStats() const
{
	if (!IsValid(GetTarget()))
		return {};

	TArray<UActorComponent*> components;
	GetTarget()->GetComponents(UCZStatsComponent::StaticClass(), components);

	if (components.IsEmpty())
		return {};
	
	if (const auto stats = Cast<UCZStatsComponent>(components[0]))
		return stats;
	
	return {};
}
