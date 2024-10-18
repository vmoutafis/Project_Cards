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
	
protected:
	virtual void OnEffectActivated() override;
	
};
