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
	DebugHand = false;
	m_canInteract = false;
}

void ACZPlayerPawn::ClearHand()
{
	for (const auto& cardRef : m_hand)
	{
		cardRef->ShrinkToReset();
		DiscardCard(cardRef);
	}

	m_hand.Empty();
	HandChanged();
}

void ACZPlayerPawn::ShuffleDeck()
{
	TArray<ACZCard*> shuffleDeck;

	const size_t deckSize = m_deck.Num();
	
	for (size_t i = 0; i < deckSize; ++i)
	{
		const size_t random = FMath::RandRange(0, m_deck.Num() - 1);

		if (m_deck.IsValidIndex(random))
		{
			shuffleDeck.Add(m_deck[random]);
			m_deck.RemoveAt(random);
		}
	}

	m_deck = shuffleDeck;
	Delegate_OnDeckChanged.Broadcast();
}

void ACZPlayerPawn::DrawHand(bool initialDraw)
{
	// draw cards from deck over time
	DrawMultipleCards(DrawSize);
	
	m_firstDraw = false;
}

void ACZPlayerPawn::StartTurn()
{
	UE_LOG(LogTemp, Warning, TEXT("Start turn, first draw: %d"), m_firstDraw);
	DrawHand(m_firstDraw);
}

void ACZPlayerPawn::ReshuffleDeck()
{
	m_deck.Append(m_discardDeck);

	m_discardDeck.Empty();

	Delegate_OnDeckChanged.Broadcast();
}

void ACZPlayerPawn::HoverHand()
{
	if (!m_canInteract)
	{
		if (IsValid(m_hoveredCard))
        {
        	m_hoveredCard->ToggleHighlight(false);
        	m_hoveredCard = nullptr;
        }

		return;
	}
	
	if (m_shouldDragCard)
	{
		HandleCardDrag();
		return;
	}
	
	FHitResult hit;
	const auto pc = Cast<APlayerController>(GetController());
	
	if (!pc->GetHitResultUnderCursorByChannel(CardDetectChannel, false, hit))
	{		
		if (!pc->GetHitResultUnderFingerByChannel(ETouchIndex::Touch1, CardDetectChannel, false, hit))
		{
			if (IsValid(m_hoveredCard))
			{
				m_hoveredCard->ToggleHighlight(false);
				m_hoveredCard = nullptr;
			}
			
			return;
		}
	}

	if (DebugHand)
		DrawDebugBox(GetWorld(), hit.Location, FVector(5.0f), FColor::Green, false, 3.0f);
	
	if (const auto& cardRef = Cast<ACZCard>(hit.GetActor()))
	{
		if (IsValid(m_hoveredCard))
		{
			if (cardRef != m_hoveredCard)
				m_hoveredCard->ToggleHighlight(false);
		}

		m_hoveredCard = cardRef;
		m_hoveredCard->ToggleHighlight(true);	
	}
	else
	{
		if (IsValid(m_hoveredCard))
		{
			m_hoveredCard->ToggleHighlight(false);
			m_hoveredCard = nullptr;
		}
	}
}

void ACZPlayerPawn::TryDragCard()
{
	HoverHand();
	
	if (!IsValid(m_hoveredCard))
		return;

	m_shouldDragCard = true;
}

void ACZPlayerPawn::StopDraggingCard()
{
	m_shouldDragCard = false;

	if (IsValid(m_hoveredCard))
	{
		int cost = m_hoveredCard->GetCost();
		int handIndex = m_hoveredCard->GetHandIndex();
		
		if (m_hoveredCard->EndDragCard())
		{
			OnCardActivated(cost);
			DiscardFromHand(handIndex);
		}
	}

	HoverHand();
}

void ACZPlayerPawn::EnableInteract(bool enable)
{
	m_canInteract = enable;

	if (!m_canInteract)
		m_hoveredCard = nullptr;
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

		AddCardToDeck(cardRef);
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
	{
		UE_LOG(LogTemp, Warning, TEXT("no valid index in deck (%i), deck size: %i"), index, m_deck.Num())
		return nullptr;
	}

	ACZCard* cardRef = m_deck[index];
	m_deck.Remove(cardRef);

	Delegate_OnDeckChanged.Broadcast();

	return cardRef;
}

ACZCard* ACZPlayerPawn::DrawCardFromDeck(int deckIndex)
{
	if (GetHandSize() >= MaxHandSize)
		return nullptr;

	auto cardRef = RemoveCardFromDeck(deckIndex);
	
	if (!IsValid(cardRef))
		return nullptr;

	AddCardToHand(cardRef);
		
	return cardRef;
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
	SpaceCardsInHand();
}

void ACZPlayerPawn::SpaceCardsInHand()
{
	if (IsHandEmpty())
		return;

	const float halfSplineLength = HandSpline->GetSplineLength() / 2.0f;
	const float halfHandSize = FMath::Max(static_cast<float>(GetHandSize() - 1), 1.0f) / 2.0f;
	
	for (size_t i = 0; i < GetHandSize(); ++i)
	{
		float distanceOnSpline = halfSplineLength;

		if (GetHandSize() > 1)
		{
			float spacing = -(halfHandSize * HandSpacing);
			float handSpacing = HandSpacing;
			
			if (FMath::Abs(spacing) > halfSplineLength)
			{
				spacing = 0.0f;
				distanceOnSpline = 0.0f;
				handSpacing = HandSpline->GetSplineLength() / static_cast<float>(GetHandSize());
			}
			
			spacing += handSpacing * static_cast<float>(i);
			distanceOnSpline += spacing;
		}

		// update location
		FTransform newTransform = FTransform();
		newTransform.SetLocation(HandSpline->GetLocationAtDistanceAlongSpline(distanceOnSpline, ESplineCoordinateSpace::World));

		// update rotation
		FRotator newRotation = FRotator::ZeroRotator;
		newRotation.Roll = HandSpline->GetRotationAtDistanceAlongSpline(distanceOnSpline, ESplineCoordinateSpace::World).Pitch * -2.0f;
		if (GetHandSize() > 1)
		{
			newRotation.Yaw = 20.0f;
		}
		
		newTransform.SetRotation(newRotation.Quaternion());

		m_hand[i]->SetHand(i, newTransform);
	}
}

void ACZPlayerPawn::DrawMultipleCards(int num)
{
	UE_LOG(LogTemp, Warning, TEXT("Drawing multiple cards: %i"), num);
	m_cardsToDraw = num;
	TryDrawNextCard();
}

void ACZPlayerPawn::DrawNextCard()
{
	auto cardRef = DrawCardFromDeck(0);
	
	if (!IsValid(cardRef))
	{
		ReshuffleDeck();
		cardRef = DrawCardFromDeck(0);
	}

	if (!IsValid(cardRef))
		return;
	
	GetWorldTimerManager().SetTimer(TH_DrawTimer, this, &ACZPlayerPawn::TryDrawNextCard, 0.2f, false);
}

void ACZPlayerPawn::TryDrawNextCard()
{
	if (m_cardsToDraw <= 0)
		return;		

	m_cardsToDraw--;
	DrawNextCard();
}

void ACZPlayerPawn::HandleCardDrag()
{
	if (!IsValid(m_hoveredCard))
		return;

	FHitResult hit;

	const auto pc = Cast<APlayerController>(GetController());
	
	if (!pc->GetHitResultUnderCursorByChannel(FieldDetectChannel, false, hit))
	{
		if (!pc->GetHitResultUnderFingerByChannel(ETouchIndex::Touch1, FieldDetectChannel, false, hit))
		{
			return;
		}
	}

	m_hoveredCard->DragCard(hit.Location);
}

