// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CZEffectsComponent.generated.h"

class UCZEffectAsset;

USTRUCT(BlueprintType)
struct FMyStruct
{
	GENERATED_BODY()

	int TurnsRemaining;

	UPROPERTY()
	UCZEffectAsset* Effect;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_CARDS_API UCZEffectsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCZEffectsComponent();

	
		
};
