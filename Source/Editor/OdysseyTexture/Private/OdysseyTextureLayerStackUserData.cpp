// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureLayerStackUserData.h"
#include "UObject/ObjectSaveContext.h"

#include "EngineUtils.h"

//-------------------------
//End UObject Interface----

void
UOdysseyTextureLayerStackUserData::InitWithEmptyLayerStack()
{
    if (LayerStack)
        return;
    LayerStack = UOdysseyTextureLayerStack::CreateEmptyFromTexture(GetTexture(), this);
}

void
UOdysseyTextureLayerStackUserData::InitWithDefaultLayerStack()
{
    if (LayerStack)
        return;
    LayerStack = UOdysseyTextureLayerStack::CreateFromTexture(GetTexture(), this);
}

void
UOdysseyTextureLayerStackUserData::InitWithDuplicateLayerStack(UOdysseyTextureLayerStack* iLayerStack)
{
    if ( LayerStack )
        return;

    if ( !iLayerStack )
        return;

    LayerStack = UOdysseyTextureLayerStack::CreateEmptyFromTexture(GetTexture(), this);
    LayerStack->CopyLayers(iLayerStack->GetRootLayers(), LayerStack->LayerRoot);
}

UOdysseyTextureLayerStack*
UOdysseyTextureLayerStackUserData::GetLayerStack()
{
    if (!LayerStack)
        InitWithDefaultLayerStack();
    return LayerStack;
}

UTexture2D*
UOdysseyTextureLayerStackUserData::GetTexture()
{
    return Cast<UTexture2D>(GetOuter());
}