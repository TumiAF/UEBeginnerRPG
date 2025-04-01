// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "Items/Weapons/Weapon.h"

// Sets default values
ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//Set All Controller Rotation false
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);

	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	//Create SpringArm
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;
	//Create Camera
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	//Hair Mesh
	HairMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HairMesh"));
	HairMesh->SetupAttachment(GetMesh());
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Tags.Add(FName("SlashCharacter"));
	InitializeSlashOverlay();
}

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(Attribute && SlashOverlay)
	{
		Attribute->RegenerateStamina(DeltaTime);
		SlashOverlay->SetStaminaBarPercent(Attribute->GetStaminaPercent());
	}
}

///初始化SlashOverlay
void ASlashCharacter::InitializeSlashOverlay()
{
	TObjectPtr<APlayerController> PlayerController = Cast<APlayerController>(GetController());
	if(PlayerController)
	{
		TObjectPtr<ASlashHUD> SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD());
		if(SlashHUD)
		{
			SlashOverlay = SlashHUD->GetSlashOverlay();
			if(SlashOverlay && Attribute)
			{
				SlashOverlay->SetHealthBarPercent(Attribute->GetHealthPercent());
				SlashOverlay->SetStaminaBarPercent(1.f);
				SlashOverlay->SetGold(0);
				SlashOverlay->SetSouls(0);
			}
		}
	}
}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                  AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();
	return DamageAmount;
}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint,AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint,Hitter);
	
	SetWeaponCollision(ECollisionEnabled::NoCollision);
	if(Attribute && Attribute->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}
	else
	{
		ActionState = EActionState::EAS_Dead;
	}
}

void ASlashCharacter::SetOverlappingItem(TObjectPtr<AItem> Item)
{
	OverlappingItem = Item;
}

void ASlashCharacter::AddSouls(TObjectPtr<ASoul> Soul)
{
	if(Attribute && SlashOverlay)
	{
		Attribute->AddSouls(Soul->GetSouls());
		SlashOverlay->SetSouls(Attribute->GetSouls());
	}
}

void ASlashCharacter::AddGold(TObjectPtr<ATreasure> Treasure)
{
	if(Attribute && SlashOverlay)
	{
		Attribute->AddGold(Treasure->GetGold());
		SlashOverlay->SetGold(Attribute->GetGold());
	}
}

// Called to bind functionality to input
void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ASlashCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ASlashCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &ASlashCharacter::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ASlashCharacter::LookUp);
	
	PlayerInputComponent->BindAction(FName("Jump"),EInputEvent::IE_Pressed, this, &ASlashCharacter::Jump);
	PlayerInputComponent->BindAction(FName("Equip"),EInputEvent::IE_Pressed, this, &ASlashCharacter::EKeyPressed);
	PlayerInputComponent->BindAction(FName("Attack"),EInputEvent::IE_Pressed, this, &ASlashCharacter::Attack);
	PlayerInputComponent->BindAction(FName("Dodge"),EInputEvent::IE_Pressed, this, &ASlashCharacter::Dodge);
}



void ASlashCharacter::Jump()
{
	if(IsUnoccupied())
	{
		Super::Jump();
	}
}

void ASlashCharacter::MoveForward(float Value)
{
	if(ActionState != EActionState::EAS_Unoccupied) return;
	if(Controller && Value!=0.0f)
	{
		const FRotator ControllerRotation = GetControlRotation();
		//只在意 Yaw
		const FRotator YawRotation(0.f,ControllerRotation.Yaw, 0.f);

		//1、FRotationMatrix(YawRotation)：通过YawRotation创建一个旋转矩阵
		//2、GetUnitAxis(EAxis::X)：通过旋转矩阵获取X轴的单位向量
		//3、X轴分量即为正方向
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ASlashCharacter::MoveRight(float Value)
{
	if(ActionState != EActionState::EAS_Unoccupied) return;
	if(Controller && Value!=0.0f)
	{
		FRotator ControllerRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControllerRotation.Yaw, 0.f);

		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
	}
}

void ASlashCharacter::Turn(float Value)
{
	if(Controller&& Value != 0.f)
	{
		AddControllerYawInput(Value);
	}
}

void ASlashCharacter::LookUp(float Value)
{
	if(Controller&& Value != 0.f)
	{
		AddControllerPitchInput(Value);
	}
}

/// ActionMapping - 键盘按键E按下之后的回调函数
/// 拾取武器
void ASlashCharacter::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	TakeWeapon(OverlappingWeapon);
}

void ASlashCharacter::Attack()
{
	Super::Attack();
	if(CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
	
}

void ASlashCharacter::Dodge()
{
	if(!IsUnoccupied() || !HasEnoughStamina()) return;
	
	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodge;
	if(Attribute && SlashOverlay)
	{
		Attribute->UseStamina(Attribute->GetDodgeCost());
		SlashOverlay->SetStaminaBarPercent(Attribute->GetStaminaPercent());
	}
}

void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName,EquipMontage);
	}
}
void ASlashCharacter::DodgeEnd()
{
	Super::DodgeEnd();
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

void ASlashCharacter::Die_Implementation()
{
	Super::Die_Implementation();
	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

bool ASlashCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
			CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped &&
			EquipWeapon;
}

void ASlashCharacter::AttachWeaponToBack()
{
	if(EquipWeapon)
	{
		EquipWeapon->AttachMeshToSocket(GetMesh(),FName("SpineSocket"));
	}
}

void ASlashCharacter::AttachWeaponToHand()
{
	if(EquipWeapon)
	{
		EquipWeapon->AttachMeshToSocket(GetMesh(),FName("RightHandSocket"));
	}
}

void ASlashCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::TakeWeapon(TObjectPtr<AWeapon> Weapon)
{
	if(Weapon) //空手，捡取武器
	{
		Weapon->Equip(GetMesh(),FName("RightHandSocket"),this,this);
		CharacterState = ECharacterState::ECS_EquippedOneHandWeapon;
		OverlappingItem = nullptr; //重置Item，避免重复赋值Weapon
		EquipWeapon = Weapon;
	}
	else
	{
		if(CanDisarm())
		{
			PlayEquipMontage(FName("Unequip"));
			CharacterState = ECharacterState::ECS_Unequipped;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
		else if(CanArm())
		{
			PlayEquipMontage(FName("Equip"));
			CharacterState = ECharacterState::ECS_EquippedOneHandWeapon;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
	}
}

void ASlashCharacter::SetHUDHealth()
{
	if(SlashOverlay && Attribute)
	{
		SlashOverlay->SetHealthBarPercent(Attribute->GetHealthPercent());
	}
}

bool ASlashCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::HasEnoughStamina()
{
	return Attribute && Attribute->GetStamina() > Attribute->GetDodgeCost();
}
