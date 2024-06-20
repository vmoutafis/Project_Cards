// Fill out your copyright notice in the Description page of Project Settings.


#include "CZEffect_Damage.h"

#include "Kismet/GameplayStatics.h"

UCZEffect_Damage::UCZEffect_Damage()
{
	Damage = 1;
}

FString UCZEffect_Damage::GetDescription() const
{
	return FString("Deal ") + FString::FromInt(GetScaledDamage()) + FString(" Damage");
}

int UCZEffect_Damage::GetScaledDamage() const
{
	return Damage;
}

void UCZEffect_Damage::OnEffectActivated()
{
	Super::OnEffectActivated();

	if (!IsValid(GetTarget()) || !IsValid(GetSource()))
		return;

	FHitResult hit;
	hit.Location = GetSource()->GetActorLocation();
	hit.Normal = -GetSource()->GetActorForwardVector();
	
	UGameplayStatics::ApplyPointDamage(
		GetTarget(),
		static_cast<float>(GetScaledDamage()),
		GetSource()->GetActorForwardVector(),
		hit,
		nullptr,
		GetSource(),
		UDamageType::StaticClass());
}
