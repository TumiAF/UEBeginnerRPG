#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterType.h"
#include "Enemy.generated.h"

class ASoul;
class UHealthBarComponent;
class AAIController;
class UPawnSensingComponent;
class AWeapon;

UCLASS()
class BEGINNERRPG_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	
	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;
	/** </AActor> */

	/** <IHitInterface> */
	virtual void GetHit_Implementation(const FVector& ImpactPoint,AActor* Hitter) override;
	/** </IHitInterface> */

	
protected:
	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

	/** <ABaseCharacter> */
	///敌人死亡
	virtual void Die_Implementation() override;
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual void AttackEnd() override;
	/** </ABaseCharacter> */

	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

private:
	/** AI Behavior */

	void InitialEnemy();
	void InitialEnemyCollision(); //初始化Enemy Collision相关设置
	void CheckPatrolTarget();
	void CheckCombatTarget();
	///巡逻倒计时到了之后的回调
	void PatrolTimerFinished();
	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsDead();
	bool IsEngaged();
	void ClearPatrolTimer();
	void StartAttackTimer();
	void ClearAttackTimer();
	///判断是否在目标范围内
	bool InTargetRange(TObjectPtr<AActor> Target, double Radius);
	void SpawnDefaultWeapon();
	void SpawnSouls();
	
	///选择巡逻目标点
	TObjectPtr<AActor> ChoosePatrolTarget();
	
	///通过AIController移动到目标点
	void MoveToTarget(TObjectPtr<AActor> Target);
	
	/// Enemy发现目标之后触发的回调函数 (UPawnSensingComponent) 
	/// @param SeenPawn 发现的目标
	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UPawnSensingComponent> PawnSensingComp;
	
	//管理血条Widget
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthBarComponent> HealthBarWidget;
	
	UPROPERTY(EditAnywhere,Category="Tumi Combat")
	TSubclassOf<AWeapon> WeaponClass;

	///战斗半径（超过这个距离，关闭血条显示）
	UPROPERTY(EditAnywhere,Category="Tumi Combat")
	double CombatRadius = 500.f;

	///攻击半径
	UPROPERTY(EditAnywhere,Category="Tumi Combat")
	double AttackRadius = 125.f;

	///可接受半径，到达范围后停止移动
	UPROPERTY(EditAnywhere,Category="Tumi Combat")
	double AcceptanceRadius = 30.f;

	UPROPERTY()
	TObjectPtr<AAIController> EnemyAIController;
	
	///当前巡逻目标
	UPROPERTY(EditInstanceOnly,Category="Tumi AI Navigation")
	TObjectPtr<AActor> PatrolTarget;
	///巡逻目标数组
	UPROPERTY(EditInstanceOnly,Category="Tumi AI Navigation")
	TArray<TObjectPtr<AActor>> PatrolTargets;

	///巡逻半径,小于这个距离之后，切换巡逻目标点
	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	///巡逻计时器
	FTimerHandle PatrolTimer;
	UPROPERTY(EditAnywhere,Category="Tumi AI Navigation")
	float PatrolWaitTimeMax = 6.f;
	UPROPERTY(EditAnywhere,Category="Tumi AI Navigation")
	float PatrolWaitTimeMin = 3.f;

	UPROPERTY(EditAnywhere,Category="Tumi AI Behavior")
	float PatrollingSpeed = 125.f;
	UPROPERTY(EditAnywhere,Category="Tumi AI Behavior")
	float ChasingSpeed = 300.f;

	FTimerHandle AttackTimer;
	UPROPERTY(EditAnywhere,Category="Tumi Combat")
	float AttackMin = 0.5f;
	UPROPERTY(EditAnywhere,Category="Tumi Combat")
	float AttackMax = 1.f;

	UPROPERTY(EditAnywhere,Category="Tumi Properties")
	TSubclassOf<ASoul> SoulClass;

};
