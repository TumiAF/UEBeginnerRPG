// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashHUD.h"

#include "Blueprint/UserWidget.h"
#include "HUD/SlashOverlay.h"

void ASlashHUD::BeginPlay()
{
	Super::BeginPlay();
	const TObjectPtr<UWorld> World = GetWorld();
	if(World)
	{
		const TObjectPtr<APlayerController> PlayerController =  World->GetFirstPlayerController();
		if(PlayerController && SlashOverlayClass)
		{
			SlashOverlay = CreateWidget<USlashOverlay>(PlayerController,SlashOverlayClass);
			SlashOverlay->AddToViewport();
		}
	}
}
