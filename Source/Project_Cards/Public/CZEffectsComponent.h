// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CZEffectsComponent.generated.h"

class UCZEffectAsset;

USTRUCT(BlueprintType)
struct FTurnEffect
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TurnsRemaining = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCZEffectAsset* Effect = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEffectsChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEffectActivated, FTurnEffect, effectData);

UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_CARDS_API UCZEffectsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCZEffectsComponent();

	// get the current turn effects
	UFUNCTION(BlueprintPure, Category=Effects)
	TArray<FTurnEffect> GetTurnEffects() const { return TurnEffects; }

	// get the current turn effects as reference
	// this allows to to change the turn effects
	UFUNCTION(BlueprintPure, Category=Effects)
	TArray<FTurnEffect>& GetTurnEffectsAsRef() { return TurnEffects; }

	// add an effect to turns
	UFUNCTION(BlueprintCallable, Category=Effects)
	void AddTurnEffect(const FTurnEffect Effect);

	// remove a turn effect
	// use the index that can be obtained from GetTurnEffects()
	UFUNCTION(BlueprintCallable, Category=Effects)
	void RemoveTurnEffect(const int index);
	
	// run when the turn starts
	// all effects that have a turn start will activate their effects
	UFUNCTION(BlueprintCallable, Category=Turns)
	void TurnStart();

	// run when the turn ends
	// all effects that have a turn end will activate their effects
	UFUNCTION(BlueprintCallable, Category=Turns)
	void TurnEnd();

	UPROPERTY(BlueprintAssignable, Category=Effects)
	FOnEffectsChanged Delegate_OnEffectsChanged;
	
	UPROPERTY(BlueprintAssignable, Category=Effects)
	FOnEffectActivated Delegate_OnEffectActivated;

private:
	UFUNCTION()
	void OnEffectActivated(UCZEffectAsset* effect);
	
private:
	// store all current turn effects
	TArray<FTurnEffect> TurnEffects;
		
};
