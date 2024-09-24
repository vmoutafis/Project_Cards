// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CZEffectAsset.h"
#include "CZEffect_Armour.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_CARDS_API UCZEffect_Armour : public UCZEffectAsset
{
	GENERATED_BODY()

public:
	UCZEffect_Armour();
	
	virtual FString GetDescription() const override;

protected:
	virtual void OnEffectActivated() override;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Effect)
	int Armour;
	
};
