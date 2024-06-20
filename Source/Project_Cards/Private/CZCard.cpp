// Fill out your copyright notice in the Description page of Project Settings.


#include "CZCard.h"

#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACZCard::ACZCard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Card Root"));
	SetRootComponent(SceneRoot);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Card Mesh"));
	Mesh->SetupAttachment(SceneRoot);

	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Hit Detection"));
	HitBox->SetupAttachment(SceneRoot);

	// default the box at the mesh bounds
	HitBox->SetBoxExtent(Mesh->Bounds.BoxExtent);
	
	m_isMoving = false;
	ToggleOffsetTransform(false);
	m_handIndex = -1;
	m_isCardActive = false;
	CardName = "Card Name";
}

FTransform ACZCard::GetCurrentTransform() const
{
	if (m_useOffsetTransform)
		return m_offsetTransform;

	return m_handTransform;
}

void ACZCard::ToggleHighlight(const bool highlight)
{
	if (!m_isCardActive)
		return;
	
	if (highlight)
	{
		const FVector newPosition = FVector(-50.0f, 0.0f, 50.0f);
		m_meshTransform.SetLocation(newPosition);
	}
	else
		m_meshTransform = m_defaultMeshTransform;

	// broadcast the change
	Delegate_OnHighlightChanged.Broadcast(this, m_handIndex);
}

void ACZCard::DrawCard(int handIndex, FTransform initialTransform)
{
	AdjustHand(handIndex, std::move(initialTransform));
	
	if (m_isCardActive)
		return;
	
	m_isDrawing = true;
	
	Delegate_OnDrawn.Broadcast(this);
}

void ACZCard::DiscardCard()
{
	ToggleHighlight(false);
	ToggleOffsetTransform(false);
	m_meshTransform = m_defaultMeshTransform;

	m_handTransform.SetLocation(m_handTransform.GetLocation() + FVector(0.0f, 0.0f, 100.0f));
	m_handTransform.SetScale3D(FVector(0.0f));
	m_isDiscarding = true;
	m_isCardActive = false;
	
	Delegate_OnDiscard.Broadcast(this);
}

void ACZCard::AdjustHand(int handIndex, FTransform initialTransform)
{
	SetHandIndex(handIndex);
	m_handTransform = std::move(initialTransform);
}

bool ACZCard::DragCard(const FVector location)
{
	if (!GetCardActive())
		return false;

	if (!IsCardBeingDragged())
	{
		ToggleHighlight(false);
		ToggleOffsetTransform(true);
	}

	m_offsetTransform.SetLocation(location);
	
	return true;
}

bool ACZCard::TryUseCard(AActor* actorHit, const FVector locationHit)
{
	if (!GetCardActive())
		return false;

	m_hitActor = actorHit;
	m_hitLocation = locationHit;

	OnCardActivated();
	
	return true;
}

// Called when the game starts or when spawned
void ACZCard::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(Mesh))
		m_defaultMeshTransform = Mesh->GetRelativeTransform();
}

void ACZCard::TryStartMoving()
{
	if (m_isMoving)
		return;
	
	m_isMoving = true;
	OnStartMoving();
}

void ACZCard::TryStopMoving()
{
	if (!m_isMoving)
		return;

	if (m_isDrawing)
	{
		m_isDrawing = false;
		Delegate_OnDrawComplete.Broadcast(this);
	}

	if (m_isDiscarding)
	{
		m_isDiscarding = false;
		ResetCard();
	}
	
	m_isMoving = false;
	OnStopMoving();
}

void ACZCard::ResetCard()
{
	m_isCardActive = false;
	
	// reset the hand transform to offscreen
	m_handTransform.SetLocation(FVector(0.0f, 0.0f, -1000.0f));
	m_handTransform.SetRotation(FRotator(0.0f).Quaternion());
	m_handTransform.SetScale3D(FVector(1.0f));

	// reset the offset transform
	m_offsetTransform.SetLocation(FVector(0.0f));
	m_offsetTransform.SetRotation(FRotator(0.0f).Quaternion());
	m_offsetTransform.SetScale3D(FVector(1.0f));

	// reset the mesh transforms
	m_meshTransform = m_defaultMeshTransform;
	
	SetActorTransform(m_handTransform);
	Mesh->SetRelativeTransform(m_meshTransform);
}

void ACZCard::HandleActorInterp()
{
	if (GetActorTransform().Equals(GetCurrentTransform(), 0.01f))
		return TryStopMoving();

	TryStartMoving();

	float speed = 10.0f;
	if (m_useOffsetTransform)
		speed = 25.0f;
	
	const FTransform newTransform = UKismetMathLibrary::TInterpTo(
		GetActorTransform(), GetCurrentTransform(), GetWorld()->GetDeltaSeconds(), speed);
	
	SetActorTransform(newTransform);
}

void ACZCard::HandleMeshInterp() const
{
	if (!IsValid(Mesh))
		return;

	if (Mesh->GetRelativeTransform().Equals(m_meshTransform))
		return;

	const FTransform newTransform = UKismetMathLibrary::TInterpTo(
		Mesh->GetRelativeTransform(), m_meshTransform, GetWorld()->GetDeltaSeconds(), 10.0f);

	Mesh->SetRelativeTransform(newTransform);
}

// Called every frame
void ACZCard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleActorInterp();
	HandleMeshInterp();
}

