// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CZEffectAsset.h"
#include "CZEffect_Damage.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_CARDS_API UCZEffect_Damage : public UCZEffectAsset
{
	GENERATED_BODY()
public:
	UCZEffect_Damage();
	
protected:
	virtual void OnEffectActivated() override;

	void ApplyDamage(const int Damage);

	virtual void OnDamageEmpowered(int& Damage, const FTurnEffect& Effect);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	TSubclassOf<UDamageType> DamageType;

	// this should be a turn based empowered effect that will empower the damage of this effect
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	TArray<TSubclassOf<UCZEffectAsset>> EmpowerEffects;
};
