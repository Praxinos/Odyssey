// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Tools/RasterPaintBucketTool/OdysseyTextureEditorRasterPaintBucketToolSourceProvider.h"
#include "TextureEditor/OdysseyTextureEditorExtension.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "TextureEditor/OdysseyTextureEditorSource.h"
#include "OdysseyPixelFormat.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "ULISLoaderModule.h"

FOdysseyTextureEditorRasterPaintBucketToolSourceProvider::~FOdysseyTextureEditorRasterPaintBucketToolSourceProvider()
{
}

FOdysseyTextureEditorRasterPaintBucketToolSourceProvider::FOdysseyTextureEditorRasterPaintBucketToolSourceProvider(FOdysseyTextureEditorExtension* iExtension)
    : mExtension(iExtension)
{
}

const TArray<FString>&
FOdysseyTextureEditorRasterPaintBucketToolSourceProvider::GetSources() const
{
    return mSources;
}

TSharedPtr<::ULIS::FBlock>
FOdysseyTextureEditorRasterPaintBucketToolSourceProvider::GetSourceBlock(EOdysseyRasterPaintBucketToolSource iSource) const
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
FOdysseyTextureEditorRasterPaintBucketToolSourceProvider::GetCurrentLayerBlock() const
{
    UTexture2D* texture = mExtension->Texture();
    if (!texture)
        return nullptr;

    UOdysseyTextureLayerStack* layerStack = mExtension->GetTextureSource()->GetLayerStack();
    if (!layerStack)
        return nullptr;

    ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
    TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(texture->Source.GetSizeX(), texture->Source.GetSizeY(), format);
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());
    TSharedPtr<IOdysseyImageRenderer> renderer = currentLayer->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, 0);
    renderer->Init();

    FOdysseyImageRendererCopyParams params(block, { block->Rect() });
    renderer->Copy(params, {});

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
    ctx.Finish();

    return block;
}

TSharedPtr<::ULIS::FBlock>
FOdysseyTextureEditorRasterPaintBucketToolSourceProvider::GetForegroundLayersBlock() const
{
    UTexture2D* texture = mExtension->Texture();
    if (!texture)
        return nullptr;

    UOdysseyTextureLayerStack* layerStack = mExtension->GetTextureSource()->GetLayerStack();
    if (!layerStack)
        return nullptr;

    ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
    TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(texture->Source.GetSizeX(), texture->Source.GetSizeY(), format);
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());
    TArray<FOdysseyImageRenderingAbility*> layersToExclude = GetForegroundLayersToExclude(currentLayer);
    FImageRendererFilter filter = FImageRendererFilter::CreateLambda(
        [layersToExclude](const FOdysseyImageRenderingAbility* iRenderingAbility)
        {
            return !layersToExclude.Contains(iRenderingAbility);
        }
    );

    TSharedPtr<IOdysseyImageRenderer> renderer = layerStack->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, 0, filter);
    if (!renderer)
        return nullptr;

    renderer->Init();

    FOdysseyImageRendererCopyParams params(block, { block->Rect() });
    renderer->Copy(params, {});

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
    ctx.Finish();

    return block;
}

TSharedPtr<::ULIS::FBlock>
FOdysseyTextureEditorRasterPaintBucketToolSourceProvider::GetBackgroundLayersBlock() const
{
    UTexture2D* texture = mExtension->Texture();
    if (!texture)
        return nullptr;

    UOdysseyTextureLayerStack* layerStack = mExtension->GetTextureSource()->GetLayerStack();
    if (!layerStack)
        return nullptr;

    ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
    TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(texture->Source.GetSizeX(), texture->Source.GetSizeY(), format);
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());
    TArray<FOdysseyImageRenderingAbility*> layersToExclude = GetBackgroundLayersToExclude(currentLayer);
    FImageRendererFilter filter = FImageRendererFilter::CreateLambda(
        [layersToExclude](const FOdysseyImageRenderingAbility* iRenderingAbility) -> bool
        {
            return !layersToExclude.Contains(iRenderingAbility);
        }
    );

    TSharedPtr<IOdysseyImageRenderer> renderer = layerStack->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, 0, filter);
    if (!renderer)
        return nullptr;

    renderer->Init();

    FOdysseyImageRendererCopyParams params(block, { block->Rect() });
    renderer->Copy(params, {});

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
    ctx.Finish();

    return block;
}

TSharedPtr<::ULIS::FBlock>
FOdysseyTextureEditorRasterPaintBucketToolSourceProvider::GetAllLayersBlock() const
{
    UTexture2D* texture = mExtension->Texture();
    if (!texture)
        return nullptr;

    UOdysseyTextureLayerStack* layerStack = mExtension->GetTextureSource()->GetLayerStack();
    if (!layerStack)
        return nullptr;

    ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
    TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(texture->Source.GetSizeX(), texture->Source.GetSizeY(), format);
    TSharedPtr<IOdysseyImageRenderer> renderer = layerStack->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, 0);
    renderer->Init();

    FOdysseyImageRendererCopyParams params(block, { block->Rect() });
    renderer->Copy(params, {});

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
    ctx.Finish();

    return block;
}

TArray<FOdysseyImageRenderingAbility*>
FOdysseyTextureEditorRasterPaintBucketToolSourceProvider::GetBackgroundLayersToExclude(UOdysseyLayer* iLayer) const
{
    TArray<FOdysseyImageRenderingAbility*> resultLayers;

    UOdysseyTextureLayerStack* layerStack = mExtension->GetTextureSource()->GetLayerStack();
    if (!layerStack)
        return resultLayers;

    TArray<UOdysseyLayer*> layers = layerStack->GetLayers();
    for( int i = 0; i < layers.Num(); i++ )
    {
        if (iLayer->IsChildOf(layers[i]) && (iLayer->GetParent() != layers[i] || iLayer->GetIndexInParent() != layers[i]->GetChildren().Num() - 1))
            continue;

        if (iLayer->IsChildOf(layers[i]))
            continue;

        resultLayers.Add(Cast<UOdysseyTextureLayer>(layers[i]));

        if (layers[i] == iLayer)
            break;
    }

    return resultLayers;
}

TArray<FOdysseyImageRenderingAbility*>
FOdysseyTextureEditorRasterPaintBucketToolSourceProvider::GetForegroundLayersToExclude(UOdysseyLayer* iLayer) const
{
    TArray<FOdysseyImageRenderingAbility*> resultLayers;

    UOdysseyTextureLayerStack* layerStack = mExtension->GetTextureSource()->GetLayerStack();
    if (!layerStack)
        return resultLayers;

    TArray<UOdysseyLayer*> layers = layerStack->GetLayers();

    for( int i = layers.Num() - 1; i >= 0; i-- )
    {
        resultLayers.Add(Cast<UOdysseyTextureLayer>(layers[i]));

        if (layers[i] == iLayer)
            break;
    }

    return resultLayers;
}
