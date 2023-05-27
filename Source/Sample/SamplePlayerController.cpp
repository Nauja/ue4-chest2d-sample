// Fill out your copyright notice in the Description page of Project Settings.


#include "SamplePlayerController.h"


void ASamplePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Allow to keep the true colors of sprites
	ConsoleCommand(TEXT("showflag.postprocessing 0"));
}
