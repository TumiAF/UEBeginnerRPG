// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Soul.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class BEGINNERRPG_API ASoul : public AItem
{
	GENERATED_BODY()
public:
	
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE void SetSouls(int32 NumberOfSouls) { Souls = NumberOfSouls; }
	
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult) override;
private:
	///这个灵魂包含的经验数值
	UPROPERTY(EditAnywhere,Category="Tumi Properties")
	int32 Souls;
};
