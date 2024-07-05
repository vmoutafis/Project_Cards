// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CZCard.generated.h"

class UBoxComponent;
class UCZEffectAsset;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHighlightChanged, AActor*, card, int, handIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActivateComplete);

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
	void ToggleOffsetTransform(const bool enable);

	// is the card moving
	UFUNCTION(BlueprintPure, Category=Hand)
	bool IsCardMoving() const { return m_isMoving; }

	// is the card moving
	bool IsUsingOffsetTransform() const { return m_useOffsetTransform; }

	// toggle the highlighting of the card
	UFUNCTION(BlueprintCallable, Category=Card)
	void ToggleHighlight(const bool highlight);

	// updates when toggle highlight runs
	UPROPERTY(BlueprintAssignable, Category=Highlight)
	FOnHighlightChanged Delegate_OnHighlightChanged;

	UPROPERTY(BlueprintAssignable, Category=Highlight)
	FOnActivateComplete Delegate_OnActivateComplete;

	// set the index of the card based on the hand
	void SetHandIndex(int index) { m_handIndex = index; }

	// get the index of the card based on the hand
	UFUNCTION(BlueprintPure, Category=Hand)
	int GetHandIndex() const { return m_handIndex; }

	// update the hand position and index
	void SetHand(int handIndex, const FTransform&  initialTransform);
	
	// drag the card
	// returns false if failed
	UFUNCTION(BlueprintCallable, Category=Card)
	bool DragCard(const FVector location);

	UFUNCTION(BlueprintCallable, Category=Card)
	bool EndDragCard();

	UFUNCTION(BlueprintCallable, Category=Card)
	bool TryUseCard(AActor* actorHit, const FVector locationHit);

	// actor that has been hit by the card
	// null if card missed or hasn't been used
	UFUNCTION(BlueprintPure, Category=Card)
	AActor* GetHitActor() const { return m_hitActor; }

	// location the actor hit
	// 0 if card missed or hasn't been used
	UFUNCTION(BlueprintPure, Category=Card)
	FVector GetHitLocation() const { return m_hitLocation; }

	// transform the card using the actor card interp
	UFUNCTION(BlueprintCallable, Category=Card)
	void TransformCardActor(FTransform newTransform, float speed = 10.0f);
	
	// transform the card using the interp
	UFUNCTION(BlueprintCallable, Category=Card)
	void TransformCardOffset(FTransform newTransform, float speed = 25.0f);

	UFUNCTION(BlueprintCallable, Category=Activation)
	void CompleteActivation();

	UFUNCTION(BlueprintImplementableEvent, Category=Activation)
	void OnActivationComplete();

	UFUNCTION(BlueprintCallable, Category=Effects)
	void ApplyEffects();

	void ShrinkToReset();
	
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
	UFUNCTION(BlueprintCallable, Category=Card)
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Components)
	USceneComponent* SceneRoot;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Components)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Components)
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Card)
	TEnumAsByte<ECollisionChannel> EnemyDetection;

	UPROPERTY(EditDefaultsOnly, Category=Card)
	bool DebugHit;

private:
	// determine whether or not to use offset transform
	bool m_useOffsetTransform;

	// determine if the card is moving
	bool m_isMoving;

	// the starting mesh transform to return it to it's original position
	FTransform m_defaultMeshTransform;

	// index for the card in the hand
	int m_handIndex;

	// has the card been used
	bool m_cardUsed;

	// is the card active
	bool m_cardActive;
	
	UPROPERTY()
	// actor that has been hit by the card
	// null if card missed or hasn't been used
	AActor* m_hitActor;

	// location the actor hit
	// 0 if card missed or hasn't been used
	FVector m_hitLocation;

	// is the card dragging
	bool m_isDragging;

	// speed of the offset transform interp
	float m_offsetInterpSpeed;

	// speed of the hand transform interp
	float m_handInterpSpeed;

	// card set to reset when shrink complete
	bool m_shrinkToReset;
};
