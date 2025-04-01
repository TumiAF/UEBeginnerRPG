


#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Characters/SlashCharacter.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Items/Soul.h"
#include "Items/Weapons/Weapon.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"


AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());
	
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SightRadius = 3000.f; //视线半径
	PawnSensingComp->SetPeripheralVisionAngle(45.f); //周边视觉角度
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	InitialEnemy();
	///绑定PawnSensing的回调
	if(PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this,&AEnemy::PawnSeen);
	}
	///获得AAIController
	EnemyAIController = Cast<AAIController>(GetController());
	
	///初始化起始巡逻目标点，先前往第一个点位
	if(PatrolTarget == nullptr && PatrolTargets.Num() > 0)
	{
		const int32 RandomIndex = FMath::RandRange(0,PatrolTargets.Num() - 1);
		if(PatrolTarget != PatrolTargets[RandomIndex])
		{
			PatrolTarget = PatrolTargets[RandomIndex];
		}
	}
	MoveToTarget(PatrolTarget);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(IsDead()) return;
	
	if(EnemyState > EEnemyState::EES_Patrolling) //Chasing and Attacking
	{
		///判断是否还在攻击范围内， 如果不在攻击范围内，则隐藏血条
		CheckCombatTarget();
	}
	else //Patrolling状态
	{
		///判断当前是否处于巡逻半径内，如果处于，隔一段时间重新选择一个目标点
		CheckPatrolTarget();
	}
	
}

void AEnemy::Destroyed()
{
	if( EquipWeapon)
	{
		EquipWeapon->Destroy();
	}
}

///初始化敌人
void AEnemy::InitialEnemy()
{
	Tags.Add(FName("Enemy"));
	///初始化Collision设置
	InitialEnemyCollision();
	///生成默认武器
	SpawnDefaultWeapon();
	///默认关闭血条
	HideHealthBar();
	
}

///初始化敌人Collision相关参数
void AEnemy::InitialEnemyCollision()
{
	//设置碰撞体的对象类型 - WorldDynamic
	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
	//设置碰撞体的检测响应：Visibility 为 Block;Camera为 Ignore 
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	//设置 允许生成Overlap事件
	GetMesh()->SetGenerateOverlapEvents(true);
	//设置胶囊体的检测响应 - Camera为 Ignore
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint,AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint,Hitter);
	if(IsDead())
	{
		HideHealthBar();
	}
	else
	{
		///受到伤害，显示血条
		ShowHealthBar();
	}
	ClearPatrolTimer();
	ClearAttackTimer();
	SetWeaponCollision(ECollisionEnabled::NoCollision);
	StopAttackMontage();

	if(IsInsideAttackRadius() && !IsDead())
	{
		StartAttackTimer();
	}
}

///检测目标是否在攻击范围内
void AEnemy::CheckCombatTarget()
{
	///不在攻击范围内，隐藏血条、失去攻击欲望，回到Patrolling状态
	if(IsOutsideCombatRadius())
	{
		ClearAttackTimer();
		LoseInterest();
		if(!IsEngaged())
		{
			StartPatrolling();
		}
		
	}
	//不在Attack范围内，继续追击Player
	else if(IsOutsideAttackRadius() && !IsChasing())
	{
		ClearAttackTimer();
		if(!IsEngaged())
		{
			ChaseTarget();
		}
	}
	//在Attack范围内，则攻击Player
	else if(CanAttack())
	{
		StartAttackTimer();
	}
}

///检查巡逻目标点，隔一段时间移动到新的目标点
void AEnemy::CheckPatrolTarget()
{
	if(InTargetRange(PatrolTarget,PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget(); //选择一个新的目标点
		///设置巡逻计时器
		const float WaitTime = FMath::RandRange(PatrolWaitTimeMin,PatrolWaitTimeMax);
		GetWorldTimerManager().SetTimer(PatrolTimer,this,&AEnemy::PatrolTimerFinished,WaitTime);
	}
}


///受到伤害后执行的逻辑
float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	///伤害数据处理
	HandleDamage(DamageAmount);
	///获得攻击者
	CombatTarget = EventInstigator->GetPawn();
	///判断目标是否在攻击距离内
	if(IsInsideAttackRadius())
	{
		EnemyState = EEnemyState::EES_Attacking;
	}
	else
	{
		///受到攻击后，开始追击目标
		ChaseTarget();
	}
	return DamageAmount;
}

