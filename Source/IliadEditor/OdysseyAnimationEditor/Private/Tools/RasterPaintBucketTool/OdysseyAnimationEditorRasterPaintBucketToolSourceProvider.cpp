// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Tools/RasterPaintBucketTool/OdysseyAnimationEditorRasterPaintBucketToolSourceProvider.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "OdysseyAnimation.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "ULISLoaderModule.h"

FOdysseyAnimationEditorRasterPaintBucketToolSourceProvider::~FOdysseyAnimationEditorRasterPaintBucketToolSourceProvider()
{
}

FOdysseyAnimationEditorRasterPaintBucketToolSourceProvider::FOdysseyAnimationEditorRasterPaintBucketToolSourceProvider(FOdysseyAnimationEditorExtension* iExtension)
    : mExtension(iExtension)
{
}

const TArray<FString>&
FOdysseyAnimationEditorRasterPaintBucketToolSourceProvider::GetSources() const
{
    return mSources;
}

TSharedPtr<::ULIS::FBlock>
FOdysseyAnimationEditorRasterPaintBucketToolSourceProvider::GetSourceBlock(EOdysseyRasterPaintBucketToolSource iSource) const
{
    switch(iSource)
    {
        //Current Layer
        case EOdysseyRasterPaintBucketToolSource::CurrentLayer :
            return GetCurrentLayerBlock();
        //Foreground Layers
        case EOdysseyRasterPaintBucketToolSource::ForegroundLayers :
            return GetForegroundLayersBlock();
        //Background Layers
        case EOdysseyRasterPaintBucketToolSource::BackgroundLayers :
            return GetBackgroundLayersBlock();
        //All Layers
        case EOdysseyRasterPaintBucketToolSource::AllLayers :
            return GetAllLayersBlock();
    }

    return nullptr;
}

TSharedPtr<::ULIS::FBlock>
FOdysseyAnimationEditorRasterPaintBucketToolSourceProvider::GetCurrentLayerBlock() const
{
    UOdysseyAnimation* animation = mExtension->Animation();
    if (!animation)
        return nullptr;

    UOdysseyAnimationLayerStack* layerStack = mExtension->LayerStack();
    if (!layerStack)
        return nullptr;

    TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(animation->GetWidth(), animation->GetHeight(), animation->GetFormat());
    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
    TSharedPtr<IOdysseyImageRenderer> renderer = currentLayer->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, animation->CurrentFrame);
    renderer->Init();

    FOdysseyImageRendererCopyParams params(block, { block->Rect() });
    renderer->Copy(params, {});

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(animation->GetFormat());
    ctx.Finish();

    return block;
}

TSharedPtr<::ULIS::FBlock>
FOdysseyAnimationEditorRasterPaintBucketToolSourceProvider::GetForegroundLayersBlock() const
{
    UOdysseyAnimation* animation = mExtension->Animation();
    if (!animation)
        return nullptr;

    UOdysseyAnimationLayerStack* layerStack = mExtension->LayerStack();
    if (!layerStack)
        return nullptr;

    TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(animation->GetWidth(), animation->GetHeight(), animation->GetFormat());
    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
    TArray<FOdysseyImageRenderingAbility*> layersToExclude = GetForegroundLayersToExclude(currentLayer);
    FImageRendererFilter filter = FImageRendererFilter::CreateLambda(
        [layersToExclude](const FOdysseyImageRenderingAbility* iRenderingAbility)
        {
            return !layersToExclude.Contains(iRenderingAbility);
        }
    );

    TSharedPtr<IOdysseyImageRenderer> renderer = animation->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, animation->CurrentFrame, filter);
    if (!renderer)
        return nullptr;

    renderer->Init();

    FOdysseyImageRendererCopyParams params(block, { block->Rect() });
    renderer->Copy(params, {});

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(animation->GetFormat());
    ctx.Finish();

    return block;
}

TSharedPtr<::ULIS::FBlock>
FOdysseyAnimationEditorRasterPaintBucketToolSourceProvider::GetBackgroundLayersBlock() const
{
    UOdysseyAnimation* animation = mExtension->Animation();
    if (!animation)
        return nullptr;

    UOdysseyAnimationLayerStack* layerStack = mExtension->LayerStack();
    if (!layerStack)
        return nullptr;

    TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(animation->GetWidth(), animation->GetHeight(), animation->GetFormat());
    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
    TArray<FOdysseyImageRenderingAbility*> layersToExclude = GetBackgroundLayersToExclude(currentLayer);
    FImageRendererFilter filter = FImageRendererFilter::CreateLambda(
        [layersToExclude](const FOdysseyImageRenderingAbility* iRenderingAbility) -> bool
        {
            return !layersToExclude.Contains(iRenderingAbility);
        }
    );

    TSharedPtr<IOdysseyImageRenderer> renderer = animation->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, animation->CurrentFrame, filter);
    if (!renderer)
        return nullptr;

    renderer->Init();
    FOdysseyImageRendererCopyParams params(block, { block->Rect() });
    renderer->Copy(params, {});

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(animation->GetFormat());
    ctx.Finish();

    return block;
}

TSharedPtr<::ULIS::FBlock>
FOdysseyAnimationEditorRasterPaintBucketToolSourceProvider::GetAllLayersBlock() const
{
    UOdysseyAnimation* animation = mExtension->Animation();
    if (!animation)
        return nullptr;

    TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(animation->GetWidth(), animation->GetHeight(), animation->GetFormat());
    TSharedPtr<IOdysseyImageRenderer> renderer = animation->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, animation->CurrentFrame);
    renderer->Init();
    FOdysseyImageRendererCopyParams params(block, { block->Rect() });
    renderer->Copy(params, {});

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(animation->GetFormat());
    ctx.Finish();

    return block;
}

TArray<FOdysseyImageRenderingAbility*>
FOdysseyAnimationEditorRasterPaintBucketToolSourceProvider::GetBackgroundLayersToExclude(UOdysseyLayer* iLayer) const
{
    TArray<FOdysseyImageRenderingAbility*> resultLayers;

    UOdysseyAnimationLayerStack* layerStack = mExtension->LayerStack();
    if (!layerStack)
        return resultLayers;

    TArray<UOdysseyLayer*> layers = layerStack->GetLayers();
    for( int i = 0; i < layers.Num(); i++ )
    {
        if (iLayer->IsChildOf(layers[i]) && (iLayer->GetParent() != layers[i] || iLayer->GetIndexInParent() != layers[i]->GetChildren().Num() - 1))
            continue;

        if (iLayer->IsChildOf(layers[i]))
            continue;

        resultLayers.Add(Cast<UOdysseyAnimationLayer>(layers[i]));

        if (layers[i] == iLayer)
            break;
    }

    return resultLayers;
}

TArray<FOdysseyImageRenderingAbility*>
FOdysseyAnimationEditorRasterPaintBucketToolSourceProvider::GetForegroundLayersToExclude(UOdysseyLayer* iLayer) const
{
    TArray<FOdysseyImageRenderingAbility*> resultLayers;

    UOdysseyAnimationLayerStack* layerStack = mExtension->LayerStack();
    if (!layerStack)
        return resultLayers;

    TArray<UOdysseyLayer*> layers = layerStack->GetLayers();

    for( int i = layers.Num() - 1; i >= 0; i-- )
    {
        resultLayers.Add(Cast<UOdysseyAnimationLayer>(layers[i]));

        if (layers[i] == iLayer)
            break;
    }

    return resultLayers;
}
