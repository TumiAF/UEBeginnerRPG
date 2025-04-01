// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/Bird.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ABird::ABird()
{
 	PrimaryActorTick.bCanEverTick = true;
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetCapsuleHalfHeight(20.f);
	Capsule->SetCapsuleRadius(15.f);
	SetRootComponent(Capsule);

	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
	BirdMesh->SetupAttachment(GetRootComponent());

	//Create SpringArm
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;
	//Create Camera
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}


void ABird::BeginPlay()
{
	Super::BeginPlay();
	
}


void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//传入函数的地址
	PlayerInputComponent->BindAxis(FName("MoveForward"),this, &ABird::MoveForward);
	PlayerInputComponent->BindAxis(FName("Turn"),this, &ABird::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"),this,&ABird::LookUp);
}

//Set Move Forward
void ABird::MoveForward(float Value)
{
	if(Controller && Value != 0.f)
	{
		FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward,Value);
	}
}

//Set Controller Rotator - Yaw
void ABird::Turn(float Value)
{
	if(Controller&& Value != 0.f)
	{
		AddControllerYawInput(Value);
	}
}

//Set Controller Rotator - Pitch
void ABird::LookUp(float Value)
{
	if(Controller&& Value != 0.f)
	{
		AddControllerPitchInput(Value);
	}
}

