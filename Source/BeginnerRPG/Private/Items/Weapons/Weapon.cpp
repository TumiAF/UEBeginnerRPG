// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"

#include "NiagaraComponent.h"
#include "Characters/SlashCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Interfaces/HitInterface.h"
#include "Kismet/GameplayStatics.h"

AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Collision Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
	//设置碰撞类型 - 仅查询 ——> 初始化先关闭碰撞
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//设置碰撞响应 - 所有通道都为 Overlap
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	//设置单一通道的碰撞响应 - 将Pawn设置为Ignore
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Ignore);

	//创建用于检测的起始点和结束点
	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	//绑定碰撞回调函数
	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                           int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ActorIsSameType(OtherActor)) return;

	FHitResult BoxHit;
	BoxTrace(BoxHit);
	
	if(BoxHit.GetActor())
	{
		if (ActorIsSameType(BoxHit.GetActor())) return;
		
		///造成伤害
		UGameplayStatics::ApplyDamage
		(
			BoxHit.GetActor(), //受到伤害的Actor
			Damage, //基础伤害
			GetInstigator()->GetController(), //造成伤害的Instigator（e.g. 使用武器的人）
			this, //造成伤害的实际Actor（e.g. 武器）
			UDamageType::StaticClass() //伤害类型，单纯返回一个UDamageType
		);
		ExecuteGetHit(BoxHit);//被击中的目标执行GetHit
		
		CreateFields(BoxHit.ImpactPoint);//创建物理场，击碎物体
	}
}

/// Do Box Trace
/// @param BoxHit BoxTrace的检测结果
void AWeapon::BoxTrace(FHitResult& BoxHit)
{
	/**
	 * 记录起始点和结束点的世界坐标
	 * Ex1：BoxTraceStart->GetRelativeLocation() 这样获取的就是局部坐标
	 */
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();
	
	/**
	 * 进行BoxTrace检测
	 */
	TArray<AActor*> ActorsToIgnore; //忽略的Actor列表
	ActorsToIgnore.Add(this);//忽略武器本身
	ActorsToIgnore.Add(this->GetOwner());//忽略武器的拥有者
	for(AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		BoxTraceExtent,
		BoxTraceStart->GetComponentRotation(),
		TraceTypeQuery1,
		false,
		ActorsToIgnore,
		bShowBoxDebug? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		BoxHit,
		true
		);
	///检测过的Actor就进入忽略列表，避免重复检测
	IgnoreActors.AddUnique(BoxHit.GetActor());
}

void AWeapon::ExecuteGetHit(FHitResult& BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	if(HitInterface)
	{
		HitInterface->Execute_GetHit(BoxHit.GetActor(),BoxHit.ImpactPoint,GetOwner());
		//HitInterface->GetHit(BoxHit.ImpactPoint);
	}
}


///装备武器(Attach Weapon),对外提供的接口
///@param InParent 被Attach的父节点
///@param InSocketName 挂载的Socket名称
///@param NewOwner 武器的拥有者 AActor
///@param NewInstigator 武器的使用者 APawn
void AWeapon::Equip(USceneComponent* InParent, FName InSocketName,AActor* NewOwner, APawn* NewInstigator)
{
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);

	ItemState = EItemState::EIS_Equipped;
	///将物品挂载到指定Socket上
	AttachMeshToSocket(InParent, InSocketName);
	///播放拾取音效
	PlayEquipSound();
	///关闭触发球体的碰撞
	DisableSphereCollision();
	///装备武器之后，关闭Niagara效果
	DisableEmberEffect();
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
	ItemMesh->AttachToComponent(InParent,FAttachmentTransformRules::SnapToTargetIncludingScale, InSocketName);
}

void AWeapon::PlayEquipSound()
{
	if(EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquipSound,
			GetActorLocation()
		);
	}
}

void AWeapon::DisableSphereCollision()
{
	if(SphereComp)
	{
		SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeapon::DisableEmberEffect()
{
	if(ItemEffect)
	{
		ItemEffect->Deactivate();
	}
}

bool AWeapon::ActorIsSameType(AActor* OtherActor)
{
	if(GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy")))
		return true;
	return false;
}