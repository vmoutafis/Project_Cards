// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CZCard.generated.h"

UCLASS()
class PROJECT_CARDS_API ACZCard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACZCard();

	// get the transform of the card whether dragged or in hand
	FTransform GetCurrentTransform() const;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// toggle whether or not to use the offset transform
	void ToggleOffsetTransform(const bool Enable) { m_useOffsetTransform = Enable; }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// interp the actor to the correct location
	void HandleActorInterp();

	// transform of the card while dragging
	FTransform m_offsetTransform;

	// transform of the card in hand
	FTransform m_handTransform;

	// determine whether or not to use offset transform
	bool m_useOffsetTransform;
};
