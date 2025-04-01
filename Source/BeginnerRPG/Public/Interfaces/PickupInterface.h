#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickupInterface.generated.h"


class ATreasure;
// This class does not need to be modified.
//这个class用于接入UE的反射系统
UINTERFACE(MinimalAPI)
class UPickupInterface : public UInterface
{
	GENERATED_BODY()
};

class ASoul;
class AItem;
class BEGINNERRPG_API IPickupInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetOverlappingItem(TObjectPtr<AItem> Item);
	virtual void AddSouls(TObjectPtr<ASoul> Soul);
	virtual void AddGold(TObjectPtr<ATreasure> Treasure);
};
