// Fill out your copyright notice in the Description page of Project Settings.


#include "CZPlayerPawn.h"

#include "CZCard.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"

// Sets default values
ACZPlayerPawn::ACZPlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Player Root"));
	SetRootComponent(SceneRoot);
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Camera"));
	Camera->SetupAttachment(SceneRoot);

	HandRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Player Hand Transform"));
	HandRoot->SetupAttachment(SceneRoot);

	HandSpline = CreateDefaultSubobject<USplineComponent>(TEXT("Card Hand Spline"));
	HandSpline->SetupAttachment(HandRoot);

	CardBlocker = CreateDefaultSubobject<UBoxComponent>(TEXT("Ignore Zone"));
	CardBlocker->SetupAttachment(HandRoot);

	CardField = CreateDefaultSubobject<UBoxComponent>(TEXT("Grab Zone"));
	CardField->SetupAttachment(HandRoot);

	MaxHandSize = 10;
	m_firstDraw = true;
	HandSpacing = 50.0f;
	DrawSize = 5;
}

void ACZPlayerPawn::ShuffleDeck()
{
	TArray<ACZCard*> shuffleDeck;

	const size_t deckSize = m_deck.Num();
	for (size_t i = 0; i < deckSize; ++i)
	{
		const size_t random = FMath::RandRange(0, m_deck.Num() - 1);

		if (m_deck.IsValidIndex(random))
			shuffleDeck.Add(m_deck[random]);
	}

	m_deck = shuffleDeck;
	Delegate_OnDeckChanged.Broadcast();
}

void ACZPlayerPawn::DrawHand(bool initialDraw)
{
	if (IsDeckEmpty() && IsHandEmpty())
		ReshuffleDeck();

	// draw cards from deck over time
	DrawMultipleCards(DrawSize);
	
	m_firstDraw = false;
}

void ACZPlayerPawn::StartBattle(TArray<TSubclassOf<ACZCard>> deck)
{
	CreateDeck(deck);
	DrawHand(true);
}

void ACZPlayerPawn::ReshuffleDeck()
{
	m_deck.Append(m_discardDeck);

	m_discardDeck.Empty();

	Delegate_OnDeckChanged.Broadcast();
}

void ACZPlayerPawn::CreateDeck(TArray<TSubclassOf<ACZCard>> deck)
{
	for (const auto& classRef : deck)
	{
		FVector location(HandRoot->GetComponentLocation() + FVector(500.0f, 0.0f, -1000.0f));
		FActorSpawnParameters params;
		params.Owner = this;
		ACZCard* cardRef = GetWorld()->SpawnActor<ACZCard>(
			classRef,
			location,
			FRotator(0.0f),
			params
		);
		
		cardRef->SetActorHiddenInGame(true);
	}

	ShuffleDeck();
}

void ACZPlayerPawn::AddCardToDeck(ACZCard* card)
{
	m_deck.Add(card);

	Delegate_OnDeckChanged.Broadcast();
}

ACZCard* ACZPlayerPawn::RemoveCardFromDeck(int index)
{
	if (!m_deck.IsValidIndex(index))
		return nullptr;

	ACZCard* cardRef = m_deck[index];

	m_deck.RemoveAt(index);

	Delegate_OnDeckChanged.Broadcast();

	return cardRef;
}

ACZCard* ACZPlayerPawn::DrawCardFromDeck(int deckIndex)
{
	if (GetHandSize() >= MaxHandSize)
		return nullptr;

	if (const auto& cardRef = RemoveCardFromDeck(deckIndex))
	{
		const int cardIndex = AddCardToHand(cardRef);
		SpaceCardsInHand();
		
		return m_deck[cardIndex];
	}

	return nullptr;
}

int ACZPlayerPawn::AddCardToHand(ACZCard* card)
{
	const size_t index = m_hand.Add(card);

	HandChanged();

	return static_cast<int>(index);
}

void ACZPlayerPawn::DiscardCard(ACZCard* card)
{
	m_discardDeck.Add(card);
	
	Delegate_OnDeckChanged.Broadcast();
}

void ACZPlayerPawn::DiscardFromHand(int handIndex)
{
	if (const auto& cardRef = RemoveCardFromHand(handIndex))
		DiscardCard(cardRef);
}

ACZCard* ACZPlayerPawn::RemoveCardFromHand(int index)
{
	if (!m_hand.IsValidIndex(index))
		return nullptr;

	ACZCard* cardRef = m_hand[index];

	m_hand.RemoveAt(index);

	HandChanged();
	
	return cardRef;
}

void ACZPlayerPawn::HandChanged()
{
	
}

void ACZPlayerPawn::SpaceCardsInHand()
{
	if (IsHandEmpty())
		return;

	const float halfSplineLength = HandSpline->GetSplineLength() / 2.0f;
	const float halfHandSize = static_cast<float>(GetHandSize()) / 2.0f;
	uint64 index = 0;
	
	for (const auto& cardRef : m_hand)
	{
		float distanceOnSpline = halfSplineLength;

		if (GetHandSize() > 1)
		{
			float spacing = -(halfHandSize * HandSpacing);
			float handSpacing = HandSpacing;
			
			if (FMath::Abs(spacing) > halfSplineLength)
			{
				handSpacing = HandSpline->GetSplineLength() / static_cast<float>(GetHandSize());
			}
			
			spacing += handSpacing * index;
			distanceOnSpline += spacing;
		}

		// update location
		FTransform newTransform;
		newTransform.SetLocation(HandSpline->GetLocationAtDistanceAlongSpline(distanceOnSpline, ESplineCoordinateSpace::World));

		// update rotation
		FRotator newRotation;
		newRotation.Roll = HandSpline->GetRotationAtDistanceAlongSpline(distanceOnSpline, ESplineCoordinateSpace::World).Pitch * -2.0f;
		if (GetHandSize() > 1)
		{
			newRotation.Yaw = 20.0f;
		}
		
		newTransform.SetRotation(newRotation.Quaternion());
		
		cardRef->DrawCard(index, newTransform);
		
		++index;
	}
}

void ACZPlayerPawn::DrawMultipleCards(int num)
{
	m_cardsToDraw = num;
	TryDrawNextCard();
}

void ACZPlayerPawn::DrawNextCard()
{
	if (const auto& cardRef = DrawCardFromDeck(0))
	{
		cardRef->Delegate_OnDrawComplete.AddDynamic(this, &ACZPlayerPawn::TryDrawNextCard);
		return;
	}

	ReshuffleDeck();
	
	if (const auto& cardRef = DrawCardFromDeck(0))
		cardRef->Delegate_OnDrawComplete.AddDynamic(this, &ACZPlayerPawn::TryDrawNextCard);
}

void ACZPlayerPawn::TryDrawNextCard(ACZCard* previousCard)
{
	if (IsValid(previousCard))
	{
		m_cardsToDraw--;
		previousCard->Delegate_OnDrawComplete.RemoveDynamic(this, &ACZPlayerPawn::TryDrawNextCard);
	}

	if (m_cardsToDraw > 0)
		DrawNextCard();
}

// Called every frame
void ACZPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

