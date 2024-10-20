// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CZPlayerPawn.generated.h"

class ACZCard;
class UBoxComponent;
class USplineComponent;
class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeckChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDrawComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDrawStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNotEnoughEnergy);

UCLASS()
class PROJECT_CARDS_API ACZPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACZPlayerPawn();

	UPROPERTY(BlueprintAssignable, Category=Deck)
	FOnDeckChanged Delegate_OnDeckChanged;

	UPROPERTY(BlueprintAssignable, Category=Hand)
	FOnDrawComplete Delegate_OnDrawComplete;

	UPROPERTY(BlueprintAssignable, Category=Hand)
	FOnDrawStart Delegate_OnDrawStart;

	UPROPERTY(BlueprintAssignable, Category=Hand)
	FNotEnoughEnergy Delegate_NotEnoughEnergy;

	// check if the deck is empty
	UFUNCTION(BlueprintPure, Category=Deck)
	bool IsDeckEmpty() const { return m_deck.IsEmpty(); }

	// get the amount of cards left in the deck
	UFUNCTION(BlueprintPure, Category=Deck)
	int GetDeckSize() const { return m_deck.Num(); }

	// check if the hand is empty
	UFUNCTION(BlueprintPure, Category=Hand)
	bool IsHandEmpty() const { return m_hand.IsEmpty(); }

	// get the amount of cards left in the hand
	UFUNCTION(BlueprintPure, Category=Hand)
	int GetHandSize() const { return m_hand.Num(); }

	// check if the hand is empty
	UFUNCTION(BlueprintPure, Category=Deck)
	bool IsDiscardEmpty() const { return m_discardDeck.IsEmpty(); }

	// get the amount of cards left in the hand
	UFUNCTION(BlueprintPure, Category=Deck)
	int GetDiscardSize() const { return m_discardDeck.Num(); }

	// add discard back into deck and shuffle
	void ReshuffleDeck();

	// on mouse move test cards underneath
	UFUNCTION(BlueprintCallable, Category=Hand)
	void HoverHand();

	UFUNCTION(BlueprintCallable, Category=Hand)
	void TryDragCard();

	UFUNCTION(BlueprintCallable, Category=Hand)
	void StopDraggingCard();

	UFUNCTION(BlueprintCallable, Category=Hand)
	void EnableInteract(bool enable);

	UFUNCTION(BlueprintCallable, Category=Hand)
	void ClearHand();

	UFUNCTION(BlueprintImplementableEvent, Category=Hand)
	void OnCardActivated(int Cost);

	// get the cards in hand
	UFUNCTION(BlueprintPure, Category=Cards)
	TArray<ACZCard*> GetHand() const { return m_hand; }

	// get the cards in deck
	UFUNCTION(BlueprintPure, Category=Cards)
	TArray<ACZCard*> GetDeck() const { return m_deck; }

	// get the cards in the discard deck
	UFUNCTION(BlueprintPure, Category=Cards)
	TArray<ACZCard*> GetDiscardDeck() const { return m_discardDeck; }

	// discard a card from the hand
	UFUNCTION(BlueprintCallable, Category=Card)
	void DiscardFromHand(int handIndex);
	
protected:
	// shuffle the cards in the deck
	UFUNCTION(BlueprintCallable, Category=Deck)
	void ShuffleDeck();

	// draw the players hand
	UFUNCTION(BlueprintCallable, Category=Deck)
	void DrawHand(bool initialDraw = false);
	
	// run at the start of the battle
	UFUNCTION(BlueprintCallable, Category=Battle)
	void StartTurn();
	
	// create a deck out of a class array
	UFUNCTION(BlueprintCallable, Category=Deck)
	void CreateDeck(TArray<TSubclassOf<ACZCard>> deck);

	// add a card to the deck
	void AddCardToDeck(ACZCard* card);

	// remove a card from the deck
	ACZCard* RemoveCardFromDeck(int index);
	
	// add a card from the deck to the hand
	ACZCard* DrawCardFromDeck(int deckIndex);

	// add a card to the hand
	int AddCardToHand(ACZCard* card);

	// add a card to the discard pile
	void DiscardCard(ACZCard* card);
	
	// remove a card from the hand
	ACZCard* RemoveCardFromHand(int index);

	// run when the hnd has changed
	void HandChanged();

	// adjust the transform of cards in the hand
	void SpaceCardsInHand();

	// draw multiple cards from the deck
	void DrawMultipleCards(int num);

	UFUNCTION(BlueprintImplementableEvent, Category=Hand)
	void OnDrawStarted();

	UFUNCTION(BlueprintImplementableEvent, Category=Hand)
	void OnDrawComplete();

	virtual void Tick(float DeltaSeconds) override;

private:
	void DrawNextCard();

	UFUNCTION()
	void TryDrawNextCard();

	void HandleCardDrag();
	
public:
	UPROPERTY(EditDefaultsOnly, Category=Components)
	USceneComponent* SceneRoot;
	
	UPROPERTY(EditDefaultsOnly, Category=Components)
	UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, Category=Components)
	USceneComponent* HandRoot;

	UPROPERTY(EditDefaultsOnly, Category=Components)
	USplineComponent* HandSpline;

	UPROPERTY(EditDefaultsOnly, Category=Components)
	UBoxComponent* CardBlocker;

	UPROPERTY(EditDefaultsOnly, Category=Components)
	UBoxComponent* CardField;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Hand)
	int MaxHandSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Hand)
	int DrawSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Hand)
	float HandSpacing;

	UPROPERTY(EditDefaultsOnly, Category=Hand)
	TEnumAsByte<ETraceTypeQuery> CardDetectChannel;

	UPROPERTY(EditDefaultsOnly, Category=Hand)
	TEnumAsByte<ETraceTypeQuery> FieldDetectChannel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Hand)
	bool DebugHand;
	
private:
	// deck of cards
	TArray<ACZCard*> m_deck;

	// cards in hand
	TArray<ACZCard*> m_hand;

	// cards in discard pile
	TArray<ACZCard*> m_discardDeck;

	// is this the first draw
	bool m_firstDraw;

	// is the player drawing cards
	bool m_drawingCards;

	// cards to draw when multi
	int m_cardsToDraw;

	// timer to draw the next card
	FTimerHandle TH_DrawTimer;

	// card that is currently hovered
	UPROPERTY()
	ACZCard* m_hoveredCard;

	// if hovered card is set should drag card will drag the hovered card
	bool m_shouldDragCard;

	// if the player can interact with cards
	bool m_canInteract;
};
