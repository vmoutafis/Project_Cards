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
	
	int GetScaledDamage() const;
	
protected:
	virtual void OnEffectActivated() override;

	virtual void ApplyDamage(const int Damage);

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	TSubclassOf<UDamageType> DamageType;
};
