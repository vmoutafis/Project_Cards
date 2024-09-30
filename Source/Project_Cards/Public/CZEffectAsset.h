// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CZStatsComponent.h"
#include "CZEffectAsset.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActivated, UCZEffectAsset*, effect);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnded, UCZEffectAsset*, effect);

/**
 * 
 */
UCLASS()
class PROJECT_CARDS_API UCZEffectAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UCZEffectAsset();
	
	void ActivateEffect(AActor* target, AActor* source);

	void EndEffect();
	
	UPROPERTY(BlueprintAssignable, Category=Effect)
	FOnActivated Delegate_OnActivated;

	UPROPERTY(BlueprintAssignable, Category=Effect)
	FOnEnded Delegate_OnEnded;

	UFUNCTION(BlueprintPure, Category=Effect)
	virtual FString GetDescription() const;
	
protected:
	virtual void OnEffectActivated() {}

	virtual void OnEffectEnded() {}

	AActor* GetTarget() const { return m_target; }

	AActor* GetSource() const { return m_source; }

	AActor* GetSourceOwner() const;

	UCZStatsComponent* GetSourceStats() const;
	
	UCZStatsComponent* GetTargetStats() const;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Effect)
	FString EffectName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Effect)
	TEnumAsByte<EPrimaryAttributes> EffectAttribute;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Effect)
	int EffectDuration;
	
private:
	UPROPERTY()
	AActor* m_target;

	UPROPERTY()
	AActor* m_source;
};
