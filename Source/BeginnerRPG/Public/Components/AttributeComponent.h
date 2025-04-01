// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEGINNERRPG_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	///当前单位是否存活
	bool IsAlive();
	///接收到伤害
	void ReceiveDamage(float Damage);
	///消耗了耐力值
	void UseStamina(float StaminaCost);
	void RegenerateStamina(float DeltaTime);
	///获取当前生命值百分比
	float GetHealthPercent();
	///获取当前耐力值百分比
	float GetStaminaPercent();

	void AddSouls(int32 NumberOfSouls);
	void AddGold(int32 NumberOfGold);

	
	FORCEINLINE int32 GetGold() const {return Gold;}
	FORCEINLINE int32 GetSouls() const {return Souls;}
	FORCEINLINE float GetStamina() const {return Stamina;}
	FORCEINLINE float GetDodgeCost() const {return DodgeCost;}

protected:
	virtual void BeginPlay() override;

private:
	//当前生命值
	UPROPERTY(EditAnywhere,Category = "Tumi Actor Attributes")
	float Health;
	//最大生命值
	UPROPERTY(EditAnywhere,Category = "Tumi Actor Attributes")
	float MaxHealth;
	//当前耐力值
	UPROPERTY(EditAnywhere,Category = "Tumi Actor Attributes")
	float Stamina;
	//最大耐力值
	UPROPERTY(EditAnywhere,Category = "Tumi Actor Attributes")
	float MaxStamina;
	///耐力恢复速度（/秒)
	UPROPERTY(EditAnywhere,Category = "Tumi Actor Attributes")
	float StaminaRegenerateRate = 5.f;

	//翻滚消耗
	UPROPERTY(EditAnywhere,Category = "Tumi Actor Attributes")
	float DodgeCost = 10.f;
	
	UPROPERTY(EditAnywhere,Category = "Tumi Actor Attributes")
	int32 Gold;

	UPROPERTY(EditAnywhere,Category = "Tumi Actor Attributes")
	int32 Souls;

};
