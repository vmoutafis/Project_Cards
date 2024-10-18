// Fill out your copyright notice in the Description page of Project Settings.


#include "CZEffectAsset.h"

#include "CZEffectsComponent.h"
#include "CZGameInstance.h"

UCZEffectAsset::UCZEffectAsset()
{
	m_target = nullptr;
	m_source = nullptr;
	EffectName = FString("Effect Name");
	EffectAttribute = PA_Strength;
	bApplyEffectOnActivate = true;
	TurnActivationType = TA_Start;
	TurnEffectType = TUT_None;
	TurnEffectTarget = TT_Target;
	Icon = nullptr;
	EffectPower = DefaultPower = 1;
	EffectDescription = FString("This is an effect called ") + EffectName;
	IconColour = FLinearColor::Red;
}

void UCZEffectAsset::InitialiseEffect()
{
	EffectPower = DefaultPower;
}

void UCZEffectAsset::ActivateEffect(AActor* target, const FVector HitLocation, AActor* source)
{
	m_target = target;
	m_source = source;
	m_targetHitLocation = HitLocation;
	
	if (!IsValid(m_target) || !IsValid(m_source))
		return;
	
	if (bApplyEffectOnActivate)
	{
		OnEffectActivated();

		Delegate_OnActivated.Broadcast(this);
	}

	if (TurnEffectType == TUT_None)
	{
		EndEffect();
		return;
	}
	
	UActorComponent* comp = nullptr;
	
	switch (TurnEffectTarget)
	{
	case TT_Source :
		comp = source->GetComponentByClass(UCZEffectsComponent::StaticClass());
		break;
	case TT_Target :
		comp = target->GetComponentByClass(UCZEffectsComponent::StaticClass());
		break;
		default:
			break;
	}
	
	if (const auto effectComp = Cast<UCZEffectsComponent>(comp))
	{
		bool effectStacked = false;
		
		for (int i = effectComp->GetTurnEffects().Num() - 1; i >= 0; --i)
		{
			// skip if it's the wrong class
			if (effectComp->GetTurnEffects()[i].Effect->GetClass() != this->GetClass())
				continue;

			// apply stack effects
			effectStacked = true;

			if (TurnEffectType == TUT_PowerBased)
				effectComp->GetTurnEffects()[i].Effect->EmpowerEffect(effectComp->GetTurnEffectsAsRef()[i]);
			else
				effectComp->GetTurnEffects()[i].TurnsRemaining = 1;
			
			Delegate_OnUpdated.Broadcast({ effectComp->GetTurnEffects()[i].TurnsRemaining, this });

			break;
		}
		
		if (!effectStacked)
		{
			int EffectDuration = 1;
			EffectPower = DefaultPower;
			
			if (TurnEffectType == TUT_PowerBased)
				EffectDuration = EffectPower;
			
			effectComp->AddTurnEffect({EffectDuration, this});
		}
	}
}

void UCZEffectAsset::TurnStart()
{
	if (TurnActivationType == TA_Start || TurnActivationType == TA_Both)
		ReactivateEffect();

	OnTurnStart();
}

void UCZEffectAsset::TurnEnd()
{
	if (TurnActivationType == TA_End || TurnActivationType == TA_Both)
		ReactivateEffect();

	OnTurnEnd();
}

void UCZEffectAsset::ReactivateEffect()
{
	if (!IsValid(m_target) || !IsValid(m_source))
		return;
	
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
	FString FinalDes = EffectDescription;

	FinalDes = FinalDes.Replace(TEXT("<POWER>"), *FString::FromInt(GetScaledEffectPower()));
	FinalDes = FinalDes.Replace(TEXT("<NAME>"), *EffectName);

	if (FinalDes.Contains(TEXT("<ACTIVATION>")))
	{
		FString ActiveStr = "";
		
		switch (TurnActivationType)
		{
		case TA_None :
			ActiveStr = "None";
			break;
		case TA_Both :
			ActiveStr = "Both";
			break;
		case TA_Start :
			ActiveStr = "Start";
			break;
		case TA_End :
			ActiveStr = "End";
			break;
			default:
				break;
		}
		
		FinalDes = FinalDes.Replace(TEXT("<ACTIVATION>"), *ActiveStr);
	}

	if (FinalDes.Contains(TEXT("<TARGET>")))
	{
		FString TargetStr = "";
		
		switch (TurnEffectTarget)
		{
		case TT_Source :
			TargetStr = "Self";
			break;
		case TT_Target :
			TargetStr = "Enemy";
			break;
			default:
				break;
		}
		
		FinalDes = FinalDes.Replace(TEXT("<TARGET>"), *TargetStr);
	}
	
	return FinalDes;
}

void UCZEffectAsset::EmpowerEffect(FTurnEffect& TurnEffect)
{
	EffectPower++;
	
	if (TurnEffectType == TUT_PowerBased)
		TurnEffect.TurnsRemaining = EffectPower;
	
	OnEmpower(TurnEffect);

	Delegate_OnUpdated.Broadcast(TurnEffect);
}

void UCZEffectAsset::ReduceEffectPower(FTurnEffect& TurnEffect)
{
	EffectPower = FMath::Max(0, EffectPower - 1);
	
	if (TurnEffectType == TUT_PowerBased)
		TurnEffect.TurnsRemaining = EffectPower;

	OnPowerReduced(TurnEffect);
	
	Delegate_OnUpdated.Broadcast(TurnEffect);
}

int UCZEffectAsset::GetScaledEffectPower() const
{
	if (EffectAttribute == PA_None)
		return EffectPower;

	if (!IsValid(GetSourceStats()))
	{
		if (const auto GI = Cast<UCZGameInstance>(GetWorld()->GetGameInstance()))
		{
			return EffectPower * GI->GetPrimaryAttribute(EffectAttribute);	
		}

		return EffectPower;
	}
	
	return EffectPower * GetSourceStats()->GetPrimaryAttribute(EffectAttribute);
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
