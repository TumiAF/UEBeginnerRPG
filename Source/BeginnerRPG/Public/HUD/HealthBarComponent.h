// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

/**
 * 
 */
class UHealthBar;
UCLASS()
class BEGINNERRPG_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	void SetHealthPercent(float Percent);
private:
	UPROPERTY() //空的也能保证它是以空指针进行初始化，而非垃圾数据开始；
	TObjectPtr<UHealthBar> HealthBarWidget;
};
