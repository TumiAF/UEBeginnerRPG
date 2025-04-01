#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HitInterface.generated.h"

// This class does not need to be modified.
//这个class用于接入UE的反射系统
UINTERFACE(MinimalAPI)
class UHitInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BEGINNERRPG_API IHitInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	///BlueprintNativeEvent：
	///（功能之一）UE自动将这个函数变为virtual function
	///（功能之二）允许蓝图和C++都可以实现这个函数
	UFUNCTION(BlueprintNativeEvent)
	void GetHit(const FVector& ImpactPoint,AActor* Hitter);
};
