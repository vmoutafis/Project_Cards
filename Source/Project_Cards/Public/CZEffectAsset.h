// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CZStatsComponent.h"
#include "CZEffectAsset.generated.h"

struct FTurnEffect;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActivated, UCZEffectAsset*, effect);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnded, UCZEffectAsset*, effect);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdated, UCZEffectAsset*, effect);

UENUM(BlueprintType)
enum ETurnActivations : uint8
{
	TA_Start = 0 UMETA(DisplayName="Start"),
	TA_End UMETA(DisplayName="End"),
	TA_Both UMETA(DisplayName="Start and End"),
	TA_None UMETA(DisplayName="None")
};

UENUM(BlueprintType)
enum ETargetTypes : uint8
{
	TT_Target = 0 UMETA(DisplayName="Target"),
	TT_Source UMETA(DisplayName="Source")
};

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

	void TurnStart();

	void TurnEnd();
	
	void EndEffect();
	
	UPROPERTY(BlueprintAssignable, Category=Effect)
	FOnActivated Delegate_OnActivated;

	UPROPERTY(BlueprintAssignable, Category=Effect)
	FOnEnded Delegate_OnEnded;
	
	UPROPERTY(BlueprintAssignable, Category=Effect)
	FOnUpdated Delegate_OnUpdated;

	UFUNCTION(BlueprintPure, Category=Effect)
	virtual FString GetDescription() const;

	void EmpowerEffect(const FTurnEffect& TurnEffect);
	
protected:
	virtual void OnEffectActivated() {}

	// activates the effect a second time if activate effect has already run
	void ReactivateEffect();
	
	virtual void OnEffectEnded() {}

	virtual void OnTurnStart() {}

	virtual void OnTurnEnd() {}

	AActor* GetTarget() const { return m_target; }

	AActor* GetSource() const { return m_source; }

	AActor* GetSourceOwner() const;

	UCZStatsComponent* GetSourceStats() const;
	
	UCZStatsComponent* GetTargetStats() const;

	virtual void OnEmpower(const FTurnEffect& TurnEffect) {}

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Effect)
	FString EffectName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Effect)
	TEnumAsByte<EPrimaryAttributes> EffectAttribute;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Effect)
	FString EffectDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Effect)
	UTexture2D* Icon;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Effect)
	bool bApplyEffectOnActivate;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Turn)
	int EffectDuration;

	// default power to reset too
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Turn)
	int EffectPower;

	// actual current power that adjusts as empowered
	UPROPERTY(BlueprintReadOnly, Category=Turn)
	int CurrentEffectPower;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Turn)
	TEnumAsByte<ETurnActivations> TurnActivationType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Turn)
	TEnumAsByte<ETargetTypes> TurnEffectTarget;

	// increase the power of the effect when reapplied
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Turn|Stack Type")
	bool bEmpowerValueOnApply;

	// increase the duration of the effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Turn|Stack Type")
	bool bStackDurationOnApply;

	// reset the duration of the effect
	// this will override stack duration
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Turn|Stack Type")
	bool bResetDurationOnApply;
	
private:
	UPROPERTY()
	AActor* m_target;

	UPROPERTY()
	AActor* m_source;
};
