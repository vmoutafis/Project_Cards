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
	
	virtual FString GetDescription() const override;

	int GetScaledDamage() const;
	
protected:
	virtual void OnEffectActivated() override;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Damage)
	int Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Damage)
	bool bAddPowerToDamage;
	
};
