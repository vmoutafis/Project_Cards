// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CZStatsComponent.h"
#include "CZGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_CARDS_API UCZGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	friend class UCZStatsComponent;

	UCZGameInstance();

	// get a primary attribute
	UFUNCTION(BlueprintPure, Category=Attributes)
	int GetPrimaryAttribute(const TEnumAsByte<EPrimaryAttributes> attribute);

	// set a primary attribute
	UFUNCTION(BlueprintCallable, Category=Attributes)
	void SetPrimaryAttribute(const TEnumAsByte<EPrimaryAttributes> attribute, const int value, const bool increment = false);
	
	// Reset all primary attributes
	UFUNCTION(BlueprintCallable, Category=Attributes)
	void ResetPrimaryAttributes();

	UFUNCTION(BlueprintPure, Category=Attributes)
	int GetTotalAssignedAttributePoints();
	
	// after a primary attrib has been updated
	UPROPERTY(BlueprintAssignable, Category=Attributes)
	FPrimaryAttributeUpdated Delegate_OnPrimaryAttributeUpdated;
	
protected:
	UFUNCTION(BlueprintImplementableEvent, Category=Attributes)
	void OnPrimaryAttributeUpdated(const TEnumAsByte<EPrimaryAttributes>& attribute, const int& value);
	
protected:
	int PrimaryAttributes[NUM_PRIM_ATTRIBS];

};
