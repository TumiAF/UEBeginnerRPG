#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Treasure.h"


ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;
	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);//设置生成重叠事件 = true
}


void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
}


void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint,AActor* Hitter)
{
	TObjectPtr<UWorld> World = GetWorld();
	if(World && TreasureClasses.Num() > 0 )
	{
		FVector Location = GetActorLocation();
		Location.Z += 75.f;
		///从宝物数组中随机一个宝物进行生成
		const int32 Selection = FMath::RandRange(0,TreasureClasses.Num()-1);
		World->SpawnActor<ATreasure>(TreasureClasses[Selection],Location,GetActorRotation());
	}
}

