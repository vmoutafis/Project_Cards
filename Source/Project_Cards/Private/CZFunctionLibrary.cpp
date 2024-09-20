// Fill out your copyright notice in the Description page of Project Settings.


#include "CZFunctionLibrary.h"

bool UCZFunctionLibrary::IsObjectOfClass(UObject* object, TSubclassOf<UObject> parentClass)
{
	if (!IsValid(object) || !IsValid(parentClass))
		return false;

	return object->GetClass() == *parentClass || object->GetClass()->IsChildOf(*parentClass);
}
