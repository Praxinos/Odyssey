// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTextureLayerStackUserData.h"

#include "EngineUtils.h"
#include "Misc/FeedbackContext.h"
#include "UObject/ObjectSaveContext.h"
#include "UObject/SavePackage.h"

#include "OdysseyTextureLayerImageVector.h"

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
UOdysseyTextureLayerStackUserData::InitWithEmptyVectorLayer()
{
    if (LayerStack)
        return;

    LayerStack = UOdysseyTextureLayerStack::CreateWithEmptyVectorLayer(GetTexture(), this);
}

void
UOdysseyTextureLayerStackUserData::InitWithEmptyRasterLayer()
{
    if (LayerStack)
        return;

    LayerStack = UOdysseyTextureLayerStack::CreateWithEmptyRasterLayer(GetTexture(), this);
}

void
UOdysseyTextureLayerStackUserData::InitWithDuplicateLayerStack(UOdysseyTextureLayerStack* iLayerStack)
{
    if ( LayerStack )
        return;

    if ( !iLayerStack )
        return;

    LayerStack = UOdysseyTextureLayerStack::CreateEmptyFromTexture(GetTexture(), this);
    LayerStack->CopyLayers(iLayerStack->GetRootLayers(), LayerStack->GetLayerRoot());
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

#if WITH_EDITOR
void
UOdysseyTextureLayerStackUserData::OnRefreshReferencedPalette(UOdysseyPalette* iPalette)
{
    UTexture* texture = GetTexture();

    UPackage* package = texture->GetOutermost();
    FSavePackageArgs saveArgs;
    saveArgs.TopLevelFlags = RF_Standalone;
    saveArgs.Error = GWarn;
    saveArgs.SaveFlags = SAVE_NoError;
    FString packageFilename = FPackageName::LongPackageNameToFilename(package->GetName(), FPackageName::GetAssetPackageExtension());
    UPackage::SavePackage(package, this, *packageFilename, saveArgs);
    FlushAsyncLoading();

    TArray<UOdysseyLayer*> layers = GetLayerStack()->GetLayers();
    for (UOdysseyLayer* layer : layers)
    {
        if (!layer->IsA(UOdysseyTextureLayerImageVector::StaticClass()))
            continue;

        UOdysseyTextureLayerImageVector* vectorLayer = Cast<UOdysseyTextureLayerImageVector>(layer);
        if (!vectorLayer)
            continue;

        vectorLayer->OnRefreshReferencedPalette(iPalette);
    }
}
#endif
