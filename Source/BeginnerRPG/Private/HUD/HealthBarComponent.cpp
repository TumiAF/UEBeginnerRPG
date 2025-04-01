// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarComponent.h"
#include "Components/ProgressBar.h"
#include "HUD/HealthBar.h"

void UHealthBarComponent::SetHealthPercent(float Percent)
{
	if(HealthBarWidget == nullptr)
	{
		/** 获得UserWidget-HealthBar
		 * 通过 UWidgetComponent 上自带的接口，来获得它对应的UserWidget
		 * 再通过类型转换，转为UHealthBar；
		 */
		HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
	}
	if (HealthBarWidget && HealthBarWidget->HealthBar)
	{
		///调用 UProgressBar 的函数进行设置进度条百分比
		HealthBarWidget->HealthBar->SetPercent(Percent);
	}
}
