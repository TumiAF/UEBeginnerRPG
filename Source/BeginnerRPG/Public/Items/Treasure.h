// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Treasure.generated.h"

/**
 * 
 */
UCLASS()
class BEGINNERRPG_API ATreasure : public AItem
{
	GENERATED_BODY()
public:
	
	FORCEINLINE int32 GetGold() const { return Gold; }
	
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult) override;
private:
	//宝物的价值，即金币数值
	UPROPERTY(EditAnywhere,Category = "Tumi Properties")
	int32 Gold;
};
