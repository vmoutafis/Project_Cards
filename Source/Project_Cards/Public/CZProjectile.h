// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CZProjectile.generated.h"

class UCZEffectAsset;
class UProjectileMovementComponent;
class UCapsuleComponent;

UCLASS()
class PROJECT_CARDS_API ACZProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACZProjectile();	

	UFUNCTION(BlueprintCallable, Category=Projectile)
	void LaunchProjectile(AActor* Source, AActor* Target, const FVector HitLocation, const TArray<UCZEffectAsset*>& effects, const float speed = 1000.0f);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void ActivateEffects(AActor* Target);

	void TargetHit(const FVector Location);
	
	virtual void OnTargetHit(const FVector Location) {}

	UFUNCTION(BlueprintImplementableEvent)
	void OnTargetHitEvent(const FVector Location);

	virtual void LifeSpanExpired() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Components)
	UCapsuleComponent* Collision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Components)
	UProjectileMovementComponent* ProjectileMovement;

protected:
	// effects will be referenced from the card on activate
	TArray<UCZEffectAsset*> CardEffects;

	// actor that will be working with overlap
	UPROPERTY()
	AActor* TargetActor;

	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	bool bDestroyOnHit;
};