///Enemy死亡
void AEnemy::Die_Implementation()
{
	Super::Die_Implementation();
	///设置死亡状态
	EnemyState = EEnemyState::EES_Dead;
	///关闭血条
	HideHealthBar();
	///关闭攻击计时器
	ClearAttackTimer();
	///关闭碰撞体
	DisableCapsule();
	///销毁尸体
	SetLifeSpan(DeathLifeSpan);
	///关闭武器碰撞
	SetWeaponCollision(ECollisionEnabled::NoCollision);
	///生成灵魂
	SpawnSouls();
}


/// @param Target 目标点位 o r目标单位
/// @param Radius 检测范围
/// @return 判断是否在目标范围内
bool AEnemy::InTargetRange(TObjectPtr<AActor> Target, double Radius)
{
	if(Target == nullptr) return false;
	
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

/// 通过随机数选择巡逻目标点
/// @return 巡逻目标点
TObjectPtr<AActor> AEnemy::ChoosePatrolTarget()
{
	///排除当前目标点，将其余Target添加到一个合法数组中，防止随机到相同的目标点；
	TArray<TObjectPtr<AActor>> ValidTargets;
	for(TObjectPtr<AActor> Target : PatrolTargets)
	{
		if(Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}
	const int32 NumPatrolTargets = ValidTargets.Num();
	if(NumPatrolTargets > 0)
	{
		const int32 RandomIndex = FMath::RandRange(0,NumPatrolTargets - 1);
		return ValidTargets[RandomIndex];
	}
	return PatrolTarget;
}

/// 通过AIController移动到目标点
/// @param Target AIController MoveTo 的目标
void AEnemy::MoveToTarget(TObjectPtr<AActor> Target)
{
	if(EnemyAIController == nullptr || Target == nullptr) return;
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target); //设置目标单位
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);	//设置可接受半径，到达范围后停止移动
	EnemyAIController->MoveTo(MoveRequest);
}

///巡逻计时器的回调：巡逻倒计时之后前往下一个目标点
void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

/// Enemy发现目标之后触发的回调函数
/// @param SeenPawn 发现的目标
void AEnemy::PawnSeen(APawn* SeenPawn)
{
	if(SeenPawn->ActorHasTag(FName("Dead")))
	{
		return;
	}
	
	const bool bShouldChaseTarget =
		EnemyState == EEnemyState::EES_Patrolling &&
		SeenPawn->ActorHasTag(FName("SlashCharacter"));
	if(bShouldChaseTarget)
	{
		CombatTarget = SeenPawn;
		ClearPatrolTimer();
		ChaseTarget();
	}
}

void AEnemy::Attack()
{
	Super::Attack();
	if(CombatTarget == nullptr) return;
	
	EnemyState = EEnemyState::EES_Engaged;
	PlayAttackMontage();
}

bool AEnemy::CanAttack()
{
	return IsInsideAttackRadius() && !IsAttacking() && !IsEngaged() && !IsDead();
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);
	if(Attribute && HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attribute->GetHealthPercent());//修改HealthBarWidget的样式
	}
}

void AEnemy::AttackEnd()
{
	Super::AttackEnd();
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void AEnemy::HideHealthBar()
{
	if(HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
}

void AEnemy::ShowHealthBar()
{
	if(HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
}

///失去兴趣，放弃追踪
void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}

///开始Patrolling
void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

///是否在战斗范围内
bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget,CombatRadius);
}

///Chasing状态，追击目标
void AEnemy::ChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget);
}

///是否在攻击范围内
bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget,AttackRadius);
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget,AttackRadius);
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

///启动Attack计时器
void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMin,AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer,this,&AEnemy::Attack,AttackTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

///初始化生成武器
void AEnemy::SpawnDefaultWeapon()
{
	const TObjectPtr<UWorld> World = GetWorld();
	if(World && WeaponClass)
	{
		TObjectPtr<AWeapon> DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(),FName("WeaponSocket"),this,this);
		EquipWeapon = DefaultWeapon;
	}
}

void AEnemy::SpawnSouls()
{
	TObjectPtr<UWorld> World = GetWorld();
	if(World && SoulClass && Attribute)
	{
		const FVector SpawnActorLocation = GetActorLocation() + FVector(0.f,0.f,200.f);
		const TObjectPtr<ASoul> SpawnSoul = World->SpawnActor<ASoul>(SoulClass,SpawnActorLocation,GetActorRotation());
		if(SpawnSoul)
		{
			SpawnSoul->SetSouls(Attribute->GetSouls());//为生成的灵魂设置经验数值
		}
	}
}


