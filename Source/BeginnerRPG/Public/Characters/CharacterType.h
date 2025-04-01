#pragma once

///角色的状态（可用于切换动画） \n
///uint8：优化枚举类型 \n
///UENUM：将该类型暴露给反射系统 \n
///BlueprintType：允许在蓝图中使用该类型 \n 
UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	//ECS：即ECharacterState的首字母缩写，用于标识和规范枚举变量
	//UMETA(DisplayName = "Unequipped")：优化在蓝图中看到的文本显示
	ECS_Unequipped UMETA(DisplayName = "Unequipped"), //未持有武器
	ECS_EquippedOneHandWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"), //持有单手武器
	ECS_EquippedTwoHandWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon")//持有双手武器
};

///行为状态
UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"), //空闲，该状态可以移动
	EAS_HitReaction UMETA(DisplayName = "HitReaction"), //受击状态
	EAS_Attacking UMETA(DisplayName = "Attacking"), //攻击
	EAS_EquippingWeapon UMETA(DisplayName = "EquippingWeapon"), //装备or卸下武器
	EAS_Dodge UMETA(DisplayName = "Dodge"), //翻滚
	EAS_Dead UMETA(DisplayName = "Dead") //死亡
};

///死亡姿势
UENUM(BlueprintType)
enum EDeathPose
{
	EDP_Death1 UMETA(DisplayName = "Death1"), //死亡姿势1
	EDP_Death2 UMETA(DisplayName = "Death2"),
	EDP_Death3 UMETA(DisplayName = "Death3"),
	EDP_Death4 UMETA(DisplayName = "Death4"),
	EDP_Death5 UMETA(DisplayName = "Death5"),

	EDP_MAX UMETA(DisplayName = "DefaultMax")
};

///Enemy状态
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_NoState UMETA(DispalyName = "NoState"), //无状态
	EES_Dead UMETA(DisplayName = "Dead"), //死亡
	EES_Patrolling UMETA(DisplayName = "Patrolling"), //巡逻
	EES_Chasing UMETA(DisplayName = "Chasing"), //追击
	EES_Attacking UMETA(DisplayName = "Attacking"), //攻击
	EES_Engaged UMETA(DisplayName = "Engaged"), // 交战状态

	
};