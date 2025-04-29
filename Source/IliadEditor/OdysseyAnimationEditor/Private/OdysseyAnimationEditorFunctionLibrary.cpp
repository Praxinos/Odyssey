// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationEditorFunctionLibrary.h"
#include "AssetToolsModule.h"
#include "OdysseyAnimationFactory.h"

#define LOCTEXT_NAMESPACE "UOdysseyAnimationEditorFunctionLibrary"

UOdysseyAnimation*
UOdysseyAnimationEditorFunctionLibrary::CreateAnimationAsset(FString AssetName, FString PackagePath, int Width, int Height, EOdysseyAnimationFormat Format, float FramesPerSecond)
{
    if (AssetName.IsEmpty())
        return nullptr;

    if (PackagePath.IsEmpty())
        return nullptr;

    if (Width <= 0 || Height <= 0)
        return nullptr;

    if (FramesPerSecond < 0.f)
        return nullptr;

    IAssetTools& assetTools = FAssetToolsModule::GetModule().Get();
    UOdysseyAnimation* animation = Cast<UOdysseyAnimation>(
        assetTools.CreateAsset(
            AssetName,
            PackagePath,
            UOdysseyAnimation::StaticClass(),
            UOdysseyAnimationFactory::StaticClass()->GetDefaultObject<UFactory>()
        )
    );

    if (!animation)
        return nullptr;

    animation->Init(Width, Height, Format, FramesPerSecond);

    return animation;
}

#undef LOCTEXT_NAMESPACE
