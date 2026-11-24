// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#include "OdysseyAnimationExport.h"

#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "Materials/MaterialInterface.h"
#include "Misc/ScopedSlowTask.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookFactory.h"
#include "PaperSprite.h"
#include "PaperSpriteFactory.h"

#include "OdysseyAnimationRenderContext.h"
#include "OdysseyExportImage.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationExport"
namespace Odyssey
{
UPaperFlipbook*
ExportAsFlipbook(UObject* iObject, const FInt32Range& iRange, float iFramesPerSecond, FString AssetName, FString Path)
{
    if ( Path.IsEmpty())
        return nullptr;

    IOdysseyTextureRenderingAbility* textureRenderingAbility = Cast<IOdysseyTextureRenderingAbility>(iObject);
    if (!textureRenderingAbility)
        return nullptr;

    int startFrame = iRange.GetLowerBoundValue();
    int endFrame = iRange.GetUpperBoundValue();
    FString endFrameStr = FString::FromInt(endFrame);

    FScopedSlowTask progressBar(endFrame - startFrame + 1, LOCTEXT("image-rendering-ability.export-as-flipbook.progress-bar.title", "Export As Flipbook"));
    progressBar.MakeDialog();

    // Create flipbook asset
    FString flipbookAssetName = AssetName;
    FString flipbookPackagePath = Path;

    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    UPaperFlipbook* flipbook = Cast<UPaperFlipbook>(AssetTools.CreateAsset(
        flipbookAssetName,
        flipbookPackagePath,
        UPaperFlipbook::StaticClass(),
        UPaperFlipbookFactory::StaticClass()->GetDefaultObject<UFactory>()
    ));

    //sprite factory is not easily accessible because not exported through PAPER2D_API
    UFactory* spriteFactory = *AssetTools.GetNewAssetFactories().FindByPredicate(
        [](UFactory* iFactory)
        {
            return iFactory->DoesSupportClass(UPaperSprite::StaticClass());
        }
    );

    FScopedFlipbookMutator flipbookMutator(flipbook);
    flipbookMutator.FramesPerSecond = iFramesPerSecond;

    TArray<FGuid> lastRenderingComposition;

    int lastKeyFrameIndex = -1;
    int lastKeyFrameFirstFrame = -1;
    for (int i = startFrame; i <= endFrame; i++)
    {
        progressBar.EnterProgressFrame();

        TArray<FGuid> renderingComposition = textureRenderingAbility->GetRenderingComposition(EOdysseyRenderingType::Render, i);
        if (renderingComposition == lastRenderingComposition)
            continue;

        lastRenderingComposition = renderingComposition;

        if (!flipbookMutator.KeyFrames.IsEmpty())
            flipbookMutator.KeyFrames.Last().FrameRun = i - lastKeyFrameFirstFrame;

        FString numStr = FString::Format(TEXT("{0}"), {i});
        FString textureName = flipbookAssetName + TEXT("_Texture_") + numStr;
        FString spriteName = flipbookAssetName + TEXT("_Sprite_") + numStr;

        lastKeyFrameFirstFrame = i;

        UPaperSprite* sprite = Cast<UPaperSprite>(AssetTools.CreateAsset(
            spriteName,
            flipbookPackagePath,
            UPaperSprite::StaticClass(),
            spriteFactory
        ));

        FOdysseyRenderContext RenderContext = FOdysseyAnimationRenderContext::MakeForFrame(FFrameNumber(i));
        UTexture2D* texture = ExportAsTexture(iObject, &RenderContext, textureName, flipbookPackagePath);
        FOdysseyObjectEditorUtils::SetPropertyValue(sprite, "SourceTexture", TSoftObjectPtr<UTexture2D>(texture));

        FPaperFlipbookKeyFrame keyframe;
        keyframe.Sprite = sprite;
        flipbookMutator.KeyFrames.Add(keyframe);
    }

    if (!flipbookMutator.KeyFrames.IsEmpty())
            flipbookMutator.KeyFrames.Last().FrameRun = endFrame - lastKeyFrameFirstFrame + 1;

    //Configure flipbook asset
    UMaterialInterface* material = LoadObject<UMaterialInterface>(nullptr, TEXT("/Odyssey/Animation2D/DefaultFlipbookMaterialInstance.DefaultFlipbookMaterialInstance"));
    FOdysseyObjectEditorUtils::SetPropertyValue(flipbook, "DefaultMaterial", material);

    return flipbook;
}

TArray<UTexture2D*>
ExportAsTextureSequence(UObject* iObject, const FInt32Range& iRange, FString AssetName, FString Path)
{
    if ( Path.IsEmpty())
        return {};

    IOdysseyTextureRenderingAbility* textureRenderingAbility = Cast<IOdysseyTextureRenderingAbility>(iObject);
    if (!textureRenderingAbility)
        return {};

    int startFrame = iRange.GetLowerBoundValue();
    int endFrame = iRange.GetUpperBoundValue();
    FString endFrameStr = FString::FromInt(endFrame);

    FScopedSlowTask progressBar(endFrame - startFrame + 1, LOCTEXT("image-rendering-ability.export-as-texture-sequence.progress-bar.title", "Export As Texture Sequence"));
    progressBar.MakeDialog();

    TArray<FGuid> lastRenderingComposition;

    TArray<UTexture2D*> textures;

    for (int i = startFrame; i <= endFrame; i++)
    {
        progressBar.EnterProgressFrame();

        TArray<FGuid> renderingComposition = textureRenderingAbility->GetRenderingComposition(EOdysseyRenderingType::Render, i);
        if (renderingComposition == lastRenderingComposition)
            continue;

        lastRenderingComposition = renderingComposition;
        FString numStr = FString::Format(TEXT("{0}"), {i});
        FString textureName = AssetName + TEXT("_") + numStr;

        FOdysseyRenderContext RenderContext = FOdysseyAnimationRenderContext::MakeForFrame(FFrameNumber(i));
        UTexture2D* texture = ExportAsTexture(iObject, &RenderContext, textureName, Path);

        textures.Add(texture);
    }

    return textures;
}

TArray<FString>
ExportAsImageSequence(
    UObject* iObject,
    const FInt32Range& iRange,
    FString Filename,
    FString Path,
    EOdysseyExportImageFormat Format
)
{
    if ( Path.IsEmpty())
        return {};

    IOdysseyTextureRenderingAbility* textureRenderingAbility = Cast<IOdysseyTextureRenderingAbility>(iObject);
    if (!textureRenderingAbility)
        return {};

    int startFrame = iRange.GetLowerBoundValue();
    int endFrame = iRange.GetUpperBoundValue();

    FScopedSlowTask progressBar(endFrame - startFrame + 1, LOCTEXT("image-rendering-ability.export-as-texture-sequence.progress-bar.title", "Export As Texture Sequence"));
    progressBar.MakeDialog();

    TArray<FGuid> lastRenderingComposition;

    FString endFrameStr = FString::FromInt(endFrame);
    TArray<FString> paths;

    for (int i = startFrame; i <= endFrame; i++)
    {
        progressBar.EnterProgressFrame();

        TArray<FGuid> renderingComposition = textureRenderingAbility->GetRenderingComposition(EOdysseyRenderingType::Render, i);
        if (renderingComposition == lastRenderingComposition)
            continue;

        lastRenderingComposition = renderingComposition;

        FString frameStr = FString::FromInt(i);
        FString filename = Filename + TEXT("_");
         for (int j = 0; j < endFrameStr.Len() - frameStr.Len(); j++)
        {
            filename += TEXT("0");
        }
        filename += FString::Printf(TEXT("%d"), i);

        FOdysseyRenderContext RenderContext = FOdysseyAnimationRenderContext::MakeForFrame(FFrameNumber(i));
        FString fullpath = ExportAsImage(iObject, &RenderContext, Format, filename, Path );

        paths.Add(fullpath);
    }

    return paths;
}

}
#undef LOCTEXT_NAMESPACE
