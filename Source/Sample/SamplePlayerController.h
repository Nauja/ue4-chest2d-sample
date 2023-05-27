// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SamplePlayerController.generated.h"

/**
 * The purpose of this class is to execute the command showflag.postprocessing 0
 * when the player spawns.
 */
UCLASS()
class SAMPLE_API ASamplePlayerController : public APlayerController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
};
