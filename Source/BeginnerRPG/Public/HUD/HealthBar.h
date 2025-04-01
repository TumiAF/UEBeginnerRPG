#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

/**
 * 
 */
class UProgressBar;
UCLASS()
class BEGINNERRPG_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta=(BindWidget)) //和WBP_HealthBar中的组件进行绑定，注意名称要匹配
	TObjectPtr<UProgressBar> HealthBar;
	
};
