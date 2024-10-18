// Fill out your copyright notice in the Description page of Project Settings.


#include "CZProjectile.h"

#include "CZEffectAsset.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACZProjectile::ACZProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger"));
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetRootComponent(Collision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement Component"));
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->InitialSpeed = 0.0f;
	
	bDestroyOnHit = true;
}

void ACZProjectile::LaunchProjectile(AActor* Source, AActor* Target, const FVector HitLocation, const TArray<UCZEffectAsset*>& effects, const float speed)
{
	if (!IsValid(Target) || !IsValid(Source))
	{
		Destroy();
		return;
	}
	
	SetOwner(Source);
	CardEffects = effects;
	TargetActor = Target;

	const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), HitLocation);
	
	SetActorRotation(LookAtRot);
	ProjectileMovement->SetVelocityInLocalSpace(LookAtRot.Vector() * speed);
}

// Called when the game starts or when spawned
void ACZProjectile::BeginPlay()
{
	Super::BeginPlay();

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ACZProjectile::OnCollisionOverlap);

	SetLifeSpan(5.0f);
}

void ACZProjectile::OnCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(TargetActor))
		return;
	
	if (OtherActor == TargetActor)
		TargetHit(GetActorLocation());
}

void ACZProjectile::ActivateEffects(AActor* Target)
{
	for (const auto& effect : CardEffects)
		effect->ActivateEffect(Target, GetActorLocation(), GetOwner());
}

void ACZProjectile::TargetHit(const FVector Location)
{
	ActivateEffects(TargetActor);

	OnTargetHit(Location);
	OnTargetHitEvent(Location);

	if (bDestroyOnHit)
		Destroy();
}

void ACZProjectile::LifeSpanExpired()
{
	Super::LifeSpanExpired();

	TargetHit(GetActorLocation());
}

