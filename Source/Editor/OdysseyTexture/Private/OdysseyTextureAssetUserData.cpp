// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureAssetUserData.h"
#include "UObject/ObjectSaveContext.h"

#include "EngineUtils.h"

UOdysseyTextureAssetUserData::UOdysseyTextureAssetUserData()
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

FOdysseyLayerStack* UOdysseyTextureAssetUserData::GetOldLayerStack()
{
    return mLayerStack;
}

void UOdysseyTextureAssetUserData::SetOldLayerStack(FOdysseyLayerStack* iLayerStack)
{
    if( !iLayerStack )
        return;

    if( mLayerStack )
        delete mLayerStack;

    mLayerStack = iLayerStack;
}

UOdysseyTextureLayerStack*
UOdysseyTextureAssetUserData::GetLayerStack()
{
    if (!LayerStack)
        LayerStack = UOdysseyTextureLayerStack::CreateFromTexture(GetTexture(), this);
    return LayerStack;
}

UTexture2D*
UOdysseyTextureAssetUserData::GetTexture()
{
    return Cast<UTexture2D>(GetOuter());
}