// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "ActorFactories/ActorFactory.h"
#include "OdysseyAnimationActorFactory.generated.h"

class AActor;
struct FAssetData;

UCLASS()
class UOdysseyAnimationActorFactory : public UActorFactory
{
    GENERATED_UCLASS_BODY()

    //~ Begin UActorFactory Interface
    virtual void PostSpawnActor(UObject* Asset, AActor* NewActor) override;
    virtual bool CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg) override;
    //~ End UActorFactory Interface
};
