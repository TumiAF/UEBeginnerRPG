// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h" //使得我们的item类可以参与UE的反射系统


class USphereComponent;
class UNiagaraComponent;
class UNiagaraSystem;
///物品的状态
enum class EItemState : uint8
{
	EIS_Hovering, // 悬浮状态（待拾取）
	EIS_Equipped, //已装备
};

// BEGINNERRPG_API： Expands to : _declspec(dllexport)
// 表示由Declspec拓展，DLL导出，可以从DLL或动态链接库中使用此类型；
UCLASS()
class BEGINNERRPG_API AItem : public AActor
{
	GENERATED_BODY() //同样，这个宏也是为了参与UE的反射系统
private:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	float runningTime = 0.f;

protected:
	virtual void BeginPlay() override;

	
	EItemState ItemState = EItemState::EIS_Hovering;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComp;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere,Category="Tumi Effect")
	TObjectPtr<UNiagaraComponent> ItemEffect;
	
	UPROPERTY(EditAnywhere,Category="Tumi Effect")
	TObjectPtr<UNiagaraSystem> PickupEffect;
	
	UPROPERTY(EditAnywhere,Category="Tumi Effect")
	TObjectPtr<USoundBase> PickupSound;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Tumi Sin Parameters")
	float amplitude = 1.f; //幅度

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Tumi Sin Parameters")
	float timeConstant = 5.f;//时间常数，加速sin输入的进程

	

	// BlueprintPure：表示这是个纯函数，即不修改任何成员变量，单纯做功能执行；
	UFUNCTION(BlueprintPure)
	float TransformedSin();
	UFUNCTION(BlueprintPure)
	float TransformedCos();

	template<typename T>
	T Avg(T first,T second);

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
	///播放拾取后的特效
	UFUNCTION()
	virtual void SpawnPickupSystem();
	///播放拾取后的音效
	UFUNCTION()
	virtual void SpawnPickupSound();

public:	
	AItem();
	virtual void Tick(float DeltaTime) override;

};

template <typename T>
T AItem::Avg(T first, T second)
{
	return (first + second) / 2;
}
