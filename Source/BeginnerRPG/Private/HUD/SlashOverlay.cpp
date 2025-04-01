// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USlashOverlay::SetHealthBarPercent(float Percent)
{
	if(Progress_Health)
	{
		Progress_Health->SetPercent(Percent);
	}
}

void USlashOverlay::SetStaminaBarPercent(float Percent)
{
	if(Progress_Stamina)
	{
		Progress_Stamina->SetPercent(Percent);
	}
}

void USlashOverlay::SetGold(int32 Gold)
{
	if(Text_GoldNum)
	{
		const FText GoldText = FText::FromString(FString::FromInt(Gold));
		Text_GoldNum->SetText(GoldText);
	}
}

void USlashOverlay::SetSouls(int32 Souls)
{
	if(Text_SoulNum)
	{
		const FText SoulText = FText::FromString(FString::FromInt(Souls));
		Text_SoulNum->SetText(SoulText);
	}
}
