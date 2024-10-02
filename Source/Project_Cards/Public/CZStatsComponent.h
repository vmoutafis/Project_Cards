// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CZStatsComponent.generated.h"

#define NUM_PRIM_ATTRIBS 5
#define NUM_SECOND_ATTRIBS 5

UENUM(BlueprintType)
enum EPrimaryAttributes : int
{
	PA_Strength = 0 UMETA(DisplayName = "Strength"),
	PA_Intellect UMETA(DisplayName = "Intellect"),
	PA_Vitality UMETA(DisplayName = "Vitality"),
	PA_Stamina UMETA(DisplayName = "Stamina"),
	PA_Dexterity UMETA(DisplayName = "Dexterity"),
	PA_None UMETA(DisplayName = "None")
};

UENUM(BlueprintType)
enum ESecondaryAttributes : int
{
	SA_Health = 0 UMETA(DisplayName = "Health"),
	SA_MaxHealth UMETA(DisplayName = "Max Health"),
	SA_Armour UMETA(DisplayName = "Armour"),
	SA_Energy UMETA(DisplayName = "Energy"),
	SA_MaxEnergy UMETA(DisplayName = "Max Energy")
};

USTRUCT(BlueprintType)
struct FPrimaryAttribute
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EPrimaryAttributes> Attribute = PA_Strength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Value = 1;
};

USTRUCT(BlueprintType)
struct FSecondaryAttribute
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ESecondaryAttributes> Attribute = SA_Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Value = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStatsInitialised);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPrimaryAttributeUpdated, TEnumAsByte<EPrimaryAttributes>, attribute, int, value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSecondaryAttributeUpdated, TEnumAsByte<ESecondaryAttributes>, attribute, int, value);

UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_CARDS_API UCZStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCZStatsComponent();

	// get a primary attribute
	UFUNCTION(BlueprintPure, Category=Attributes)
	int GetPrimaryAttribute(const TEnumAsByte<EPrimaryAttributes> attribute);

	// set a primary attribute
	UFUNCTION(BlueprintCallable, Category=Attributes)
	void SetPrimaryAttribute(const TEnumAsByte<EPrimaryAttributes> attribute, const int value, const bool increment = false);

	// after stats have been initialised
	UPROPERTY(BlueprintAssignable, Category=Initialisation)
	FStatsInitialised Delegate_OnStatsInitialised;

	// after a primary attrib has been updated
	UPROPERTY(BlueprintAssignable, Category=Attributes)
	FPrimaryAttributeUpdated Delegate_OnPrimaryAttributeUpdated;

	// after a secondary attrib has been updated
	UPROPERTY(BlueprintAssignable, Category=Attributes)
	FSecondaryAttributeUpdated Delegate_OnSecondaryAttributeUpdated;

	// set a secondary attribute
	// enable increment to add or remove from the current value
	UFUNCTION(BlueprintCallable, Category=Attributes)
	int SetSecondaryAttribute(const TEnumAsByte<ESecondaryAttributes> attribute, const int value,
		const bool increment = false, const int clampMax = -1);

	// get the value of the secondary attribute
	UFUNCTION(BlueprintPure, Category=Attributes)
	int GetSecondaryAttribute(const TEnumAsByte<ESecondaryAttributes> attribute) const;
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category=Initialisation)
	void OnStatsInitialised();

	UFUNCTION(BlueprintImplementableEvent, Category=Attributes)
	void OnPrimaryAttributeUpdated(const TEnumAsByte<EPrimaryAttributes>& attribute, const int& value);

	UFUNCTION(BlueprintImplementableEvent, Category=Attributes)
	void OnSecondaryAttributeUpdated(const TEnumAsByte<ESecondaryAttributes>& attribute, const int& value);
	
protected:
	// set default attribute values
	UPROPERTY(EditDefaultsOnly, Category=Attributes)
	TArray<FPrimaryAttribute> DefaultPrimaryAttributes;

	// set default attribute values
	UPROPERTY(EditDefaultsOnly, Category=Attributes)
	TArray<FSecondaryAttribute> DefaultSecondaryAttributes;
	
private:
	// store primary attributes array
	int PrimaryAttributes[NUM_PRIM_ATTRIBS];

	// store secondary attributes array
	int SecondaryAttributes[NUM_SECOND_ATTRIBS];
};
