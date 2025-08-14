// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationEditorFunctionLibrary.h"
#include "AssetToolsModule.h"
#include "OdysseyAnimationFactory.h"
#include "OdysseyAnimationLayerImageRaster.h"

#define LOCTEXT_NAMESPACE "UOdysseyAnimationEditorFunctionLibrary"

UOdysseyAnimation*
UOdysseyAnimationEditorFunctionLibrary::CreateAnimationAsset(FString AssetName, FString PackagePath, int Width, int Height, EOdysseyAnimationFormat Format, float FramesPerSecond, TSubclassOf<class UOdysseyAnimationLayer> DefaultLayerClass)
{
    if (AssetName.IsEmpty())
        return nullptr;

    if (PackagePath.IsEmpty())
        return nullptr;

    if (Width <= 0 || Height <= 0)
        return nullptr;

    if (FramesPerSecond < 0.f)
        return nullptr;

    UOdysseyAnimationFactory* factory = NewObject<UOdysseyAnimationFactory>();
    factory->Width = Width;
    factory->Height = Height;
    factory->Format = Format;
    factory->FrameRate = FFrameRate( FramesPerSecond, 1.f );
    factory->DefaultLayerClass = DefaultLayerClass.Get() ? DefaultLayerClass.Get() : UOdysseyAnimationLayerImageRaster::StaticClass();

    IAssetTools& assetTools = FAssetToolsModule::GetModule().Get();
    UOdysseyAnimation* animation = Cast<UOdysseyAnimation>(
        assetTools.CreateAsset(
            AssetName,
            PackagePath,
            UOdysseyAnimation::StaticClass(),
            factory
        )
    );

    return animation;
}

#undef LOCTEXT_NAMESPACE
