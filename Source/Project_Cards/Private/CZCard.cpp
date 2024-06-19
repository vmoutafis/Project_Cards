// Fill out your copyright notice in the Description page of Project Settings.


#include "CZCard.h"

// Sets default values
ACZCard::ACZCard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

FTransform ACZCard::GetCurrentTransform() const
{
	if (m_useOffsetTransform)
		return m_offsetTransform;

	return m_handTransform;
}

// Called when the game starts or when spawned
void ACZCard::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACZCard::HandleActorInterp()
{
	
}

// Called every frame
void ACZCard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

