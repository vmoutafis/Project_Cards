// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CZCard.generated.h"

class UBoxComponent;
class UCZEffectAsset;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHighlightChanged, AActor*, card, int, handIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDrawn, ACZCard*, card);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDrawComplete, ACZCard*, card);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDiscard, ACZCard*, card);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDiscardComplete, ACZCard*, card);

UCLASS()
class PROJECT_CARDS_API ACZCard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACZCard();

	// get the transform of the card whether dragged or in hand
	UFUNCTION(BlueprintPure, Category=Card)
	FTransform GetCurrentTransform() const;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// toggle whether or not to use the offset transform
	void ToggleOffsetTransform(const bool enable) { m_useOffsetTransform = enable; }

	// is the card moving
	UFUNCTION(BlueprintPure, Category=Hand)
	bool IsCardMoving() const { return m_isMoving; }

	// is the card moving
	bool IsUsingOffsetTransform() const { return m_useOffsetTransform; }

	// is the card begin dragged
	UFUNCTION(BlueprintPure, Category=Card)
	bool IsCardBeingDragged() const { return IsUsingOffsetTransform(); }

	// toggle the highlighting of the card
	UFUNCTION(BlueprintCallable, Category=Card)
	void ToggleHighlight(const bool highlight);

	// updates when toggle highlight runs
	UPROPERTY(BlueprintAssignable, Category=Highlight)
	FOnHighlightChanged Delegate_OnHighlightChanged;

	// updates when card has been drawn
	UPROPERTY(BlueprintAssignable, Category=Draw)
	FOnDrawn Delegate_OnDrawn;

	// updates when card draw has completed
	UPROPERTY(BlueprintAssignable, Category=Draw)
	FOnDrawComplete Delegate_OnDrawComplete;

	// updates when card has been discarded
	UPROPERTY(BlueprintAssignable, Category=Draw)
	FOnDiscard Delegate_OnDiscard;

	// updates when card discard has completed
	UPROPERTY(BlueprintAssignable, Category=Draw)
	FOnDiscard Delegate_OnDiscardComplete;

	// set the index of the card based on the hand
	void SetHandIndex(int index) { m_handIndex = index; }

	// get the index of the card based on the hand
	UFUNCTION(BlueprintPure, Category=Hand)
	int GetHandIndex() const { return m_handIndex; }

	// is the card ready for interaction
	UFUNCTION(BlueprintPure, Category=Card)
	bool GetCardActive() const { return m_isCardActive; }

	// draw the card
	UFUNCTION(BlueprintCallable, Category=Draw)
	void DrawCard(int handIndex, const FTransform& initialTransform);

	// discard the card from the hand
	UFUNCTION(BlueprintCallable, Category=Draw)
	void DiscardCard();

	// update the hand position and index
	void AdjustHand(int handIndex, const FTransform&  initialTransform);

	// is the card drawing
	UFUNCTION(BlueprintPure, Category=Draw)
	bool IsDrawing() const { return m_isDrawing; }

	// is the card discarding
	UFUNCTION(BlueprintPure, Category=Draw)
	bool IsDiscarding() const { return m_isDiscarding; }

	// drag the card
	// returns false if failed
	UFUNCTION(BlueprintCallable, Category=Card)
	bool DragCard(const FVector location);

	UFUNCTION(BlueprintCallable, Category=Card)
	bool TryUseCard(AActor* actorHit, const FVector locationHit);

	// actor that has been hit by the card
	// null if card missed or hasn't been used
	AActor* GetHitActor() const { return m_hitActor; }

	// location the actor hit
	// 0 if card missed or hasn't been used
	FVector GetHitLocation() const { return m_hitLocation; }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// when the card starts moving
	UFUNCTION(BlueprintImplementableEvent, Category=Card)
	void OnStartMoving();

	// when the card stops moving
	UFUNCTION(BlueprintImplementableEvent, Category=Card)
	void OnStopMoving();

	// out of hand card settings
	void ResetCard();

	// card was successfully activated
	UFUNCTION(BlueprintImplementableEvent, Category=Card)
	void OnCardActivated();
	
private:
	// interp the actor to the correct location
	void HandleActorInterp();

	// interp the mesh relative to the actor
	void HandleMeshInterp() const;

	// when the card starts moving
	void TryStartMoving();

	// when the card stops moving
	void TryStopMoving();

public:
	UPROPERTY(EditDefaultsOnly, Category=Components)
	USceneComponent* SceneRoot;
	
	UPROPERTY(EditDefaultsOnly, Category=Components)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, Category=Components)
	UBoxComponent* HitBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Card)
	FString CardName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Card)
	TArray<UCZEffectAsset*> CardEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Card)
	int Cost;
	
	// transform of the card while dragging for interp
	FTransform m_offsetTransform;

	// transform of the card in hand for interp
	FTransform m_handTransform;

	// transform of the relative mesh for interp
	FTransform m_meshTransform;
	
private:
	// determine whether or not to use offset transform
	bool m_useOffsetTransform;

	// determine if the card is moving
	bool m_isMoving;

	// the starting mesh transform to return it to it's original position
	FTransform m_defaultMeshTransform;

	// index for the card in the hand
	int m_handIndex;

	// is the card ready for interaction
	bool m_isCardActive;

	// is the card being drawn
	bool m_isDrawing;

	// is the card discarding
	bool m_isDiscarding;

	// has the card been used
	bool m_CardUsed;

	UPROPERTY()
	// actor that has been hit by the card
	// null if card missed or hasn't been used
	AActor* m_hitActor;

	// location the actor hit
	// 0 if card missed or hasn't been used
	FVector m_hitLocation;
};
