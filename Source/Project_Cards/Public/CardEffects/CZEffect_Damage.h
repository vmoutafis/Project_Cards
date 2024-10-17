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
	int GetScaledDamage() const;
	
protected:
	virtual void OnEffectActivated() override;
	
};
