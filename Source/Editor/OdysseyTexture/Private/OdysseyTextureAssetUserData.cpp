// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureAssetUserData.h"

#include "EngineUtils.h"

UOdysseyTextureAssetUserData::UOdysseyTextureAssetUserData(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    //mLayerStack = new FOdysseyLayerStack(texture->Source.GetSizeX(), texture->Source.GetSizeY(), ULISFormatForUE4TextureSourceFormat(texture->Source.GetFormat()));
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

