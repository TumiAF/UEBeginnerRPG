// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "Bird.generated.h"


//Forward Declarations
class UCapsuleComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class BEGINNERRPG_API ABird : public APawn
{
	GENERATED_BODY()
public:
	ABird();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;
	void MoveForward(float Value);
	void Turn(float Value); //Set Controller Rotator - Yaw
	void LookUp(float Value); //Set Controller Rotator - Pitch
;
private:
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* Capsule;
	
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* BirdMesh;
	
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;
};
