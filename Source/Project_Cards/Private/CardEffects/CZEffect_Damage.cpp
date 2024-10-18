// Fill out your copyright notice in the Description page of Project Settings.


#include "CardEffects/CZEffect_Damage.h"
#include "CZCard.h"
#include "Kismet/GameplayStatics.h"

UCZEffect_Damage::UCZEffect_Damage()
{
	DamageType = UDamageType::StaticClass();
}

void UCZEffect_Damage::OnEffectActivated()
{
	Super::OnEffectActivated();

	if (!IsValid(GetTarget()) || !IsValid(GetSource()))
		return;

	ApplyDamage(GetScaledEffectPower());
}

void UCZEffect_Damage::ApplyDamage(const int Damage)
{
	int FinalDamage = Damage;
	
	if (!EmpowerEffects.IsEmpty())
	{
		const auto& effectComp = Cast<UCZEffectsComponent>(GetTarget()->GetComponentByClass(UCZEffectsComponent::StaticClass()));

		if (IsValid(effectComp))
		{
			for (const auto& effect : effectComp->GetTurnEffects())
			{
				if (EmpowerEffects.Contains(effect.Effect->GetClass()))
					OnDamageEmpowered(FinalDamage, effect);
			}
		}
	}
	
	FHitResult hit;
	hit.Location = GetHitLocation();

	if (TurnEffectType == TUT_None)
		hit.Location = GetTarget()->GetActorLocation();
	
	hit.ImpactPoint = hit.Location;
	hit.Normal = -GetSource()->GetActorForwardVector();
	hit.ImpactNormal = hit.Normal;

	AActor* causer = GetSource();
	
	if (Cast<ACZCard>(causer))
		causer = causer->GetOwner();
	
	UGameplayStatics::ApplyPointDamage(
		GetTarget(),
		static_cast<float>(FinalDamage),
		GetSource()->GetActorForwardVector(),
		hit,
		nullptr,
		causer,
		DamageType);
}

void UCZEffect_Damage::OnDamageEmpowered(int& Damage, const FTurnEffect& Effect)
{
	Damage += Effect.TurnsRemaining;
}
