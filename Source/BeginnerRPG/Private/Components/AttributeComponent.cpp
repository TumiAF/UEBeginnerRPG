
#include "Components/AttributeComponent.h"


UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

bool UAttributeComponent::IsAlive()
{
	return Health > 0.f;
}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health-Damage,0.f,MaxHealth);
}

void UAttributeComponent::UseStamina(float StaminaCost)
{
	Stamina = FMath::Clamp(Stamina - StaminaCost,0.f,MaxStamina);
}

void UAttributeComponent::RegenerateStamina(float DeltaTime)
{
	if(Stamina < MaxStamina)
	{
		Stamina = FMath::Clamp(Stamina + StaminaRegenerateRate*DeltaTime,0.f,MaxStamina);
	}
}

float UAttributeComponent::GetHealthPercent()
{
	return Health / MaxHealth;
}

float UAttributeComponent::GetStaminaPercent()
{
	return Stamina / MaxStamina;
}

void UAttributeComponent::AddSouls(int32 NumberOfSouls)
{
	Souls+=NumberOfSouls;
}

void UAttributeComponent::AddGold(int32 NumberOfGold)
{
	Gold+=NumberOfGold;
}

