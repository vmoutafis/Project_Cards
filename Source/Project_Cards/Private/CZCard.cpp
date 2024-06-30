// Fill out your copyright notice in the Description page of Project Settings.


#include "CZCard.h"

#include "CZEffectAsset.h"
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
	m_useOffsetTransform = false;
	m_handIndex = -1;
	CardName = "Card Name";
	Cost = 5;
	m_cardActive = false;
	m_cardUsed = false;
	DebugHit = false;
	m_handInterpSpeed = 10.0f;
	m_offsetInterpSpeed = 25.0f;
	m_shrinkToReset = false;
}

FTransform ACZCard::GetCurrentTransform() const
{
	if (m_useOffsetTransform)
		return m_offsetTransform;

	return m_handTransform;
}

void ACZCard::ToggleHighlight(const bool highlight)
{
	if (m_isDragging)
		return;
	
	if (highlight && !m_cardUsed)
	{
		const FVector newPosition = FVector(0.0f, 0.0f, 50.0f);
		m_meshTransform.SetLocation(newPosition);
	}
	else
		m_meshTransform = m_defaultMeshTransform;

	// broadcast the change
	Delegate_OnHighlightChanged.Broadcast(this, m_handIndex);
}

void ACZCard::SetHand(int handIndex, const FTransform& initialTransform)
{
	m_useOffsetTransform = false;
	
	SetHandIndex(handIndex);
	TransformCardActor(initialTransform);
	m_meshTransform = m_defaultMeshTransform;
	SetActorHiddenInGame(false);
	m_cardActive = true;
}

bool ACZCard::DragCard(const FVector location)
{
	if (m_cardUsed)
		return false;

	ToggleHighlight(false);
	
	FTransform dragTransform = FTransform();
	dragTransform.SetLocation(location);

	TransformCardOffset(dragTransform);

	m_isDragging = true;

	return true;
}

bool ACZCard::EndDragCard()
{
	if (m_cardUsed)
		return false;

	m_isDragging = false;
	
	FHitResult hit;
	const FVector endTrace = GetActorLocation() + GetActorForwardVector() * 10000.0f;
	const FCollisionShape shape = FCollisionShape::MakeBox(HitBox->GetScaledBoxExtent() * 0.75f);
	
	if (GetWorld()->SweepSingleByChannel(hit, GetActorLocation(), endTrace, GetActorRotation().Quaternion(), EnemyDetection, shape))
	{
		if (DebugHit)
			DrawDebugBox(GetWorld(), hit.ImpactPoint, shape.GetBox(), FColor::Green, false, 3.0f);

		if (hit.GetActor() != GetOwner())
			if (TryUseCard(hit.GetActor(), hit.ImpactPoint))
				return true;
	}

	ToggleOffsetTransform(false);

	return false;
}

bool ACZCard::TryUseCard(AActor* actorHit, const FVector locationHit)
{
	if (m_cardUsed)
		return false;

	m_hitActor = actorHit;
	m_hitLocation = locationHit;
	m_cardUsed = true;

	OnCardActivated();
	
	return true;
}

void ACZCard::TransformCardActor(FTransform newTransform, float speed)
{
	m_handInterpSpeed = speed;
	ToggleOffsetTransform(false);
	m_handTransform = newTransform;
}

void ACZCard::TransformCardOffset(FTransform newTransform, float speed)
{
	m_offsetInterpSpeed = speed;
	ToggleOffsetTransform(true);
	m_offsetTransform = newTransform;
}

void ACZCard::CompleteActivation()
{
	OnActivationComplete();
	Delegate_OnActivateComplete.Broadcast();
	ResetCard();
}

void ACZCard::ApplyEffects()
{
	for (const auto& effect : CardEffects)
		effect->ActivateEffect(GetHitActor(), this);
}

void ACZCard::ShrinkToReset()
{
	ToggleOffsetTransform(false);
	m_cardUsed = true;
	m_meshTransform = m_defaultMeshTransform;
	m_handTransform.SetScale3D(FVector(0.0f));
	m_shrinkToReset = true;
}

// Called when the game starts or when spawned
void ACZCard::BeginPlay()
{
	Super::BeginPlay();

	m_defaultMeshTransform = Mesh->GetRelativeTransform(); 
	
	ResetCard();
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
	
	m_isMoving = false;
	OnStopMoving();

	if (m_shrinkToReset)
		ResetCard();
}

void ACZCard::ResetCard()
{
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

	SetActorHiddenInGame(true);
	
	SetActorTransform(m_handTransform);
	Mesh->SetRelativeTransform(m_meshTransform);
	m_cardActive = false;
	m_cardUsed = false;
	m_shrinkToReset = false;
}

void ACZCard::HandleActorInterp()
{
	if (GetActorTransform().Equals(GetCurrentTransform(), 0.01f))
		return TryStopMoving();

	TryStartMoving();

	float speed = m_handInterpSpeed;
	if (m_useOffsetTransform)
		speed = m_offsetInterpSpeed;
	
	const FTransform newTransform = UKismetMathLibrary::TInterpTo(
		GetActorTransform(), GetCurrentTransform(), GetWorld()->GetDeltaSeconds(), speed);
	
	SetActorTransform(newTransform);
}

void ACZCard::HandleMeshInterp() const
{
	if (!IsValid(Mesh))
		return;

	if (Mesh->GetRelativeTransform().Equals(m_meshTransform, 0.01f))
		return;

	const FTransform newTransform = UKismetMathLibrary::TInterpTo(
		Mesh->GetRelativeTransform(), m_meshTransform, GetWorld()->GetDeltaSeconds(), 10.0f);

	Mesh->SetRelativeTransform(newTransform);
}

// Called every frame
void ACZCard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!m_cardActive)
		return;
	
	HandleActorInterp();
	HandleMeshInterp();
}

void ACZCard::ToggleOffsetTransform(const bool enable)
{
	m_offsetTransform = FTransform();
	m_useOffsetTransform = enable;
}

