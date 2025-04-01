
#include "Characters/BaseCharacter.h"

#include "BeginnerRPG/DebugMacros.h"
#include "Characters/CharacterType.h"
#include "Components/AttributeComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	///创建Attribute
	Attribute = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attribute"));
}


void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint,AActor* Hitter)
{
	if(IsAlive() && Hitter)
	{
		///播放受击动画
		DirectionalHitReact(Hitter->GetActorLocation());
		///播放受击音效
		PlayHitSound(ImpactPoint);
		///播放受击特效
		SpawnHitParticle(ImpactPoint);
	}
	else 
	{
		Die();
	}
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/// 设置武器配置类型
/// @param CollisionType CollisionEnabled类型
void ABaseCharacter::SetWeaponCollision(ECollisionEnabled::Type CollisionType)
{
	if(EquipWeapon && EquipWeapon->GetWeaponBox())
	{
		EquipWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionType);
		EquipWeapon->IgnoreActors.Empty();//清空忽略Actor的列表（功能：防止攻击时多次击中同一个敌人）
	}
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

bool ABaseCharacter::IsAlive()
{
	return Attribute && Attribute->IsAlive();
}

void ABaseCharacter::Attack()
{
	if(CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}
}

int32 ABaseCharacter::PlayAttackMontage()
{
	const int32 Selection = PlayRandomMontageSection(AttackMontage,AttackMontageSections);
	return Selection;
}

int32 ABaseCharacter::PlayDeathMontage()
{
	const int32 Selection =  PlayRandomMontageSection(DeathMontage,DeathMontageSections);
	const TEnumAsByte<EDeathPose> Pose(Selection);	
	if(Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}
	return Selection;
}

void ABaseCharacter::PlayDodgeMontage()
{
	PlayMontageSection(DodgeMontage,FName("Default"));
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

int32 ABaseCharacter::PlayRandomMontageSection(TObjectPtr<UAnimMontage> Montage, const TArray<FName>& SectionNames)
{
	if(SectionNames.Num() <= 0) return -1;
	const int32 MaxSectionIndex = SectionNames.Num() - 1;
	const int32 Selection = FMath::RandRange(0,MaxSectionIndex);
	PlayMontageSection(Montage,SectionNames[Selection]);
	return Selection;
}

///中断AttackMontage的播放
void ABaseCharacter::StopAttackMontage()
{
	TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance)
	{
		AnimInstance->Montage_Stop(0.2f,AttackMontage);
	}
}

void ABaseCharacter::DisableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::AttackEnd()
{
}

void ABaseCharacter::DodgeEnd()
{
}

void ABaseCharacter::Die_Implementation()
{
	Tags.Add(FName("Dead"));
	///播放死亡Montage
	PlayDeathMontage();
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	const TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName,HitReactMontage);
	}
}

/**
 * @brief 受击后根据受击点播放合适方向的受击动画
 * @param ImpactPoint 受击点
 */
void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	//获取角色朝向矢量
	const FVector Forward = GetActorForwardVector();
	//将收集点矢量调整到和角色朝向矢量一个水平面上
	const FVector ImpactLower(ImpactPoint.X,ImpactPoint.Y,GetActorLocation().Z);
	//从角色本体指向受击点的矢量
	const FVector ToHit = (ImpactLower - GetActorLocation()).GetSafeNormal();
	//点乘，Forward * ToHit = |Forward||ToHit| * cos(theta)
	// 两向量已经归一化，所以模为 1，因此点乘结果为 cos(theta)
	const double CosTheta = FVector::DotProduct(Forward,ToHit);
	//反三角函数求Theta角 (结果是弧度)
	const double ThetaRadian = FMath::Acos(CosTheta);
	//将 弧度 转换成 角度
	double Theta = FMath::RadiansToDegrees(ThetaRadian);
	
	///叉乘，该矢量若朝上，则表示ToHit在Forward的右边（UE左手系，所以叉乘结果取反）
	const FVector CrossProduct = FVector::CrossProduct(Forward,ToHit);
	if(CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}

	///根据Theta角，确认受击方向，播放指定的动画序列
	FName Section("FromBack");
	if(Theta >-45.f &&Theta < 45.f)
	{
		Section = FName("FromFront");
	}
	else if(Theta >=-135.f &&Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	else if(Theta>=45.f &&Theta < 135.f)
	{
		Section = FName("FromRight");
	}
	PlayHitReactMontage(Section);
	
	/**
	 * 调试
	 * 输出Theta角
	 * 绘制矢量箭头
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1,5.f,FColor::Red,FString::Printf(TEXT("Theta = %f"),Theta));
	}
	UKismetSystemLibrary::DrawDebugArrow(this,GetActorLocation(),GetActorLocation()+ CrossProduct*210.f,5.f,FColor::Green,5.f);
	UKismetSystemLibrary::DrawDebugArrow(this,GetActorLocation(),GetActorLocation()+ Forward*60.f,5.f,FColor::Blue,5.f);
	UKismetSystemLibrary::DrawDebugArrow(this,GetActorLocation(),GetActorLocation()+ ToHit*60.f,5.f,FColor::Red,5.f);
	*/
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if(Attribute)
	{
		Attribute->ReceiveDamage(DamageAmount); //修改Attribute中的属性数值
	}
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	///在受击点播放受击音效
	if(HitsSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this,HitsSound,ImpactPoint);
	}
}

void ABaseCharacter::SpawnHitParticle(const FVector& ImpactPoint)
{
	if(HitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this,HitParticles,ImpactPoint);
	}
}

void ABaseCharacter::PlayMontageSection(TObjectPtr<UAnimMontage> Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage); //播放Montage
		AnimInstance->Montage_JumpToSection(SectionName,Montage);
	}
}

/// 获得MotionWrap之后的位置
/// @return 扭曲后的位置（和目标保持一定距离）
FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if(CombatTarget == nullptr) return FVector();

	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();
	///从CombatTarget指向本体的向量
	FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();
	TargetToMe *= WarpTargetDistance;
	return CombatTargetLocation + TargetToMe;
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if(CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}



