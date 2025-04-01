// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterType.h"
#include "SlashAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BEGINNERRPG_API USlashAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
	class ASlashCharacter* SlashCharacter;

	UPROPERTY(BlueprintReadOnly, Category="Tumi Movement")
	class UCharacterMovementComponent* SlashMovement;

	UPROPERTY(BlueprintReadOnly, Category="Tumi Movement")
	float GroundSpeed;
	
	UPROPERTY(BlueprintReadOnly, Category="Tumi Movement")
	bool IsFalling;

	UPROPERTY(BlueprintReadOnly, Category="Tumi Movement")
	ECharacterState CharacterState;

	UPROPERTY(BlueprintReadOnly, Category="Tumi Movement")
	EActionState ActionState;
	
	UPROPERTY(BlueprintReadOnly, Category="Tumi Movement")
	TEnumAsByte<EDeathPose>  DeathPose;
};
