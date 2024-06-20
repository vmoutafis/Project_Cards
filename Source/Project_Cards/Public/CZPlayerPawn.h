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

UCLASS()
class PROJECT_CARDS_API ACZPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACZPlayerPawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable, Category=Deck)
	FOnDeckChanged Delegate_OnDeckChanged;

	// shuffle the cards in the deck
	UFUNCTION(BlueprintCallable, Category=Deck)
	void ShuffleDeck();

	// draw the players hand
	UFUNCTION(BlueprintCallable, Category=Deck)
	void DrawHand(bool initialDraw = false);

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

	// run at the start of the battle
	UFUNCTION(BlueprintCallable, Category=Battle)
	void StartBattle(TArray<TSubclassOf<ACZCard>> deck);

	// add discard back into deck and shuffle
	void ReshuffleDeck();
	
protected:
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

	// discard a card from the hand
	void DiscardFromHand(int handIndex);

	// remove a card from the hand
	ACZCard* RemoveCardFromHand(int index);

	// run when the hnd has changed
	void HandChanged();

	// adjust the transform of cards in the hand
	void SpaceCardsInHand();

	// draw multiple cards from the deck
	void DrawMultipleCards(int num);

private:
	void DrawNextCard();

	UFUNCTION()
	void TryDrawNextCard(ACZCard* previousCard = nullptr);
	
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Hand)
	int MaxHandSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Hand)
	int DrawSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Hand)
	float HandSpacing;
	
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
};
