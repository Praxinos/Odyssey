// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyTextureAssetUserData.h"

#include "EngineUtils.h"

UOdysseyTextureAssetUserData::UOdysseyTextureAssetUserData(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    mLayerStack = new FOdysseyLayerStack();
}


//-------------------------
//UObject Interface--------
void UOdysseyTextureAssetUserData::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);
    Ar << mLayerStack;
}
//-------------------------
//End UObject Interface----

FOdysseyLayerStack* UOdysseyTextureAssetUserData::GetLayerStack()
{
    return mLayerStack;
}

