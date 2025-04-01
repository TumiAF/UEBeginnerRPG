// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterType.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class BEGINNERRPG_API ABaseCharacter : public ACharacter,public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

	/// 设置武器的碰撞
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(ECollisionEnabled::Type CollisionType);

	///获得DeathPose
	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const{return DeathPose;}

protected:
	/// -----*** Variable ***-----
	//属性参数
	UPROPERTY(VisibleAnywhere,Category = "Tumi Actor Attributes")
	TObjectPtr<UAttributeComponent> Attribute;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;//死亡姿势
	
	UPROPERTY(VisibleAnywhere,Category="Tumi Weapon")
	TObjectPtr<AWeapon> EquipWeapon;

	///攻击Enemy的Actor
	UPROPERTY(BlueprintReadOnly,Category="Tumi Combat")
	TObjectPtr<AActor> CombatTarget;

	///MotionWrapping后，离实际的目标间隔的距离
	UPROPERTY(EditAnywhere,Category="Tumi Combat")
	double WarpTargetDistance = 50.f;

	/// Animation Montage
	//攻击Montage
	UPROPERTY(EditDefaultsOnly,Category= "Tumi Montages" )
	TObjectPtr<UAnimMontage> AttackMontage;
	//受击Montage
	UPROPERTY(EditDefaultsOnly,Category = "Tumi Montage")
	TObjectPtr<UAnimMontage> HitReactMontage;
	///死亡动画
	UPROPERTY(EditDefaultsOnly,Category = "Tumi Montage")
	TObjectPtr<UAnimMontage> DeathMontage;
	///翻滚
	UPROPERTY(EditDefaultsOnly,Category = "Tumi Montage")
	TObjectPtr<UAnimMontage> DodgeMontage;
	///Attack Montage Section的数组
	UPROPERTY(EditAnywhere,Category= "Tumi Montage")
	TArray<FName> AttackMontageSections;
	UPROPERTY(EditAnywhere,Category= "Tumi Montage")
	TArray<FName> DeathMontageSections;
	
	UPROPERTY(EditAnywhere,Category="Tumi Actor Attributes")
	float DeathLifeSpan = 5.f;
	/// -----*** Variable End ***-----

	
	/// -----*** Function ***----- 
	virtual void BeginPlay() override;

	///实现GetHit函数
	virtual void GetHit_Implementation(const FVector& ImpactPoint,AActor* Hitter) override;

	///判断当前是否允许攻击
	virtual bool CanAttack();
	
	virtual void Attack();
	
	///BlueprintCallable：可以在蓝图中调用该函数； \n
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	UFUNCTION(BlueprintCallable)
	virtual void DodgeEnd();

	UFUNCTION(BlueprintNativeEvent)
	void Die();
	
	///播放受击动画
	virtual void PlayHitReactMontage(const FName& SectionName);

	///根据受到不同方向攻击的受击表现
	virtual void DirectionalHitReact(const FVector& ImpactPoint);

	virtual void HandleDamage(float DamageAmount);

	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	virtual void PlayDodgeMontage();

	void DisableCapsule();
	bool IsAlive();
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticle(const FVector& ImpactPoint);
	void PlayMontageSection(TObjectPtr<UAnimMontage> Montage,const FName& SectionName);
	int32 PlayRandomMontageSection(TObjectPtr<UAnimMontage> Montage,const TArray<FName>& SectionNames);
	void StopAttackMontage();
	void DisableMeshCollision();

	///Motion Wrap
	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();
	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();
	
	/// -----*** Function End ***-----
private:
	///受击音效
	UPROPERTY(EditAnywhere,Category = "Tumi Combat")
	TObjectPtr<USoundBase> HitsSound;

	///受击生成的粒子特效
	UPROPERTY(EditAnywhere,Category = "Tumi Combat")
	TObjectPtr<UParticleSystem> HitParticles;
};
