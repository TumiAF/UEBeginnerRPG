// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class USoundBase;
class UBoxComponent;
/**
 * 
 */
UCLASS()
class BEGINNERRPG_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	///把武器的mesh挂载到指定的socket上
	void AttachMeshToSocket(USceneComponent* InParent, FName InSocketName);
	
	FORCEINLINE UBoxComponent* GetWeaponBox() const {return WeaponBox;}

	//武器碰撞检测中需要忽略的Actor
	TArray<AActor*> IgnoreActors;

protected:
	//1、【 UFUNCTION() 】 因为这个是override的函数，所以不需要再加上这个宏了，可以认定为它已经有一个宏在这里了；
	//2、尾部添加 override 标识这是重写了父类的函数；
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	
	// 在Weapon上创建 物理Field，并在蓝图中进行实现函数
	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);
private:
	void BoxTrace(FHitResult& BoxHit); //执行BoxTrace
	void ExecuteGetHit(FHitResult& BoxHit); //让被击中的Actor执行GetHit
	bool ActorIsSameType(AActor* OtherActor);
	
	void PlayEquipSound();
	void DisableEmberEffect();
	void DisableSphereCollision();
	
	///在拾取武器时播放的音效
	UPROPERTY(EditAnywhere,Category="Tumi Weapon Properties")
	USoundBase* EquipSound;
	
	///武器的碰撞盒
	UPROPERTY(VisibleAnywhere,Category="Tumi Weapon Properties")
	UBoxComponent* WeaponBox;

	///碰撞检测的起始点
	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;
	///碰撞检测的结束点
	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;

	UPROPERTY(EditAnywhere,Category="Tumi Weapon Properties")
	FVector BoxTraceExtent = FVector(5.f);
	
	UPROPERTY(EditAnywhere,Category="Tumi Weapon Properties")
	bool bShowBoxDebug = false;

	///该武器造成的伤害
	UPROPERTY(EditAnywhere,Category="Tumi Weapon Properties")
	float Damage = 20.f;
	

};
