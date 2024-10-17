// Fill out your copyright notice in the Description page of Project Settings.


#include "CZEffectsComponent.h"

#include "CZEffectAsset.h"

// Sets default values for this component's properties
UCZEffectsComponent::UCZEffectsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UCZEffectsComponent::AddTurnEffect(const FTurnEffect Effect)
{
	TurnEffects.Add(Effect);

	Effect.Effect->Delegate_OnActivated.AddUniqueDynamic(this, &UCZEffectsComponent::OnEffectActivated);
	
	Delegate_OnEffectsChanged.Broadcast();
}

void UCZEffectsComponent::RemoveTurnEffect(const int index)
{
	if (!TurnEffects.IsValidIndex(index))
		return;

	TurnEffects[index].Effect->EndEffect();

	TurnEffects[index].Effect->Delegate_OnActivated.RemoveAll(this);
	TurnEffects.RemoveAt(index);

	Delegate_OnEffectsChanged.Broadcast();
}

void UCZEffectsComponent::TurnStart()
{
	for (size_t i = 0; i < TurnEffects.Num(); ++i)
	{
		TurnEffects[i].Effect->TurnStart();
		TurnEffects[i].Effect->ReduceEffectPower(TurnEffects[i]);
	}

	for (int i = TurnEffects.Num() - 1; i >= 0; --i)
	{
		if (TurnEffects[i].TurnsRemaining <= 0)
			RemoveTurnEffect(i);
	}

	Delegate_OnEffectsChanged.Broadcast();
}

void UCZEffectsComponent::TurnEnd()
{
	for (const auto effect : TurnEffects)
		effect.Effect->TurnEnd();

	Delegate_OnEffectsChanged.Broadcast();
}

void UCZEffectsComponent::OnEffectActivated(UCZEffectAsset* effect)
{
	for (const auto turnEffect : TurnEffects)
	{
		if (effect == turnEffect.Effect)
		{
			Delegate_OnEffectActivated.Broadcast(turnEffect);
			return;
		}
	}
}

