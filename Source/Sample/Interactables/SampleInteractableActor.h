// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameFramework/Actor.h"
#include "SampleInteractableActor.generated.h"

UCLASS(config = Game, Blueprintable, BlueprintType)
class SAMPLE_API USampleInteractableConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	USampleInteractableConfig();

	/** Seconds before resetting the state of this interactable */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sample)
	float ResetTimer;
};

UCLASS(config = Game, BlueprintType)
class SAMPLE_API ASampleInteractableActor : public AActor
{
	GENERATED_BODY()

public:
	ASampleInteractableActor(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	void NotifyActorBeginOverlap(class AActor* Other) override;
	void NotifyActorEndOverlap(class AActor* Other) override;
	virtual void Tick(float DeltaSeconds);
	
	/** Called from ASampleCharacter when pressing Interact button */
	UFUNCTION(BlueprintNativeEvent)
	void Interact(class AActor* Other);

	FORCEINLINE class USampleInteractableConfig* GetConfig() const { return Config; }

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void NotifyBeginOverlappingActors();

	UFUNCTION(BlueprintImplementableEvent)
	void NotifyEndOverlappingActors();

	/** Reset the state of this interactable */
	virtual void Reset();

private:
	UPROPERTY(Category = Sample, EditAnywhere)
	class USampleInteractableConfig* Config;

	UPROPERTY(Category = Sample, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* SceneComponent;

	UPROPERTY(transient)
	TArray<class AActor*> OverlappingActors;

	/** Timer before resetting the state of this interactable */
	FTimerHandle TimerHandle_Reset;
};
