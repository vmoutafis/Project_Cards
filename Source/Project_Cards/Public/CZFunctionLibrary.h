// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CZFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_CARDS_API UCZFunctionLibrary : public UObject
{
	GENERATED_BODY()
	
public:
	// tests if the object is of class or equivilent child classes
	UFUNCTION(BlueprintPure, Category=Objects)
	static bool IsObjectOfClass(UObject* object, TSubclassOf<UObject> parentClass);
};
