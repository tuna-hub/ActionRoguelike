// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ARogGameplayInterface.h"
#include "Components/TimelineComponent.h"
#include "ARogItemChest.generated.h"

class UStaticMeshComponent;
class UTimelineComponent;

UCLASS()
class ACTIONROGUELIKE_API AARogItemChest : public AActor, public IARogGameplayInterface
{
	GENERATED_BODY()

	void Interact_Implementation(AActor* InstigatorPawn);
	
public:	
	// Sets default values for this actor's properties
	AARogItemChest();

protected:

	// Meshes
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* LidMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* TreasureMesh;

	// Effects
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ParticleSysComp;

	// Lid status
	bool bIsLidOpen;

	// Timeline
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* TimelineComp;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* AnimationCurveFloat;

	UFUNCTION()
	void ToggleChestLid(float Output); // Update the lid location

	UFUNCTION()
	void OnEventFinish(); // called after the timeline all frames

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
