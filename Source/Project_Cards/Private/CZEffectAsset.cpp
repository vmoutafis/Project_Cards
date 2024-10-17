// Fill out your copyright notice in the Description page of Project Settings.


#include "CZEffectAsset.h"

#include "CZEffectsComponent.h"

UCZEffectAsset::UCZEffectAsset()
{
	m_target = nullptr;
	m_source = nullptr;
	EffectName = FString("Effect Name");
	EffectAttribute = PA_Strength;
	bApplyEffectOnActivate = true;
	TurnActivationType = TA_Start;
	bEmpowerValueOnApply = true;
	bStackDurationOnApply = false;
	bResetDurationOnApply = false;
	EffectPower = 1;
	CurrentEffectPower = 1;
	EffectDescription = FString("This is an effect called ") + EffectName;
}

void UCZEffectAsset::ActivateEffect(AActor* target, AActor* source)
{
	m_target = target;
	m_source = source;

	if (!IsValid(m_target) || !IsValid(m_source))
		return;
	
	if (bApplyEffectOnActivate)
	{
		OnEffectActivated();

		Delegate_OnActivated.Broadcast(this);
	}

	if (EffectDuration <= 0)
		EndEffect();
	else
	{
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
			
			for (int i = effectComp->GetTurnEffectsAsRef().Num() - 1; i >= 0; --i)
			{
				// skip if it's the wrong class
				if (effectComp->GetTurnEffects()[i].Effect->GetClass() != this->GetClass())
					continue;
				
				if (!bEmpowerValueOnApply && !bResetDurationOnApply && !bStackDurationOnApply)
				{
					effectComp->RemoveTurnEffect(i, false);
					continue;
				}

				effectStacked = true;
				
				if (bEmpowerValueOnApply)
					EmpowerEffect(effectComp->GetTurnEffectsAsRef()[i]);

				if (bResetDurationOnApply)
					effectComp->GetTurnEffectsAsRef()[i].TurnsRemaining = EffectDuration;

				if (bStackDurationOnApply)
					effectComp->GetTurnEffectsAsRef()[i].TurnsRemaining += EffectDuration;

				Delegate_OnUpdated.Broadcast(this);
			}

			if (!effectStacked)
			{
				CurrentEffectPower = EffectPower;
				effectComp->AddTurnEffect({EffectDuration, this});
			}
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

	FinalDes = FinalDes.Replace(TEXT("<TURNS>"), *FString::FromInt(EffectDuration));
	FinalDes = FinalDes.Replace(TEXT("<POWER>"), *FString::FromInt(EffectPower));
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

void UCZEffectAsset::EmpowerEffect(const FTurnEffect& TurnEffect)
{
	CurrentEffectPower++;
	
	OnEmpower(TurnEffect);

	Delegate_OnUpdated.Broadcast(this);
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
