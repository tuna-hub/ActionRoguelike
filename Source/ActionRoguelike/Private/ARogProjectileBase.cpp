// Fill out your copyright notice in the Description page of Project Settings.


#include "ARogProjectileBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Sound/SoundCue.h"
#include "Blueprint/ARogGameplayFunctionLibrary.h"
#include "ARogTargetDummy.h"


// Sets default values
AARogProjectileBase::AARogProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	//SphereComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	//SphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	//SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	//SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	//SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	SphereComp->SetCollisionProfileName("Projectile");
	SetRootComponent(SphereComp);

	ParticleSysComp = CreateDefaultSubobject<UParticleSystemComponent>("ParticleSysComp");
	ParticleSysComp->SetupAttachment(SphereComp);

	Projectile = CreateDefaultSubobject<UProjectileMovementComponent>("Projectile");
	Projectile->InitialSpeed = 2000.0f;
	Projectile->bRotationFollowsVelocity = true;
	Projectile->bInitialVelocityInLocalSpace = true;
	Projectile->ProjectileGravityScale = 0.0f;

	AudioComp = CreateDefaultSubobject<UAudioComponent>("AudioComp");
	AudioComp->SetupAttachment(SphereComp);
	//AudioComp->bAutoActivate = true;

	// Camera Shake
	ImpactShakeInnerRadius = 0.0f;
	ImpactShakeOuterRadius = 1500.0f;

	// Allow replicate when spawned during gameplay
	bReplicates = true;
	//SetReplicates(true);
}

// Called when the game starts or when spawned
void AARogProjectileBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AARogProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AARogProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	// Hit event
	SphereComp->OnComponentHit.AddDynamic(this, &AARogProjectileBase::OnComponentHit);
	//Ignores the intigator that spawned the projectile.
	SphereComp->IgnoreActorWhenMoving(GetInstigator(), true);
}

void AARogProjectileBase::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor)
	{
		if (OtherActor != GetInstigator())
		{
			if (Cast<AARogTargetDummy>(OtherActor))
			{
				UARogGameplayFunctionLibrary::ApplyDamage(this, OtherActor, 25.0f);
			}

			// Apply Direcional Damage if actor simulate Physics
			if (OtherComp->IsSimulatingPhysics())
			{
				// Direction = Target - Origin
				FVector Direction = Hit.TraceEnd - Hit.TraceStart;
				Direction.Normalize();
				OtherComp->AddImpulseAtLocation(Direction * 100000.f, Hit.ImpactPoint);
			}

			Explode();
		}
	}
}

void AARogProjectileBase::Explode_Implementation()
{
	if (IsValid(this))
	{
		// Play FVX
		UGameplayStatics::SpawnEmitterAtLocation(this, HitParticle, GetActorLocation(), GetActorRotation());

		// Play sound
		UGameplayStatics::SpawnSoundAtLocation(this, ImpactSound, GetActorLocation());
		
		// Play camera shake
		UGameplayStatics::PlayWorldCameraShake(this, SimpleCameraShake, GetActorLocation(), ImpactShakeInnerRadius, ImpactShakeOuterRadius);
		
		Destroy();
	}
}
