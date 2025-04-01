
#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "GameFramework/Character.h"
#include "CharacterType.h"
#include "Interfaces/PickupInterface.h"
#include "SlashCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class AItem;
class ASoul;
class UAnimMontage;
class USlashOverlay;

UCLASS()
class BEGINNERRPG_API ASlashCharacter : public ABaseCharacter,public IPickupInterface
{
	GENERATED_BODY()

public:
	ASlashCharacter();
	
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void Jump() override;

	///获取Character State
	FORCEINLINE ECharacterState GetCharacterState() const {return CharacterState;}

	FORCEINLINE EActionState GetActionState() const {return ActionState;}

	/** <AActor> */
	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;
	/** </AActor> */

	/** <IHitInterface> */
	virtual void GetHit_Implementation(const FVector& ImpactPoint,AActor* Hitter) override;
	/** </IHitInterface> */

	/** <IPickupInterface> */
	virtual void SetOverlappingItem(TObjectPtr<AItem> Item) override;
	virtual void AddSouls(TObjectPtr<ASoul> Soul) override;
	virtual void AddGold(TObjectPtr<ATreasure> Treasure) override;
	/** </IPickupInterface> */
	
protected:
	virtual void BeginPlay() override;

	virtual void DodgeEnd() override;
	///Attack攻击结束，重置ActionState \n
	///BlueprintCallable：可以在蓝图中调用该函数； \n
	virtual void AttackEnd() override;

	///判断当前是否允许攻击
	virtual bool CanAttack() override;

	/// <ABaseCharacter>
	virtual void Die_Implementation() override;

	/// </ABaseCharacter>

	/// *** 输入绑定的回调函数 ***
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	
	//键盘E 按下之后执行的回调；如：捡取物品
	void EKeyPressed();
	//鼠标左键 回调函数 攻击
	virtual void Attack() override;

	///鼠标右键 翻滚
	void Dodge();

	/// *** End ***
	
	///播放Equip Montage
	///@param SectionName 要播放的蒙太奇片段名称（FName 类型标识）
	void PlayEquipMontage(const FName& SectionName);
	
	//判断当前是否允许解除武装
	bool CanDisarm();
	//判断当前是否允许装备武器
	bool CanArm();

	///解除装备，即把武器装到背上（在这里把武器Attach到背上的Socket）
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();
	///装备武器，即把武器装到手上（在这里把武器重新Attach到手上的Socket）
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	///完成装备武器的动作，重置ActionState回Unoccupied
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	///受击结束
	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	void TakeWeapon(TObjectPtr<AWeapon> Weapon);
private:
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	///角色行为状态 \n
	///BlueprintReadWrite：允许在蓝图中对该属性进行读写操作； \n
	///因为是private属性，所以不能进行写入操作，通过meta操作来运行访问； \n
	UPROPERTY(BlueprintReadWrite,meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;
	
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* HairMesh;
	
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;//实时刷新和Character产生重叠的Item对象
	
	///装备or卸下Sword的Montage
	UPROPERTY(EditDefaultsOnly,Category= "Tumi Montages" )
	UAnimMontage* EquipMontage;

	///角色主面板UI <UUserWidget>
	UPROPERTY()
	TObjectPtr<USlashOverlay> SlashOverlay;

	
	void InitializeSlashOverlay();
	void SetHUDHealth();
	bool IsUnoccupied();
	bool HasEnoughStamina();
};
