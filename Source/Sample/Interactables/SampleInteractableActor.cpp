// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SampleInteractableActor.h"
#include "SampleCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"

USampleInteractableConfig::USampleInteractableConfig()
{}

ASampleInteractableActor::ASampleInteractableActor(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , Config(nullptr)
{
    PrimaryActorTick.bCanEverTick = true;

    SceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootComponent"));
    SceneComponent->SetIsReplicated(true);

    RootComponent = SceneComponent;
    bReplicates = true;
}

void ASampleInteractableActor::PostInitializeComponents()
{
    Super::PostInitializeComponents();
}

#if WITH_EDITOR
/**
 * Update the AActor when assigned USampleInteractableActorConfig changes.
 */
void ASampleInteractableActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    FProperty* PropertyThatChanged = PropertyChangedEvent.Property;
    if (PropertyThatChanged)
    {
    }

    Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void ASampleInteractableActor::Interact_Implementation(class AActor* Other)
{
    if (Config)
    {
        /** This will be called after x seconds */
        GetWorldTimerManager().SetTimer(TimerHandle_Reset, this, &ASampleInteractableActor::Reset, Config->ResetTimer, false);
    }
}

void ASampleInteractableActor::NotifyActorBeginOverlap(class AActor* Other)
{
    Super::NotifyActorBeginOverlap(Other);

    if (IsValid(Other) && !IsPendingKill())
    {
        if (OverlappingActors.Num() == 0)
        {
            NotifyBeginOverlappingActors();
        }
        OverlappingActors.AddUnique(Other);
    }
}

void ASampleInteractableActor::NotifyActorEndOverlap(class AActor* Other)
{
    Super::NotifyActorEndOverlap(Other);

    if (IsValid(Other) && !IsPendingKill())
    {
        OverlappingActors.Remove(Other);
        if (OverlappingActors.Num() == 0)
        {
            NotifyEndOverlappingActors();
        }
    }
}

void ASampleInteractableActor::Reset()
{}

void ASampleInteractableActor::Tick(float DeltaSeconds)
{
}