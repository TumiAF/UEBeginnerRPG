// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashOverlay.generated.h"

/**
 * 
 */
class UProgressBar;
class UTextBlock;
UCLASS()
class BEGINNERRPG_API USlashOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	///设置血量进度条的百分比
	void SetHealthBarPercent(float Percent);
	///设置耐力进度条的百分比
	void SetStaminaBarPercent(float Percent);
	void SetGold(int32 Gold);
	void SetSouls(int32 Souls);
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> Progress_Health; //血量进度条

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> Progress_Stamina; //耐力进度条

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_GoldNum; //金币数量文本显示

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_SoulNum; //灵魂数量文本显示
	
};
