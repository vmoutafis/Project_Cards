// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CZStatsComponent.generated.h"

#define NUM_ATTRIBUTES 5

UENUM(BlueprintType)
enum EPrimaryAttributes
{
	PA_Strength UMETA(DisplayName = "Strength"),
	PA_Intellect UMETA(DisplayName = "Intellect"),
	PA_Vitality UMETA(DisplayName = "Vitality"),
	PA_Stamina UMETA(DisplayName = "Stamina"),
	PA_Dexterity UMETA(DisplayName = "Dexterity")
};

USTRUCT(BlueprintType)
struct FPrimaryAttribute
{
	GENERATED_BODY()

	TEnumAsByte<EPrimaryAttributes> Attribute;

	int Value;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_CARDS_API UCZStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCZStatsComponent();

	// get a primary attribute
	UFUNCTION(BlueprintPure, Category=Attributes)
	int GetPrimaryAttribute(TEnumAsByte<EPrimaryAttributes> attribute);

	// set a primary attribute
	UFUNCTION(BlueprintCallable, Category=Attributes)
	void SetPrimaryAttribute(TEnumAsByte<EPrimaryAttributes> attribute, const int value);

protected:
	TArray<FPrimaryAttribute> DefaultAttributes;
	
private:
	// store stats array
	int PrimaryAttributes[NUM_ATTRIBUTES];
};
