// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyMagicWandShape.h"

#include "Engine/TextureRenderTarget2D.h"
#include "ImageUtils.h"

#include "OdysseyAnimationPlayer.h"
#include "OdysseyLayer.h"
#include "OdysseyLayerStack.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPixelFormat.h"
#include "OdysseySurfaceTexture2DEditable.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyMagicWandShape::~UOdysseyMagicWandShape()
{
}

UOdysseyMagicWandShape::UOdysseyMagicWandShape(const FObjectInitializer& iObjectInitializer)
    : Super(iObjectInitializer)
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse Events

FOdysseyPainterEditor*
UOdysseyMagicWandShape::GetEditor() const
{
    return mEditor;
}

void
UOdysseyMagicWandShape::SetEditor(FOdysseyPainterEditor* iEditor)
{
    mEditor = iEditor;
}

TSharedPtr<::ULIS::FBlock>
UOdysseyMagicWandShape::GetSourceBlock() const
{
    switch (Source)
    {
        //Current Layer
    case EOdysseyRasterMagicWandToolSource::CurrentLayer:
        return GetCurrentLayerBlock();
        //Foreground Layers
    case EOdysseyRasterMagicWandToolSource::ForegroundLayers:
        return GetForegroundLayersBlock();
        //Background Layers
    case EOdysseyRasterMagicWandToolSource::BackgroundLayers:
        return GetBackgroundLayersBlock();
        //All Layers
    case EOdysseyRasterMagicWandToolSource::AllLayers:
        return GetAllLayersBlock();
    }

    return nullptr;
}

TSharedPtr<::ULIS::FBlock>
UOdysseyMagicWandShape::GetCurrentLayerBlock() const
{
    UOdysseyLayerStack* layerStack = GetEditor()->LayerStack();
    if (!layerStack)
        return nullptr;

    UOdysseyLayer* layer = layerStack->GetCurrentLayer();
    if (!layer)
        return nullptr;

    FFrameNumber currentFrame(0);
    UOdysseyAnimationPlayer* player = GetEditor()->GetAnimationPlayer();
    if (player)
        currentFrame = player->GetCurrentFrame().FrameNumber;

    TStrongObjectPtr<UTextureRenderTarget2D> renderTarget(NewObject<UTextureRenderTarget2D>());
    FIntRect rect = layer->GetDefaultRenderRect();
    renderTarget->InitAutoFormat(rect.Width(), rect.Height());
    layer->Render_GameThread(renderTarget.Get(), currentFrame, EOdysseyRenderingType::Render);

    FImage OutImage;
    if (!FImageUtils::GetRenderTargetImage(renderTarget.Get(), OutImage))
        return nullptr;

    ::ULIS::eFormat format = ULISFormatForRawImageFormat(OutImage.Format);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
    TSharedPtr<::ULIS::FBlock> block = MakeShareable(new ::ULIS::FBlock(rect.Width(), rect.Height(), format));
    CopyImageToBlock(OutImage, block.Get());

    return block;
}

TSharedPtr<::ULIS::FBlock>
UOdysseyMagicWandShape::GetForegroundLayersBlock() const
{
    UOdysseyLayerStack* layerStack = GetEditor()->LayerStack();
    if (!layerStack)
        return nullptr;

    UOdysseyLayer* layer = layerStack->GetCurrentLayer();
    if (!layer)
        return nullptr;

    FFrameNumber currentFrame(0);
    UOdysseyAnimationPlayer* player = GetEditor()->GetAnimationPlayer();
    if (player)
        currentFrame = player->GetCurrentFrame().FrameNumber;

    TArray<IOdysseyRenderingAbility*> layersToExclude = GetForegroundLayersToExclude(layer);

    TStrongObjectPtr<UTextureRenderTarget2D> renderTarget(NewObject<UTextureRenderTarget2D>());
    FIntRect rect = layerStack->GetDefaultRenderRect();
    renderTarget->InitAutoFormat(rect.Width(), rect.Height());

    layerStack->Render_GameThread(
        renderTarget.Get(),
        currentFrame,
        EOdysseyRenderingType::Render,
        IOdysseyTextureRenderingAbility::FCanRenderFunction::CreateLambda(
            [layersToExclude](const IOdysseyTextureRenderingAbility* iAbility, TArray<const IOdysseyTextureRenderingAbility*> iParents)
            {
                return !layersToExclude.Contains(iAbility);
            }
        )
    );

    FImage OutImage;
    if (!FImageUtils::GetRenderTargetImage(renderTarget.Get(), OutImage))
        return nullptr;

    ::ULIS::eFormat format = ULISFormatForRawImageFormat(OutImage.Format);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
    TSharedPtr<::ULIS::FBlock> block = MakeShareable(new ::ULIS::FBlock(rect.Width(), rect.Height(), format));
    CopyImageToBlock(OutImage, block.Get());

    return block;
}

TSharedPtr<::ULIS::FBlock>
UOdysseyMagicWandShape::GetBackgroundLayersBlock() const
{
    UOdysseyLayerStack* layerStack = GetEditor()->LayerStack();
    if (!layerStack)
        return nullptr;

    UOdysseyLayer* layer = layerStack->GetCurrentLayer();
    if (!layer)
        return nullptr;

    FFrameNumber currentFrame(0);
    UOdysseyAnimationPlayer* player = GetEditor()->GetAnimationPlayer();
    if (player)
        currentFrame = player->GetCurrentFrame().FrameNumber;

    TArray<IOdysseyRenderingAbility*> layersToExclude = GetBackgroundLayersToExclude(layer);

    TStrongObjectPtr<UTextureRenderTarget2D> renderTarget(NewObject<UTextureRenderTarget2D>());
    FIntRect rect = layerStack->GetDefaultRenderRect();
    renderTarget->InitAutoFormat(rect.Width(), rect.Height());

    layerStack->Render_GameThread(
        renderTarget.Get(),
        currentFrame,
        EOdysseyRenderingType::Render,
        IOdysseyTextureRenderingAbility::FCanRenderFunction::CreateLambda(
            [layersToExclude](const IOdysseyTextureRenderingAbility* iAbility, TArray<const IOdysseyTextureRenderingAbility*> iParents)
            {
                return !layersToExclude.Contains(iAbility);
            }
        )
    );

    FImage OutImage;
    if (!FImageUtils::GetRenderTargetImage(renderTarget.Get(), OutImage))
        return nullptr;

    ::ULIS::eFormat format = ULISFormatForRawImageFormat(OutImage.Format);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
    TSharedPtr<::ULIS::FBlock> block = MakeShareable(new ::ULIS::FBlock(rect.Width(), rect.Height(), format));
    CopyImageToBlock(OutImage, block.Get());

    return block;
}

TSharedPtr<::ULIS::FBlock>
UOdysseyMagicWandShape::GetAllLayersBlock() const
{
    UOdysseyLayerStack* layerStack = GetEditor()->LayerStack();
    if (!layerStack)
        return nullptr;

    FFrameNumber currentFrame(0);
    UOdysseyAnimationPlayer* player = GetEditor()->GetAnimationPlayer();
    if (player)
        currentFrame = player->GetCurrentFrame().FrameNumber;

    TStrongObjectPtr<UTextureRenderTarget2D> renderTarget(NewObject<UTextureRenderTarget2D>());
    FIntRect rect = layerStack->GetDefaultRenderRect();
    renderTarget->InitAutoFormat(rect.Width(), rect.Height());
    layerStack->Render_GameThread(renderTarget.Get(), currentFrame, EOdysseyRenderingType::Render);

    FImage OutImage;
    if (!FImageUtils::GetRenderTargetImage(renderTarget.Get(), OutImage))
        return nullptr;

    ::ULIS::eFormat format = ULISFormatForRawImageFormat(OutImage.Format);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
    TSharedPtr<::ULIS::FBlock> block = MakeShareable(new ::ULIS::FBlock(rect.Width(), rect.Height(), format));
    CopyImageToBlock(OutImage, block.Get());

    return block;
}

void
UOdysseyMagicWandShape::IncludeColorsToMaskBlock(TSharedPtr<::ULIS::FBlock> iSrcBlock, TSharedPtr<::ULIS::FBlock> iMaskBlock) const
{
    TArray<::ULIS::FColor> colors;

    for (const FLinearColor& linearColor : IncludeColors)
    {
        ::ULIS::FColor color = ::ULIS::FColor::FromRGBAF(linearColor.R, linearColor.G, linearColor.B, linearColor.A);
        if (color.Format() != iSrcBlock->Format())
            color = color.ToFormat(iSrcBlock->Format());

        colors.Add(color);
    }

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_G8);
    ctx.FilterInto(
        [this, &colors](const ::ULIS::FPixel& iSrcPixel, ::ULIS::FPixel& iDstPixel, uint64 iNumPixels)
        {
            for (int i = 0; i < iNumPixels; i++, iSrcPixel.Next(), iDstPixel.Next())
            {
                if (iSrcPixel.AlphaF() == 0.f)
                    continue;

                for (const ::ULIS::FColor& color : colors)
                {
                    bool includeColor = true;
                    for (int j = 0; j < color.SamplesPerPixel(); j++)
                    {
                        if (iSrcPixel.HasAlpha() && iSrcPixel.AlphaIndex() == j)
                            continue;


                        if (iSrcPixel.ChannelF(j) != color.ChannelF(j))
                        {
                            includeColor = false;
                            break;
                        }
                    }

                    if (includeColor)
                    {
                        iDstPixel.SetGrey8(255);
                        break;
                    }
                }
            }
        }
        , *iSrcBlock
        , *iMaskBlock
    );
    ctx.Finish();
}

TSharedPtr<::ULIS::FBlock>
UOdysseyMagicWandShape::CreateSourceMaskBlock(TSharedPtr<::ULIS::FBlock> iBlock, const ::ULIS::ISample& iColor) const
{
    switch (ColorToleranceSource)
    {
    case EOdysseyRasterMagicWandToolColorToleranceSource::Color: return CreateSourceMaskBlockFromColor(iBlock, iColor, true, false);
    case EOdysseyRasterMagicWandToolColorToleranceSource::Transparency: return CreateSourceMaskBlockFromColor(iBlock, iColor, false, true);
    case EOdysseyRasterMagicWandToolColorToleranceSource::ColorAndTransparency: return CreateSourceMaskBlockFromColor(iBlock, iColor, true, true);
    case EOdysseyRasterMagicWandToolColorToleranceSource::Luminosity: return CreateSourceMaskBlockFromLuminosity(iBlock, iColor);
    }
    return nullptr;
}

TSharedPtr<::ULIS::FBlock>
UOdysseyMagicWandShape::CreateSourceMaskBlockFromColor(TSharedPtr<::ULIS::FBlock> iBlock, const ::ULIS::ISample& iColor, bool iUseColor, bool iUseTransparency) const
{
    TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(iBlock->Width(), iBlock->Height(), ::ULIS::Format_G8);

    ::ULIS::FColor color = iColor.ToFormat(iBlock->Format());
    TArray<float> channelsMin;
    TArray<float> channelsMax;

    for (int i = 0; i < color.SamplesPerPixel(); i++)
    {
        channelsMin.Add(color.ChannelF(i) - ColorTolerance / 100.f);
        channelsMax.Add(color.ChannelF(i) + ColorTolerance / 100.f);
    }

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_G8);
    ctx.FilterInto(
        [&, this](const ::ULIS::FPixel& iSrcPixel, ::ULIS::FPixel& iDstPixel, uint64 iNumPixels)
        {
            for (int i = 0; i < iNumPixels; i++, iSrcPixel.Next(), iDstPixel.Next())
            {
                iDstPixel.SetGrey8(0);
                for (int j = 0; j < color.SamplesPerPixel(); j++)
                {
                    if (!iUseTransparency && color.HasAlpha() && color.AlphaIndex() == j)
                        continue;

                    if (!iUseColor && (!color.HasAlpha() || color.AlphaIndex() != j))
                        continue;

                    if (iSrcPixel.ChannelF(j) < channelsMin[j] || iSrcPixel.ChannelF(j) > channelsMax[j])
                    {
                        iDstPixel.SetGrey8(255);
                        break;
                    }
                }
            }
        }
        , *iBlock
        , *block
    );
    ctx.Finish();

    return block;
}

TSharedPtr<::ULIS::FBlock>
UOdysseyMagicWandShape::CreateSourceMaskBlockFromLuminosity(TSharedPtr<::ULIS::FBlock> iBlock, const ::ULIS::ISample& iColor) const
{
    TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(iBlock->Width(), iBlock->Height(), ::ULIS::Format_G8);

    float minLuma = iColor.LumaF() - ColorTolerance / 100.f;
    float maxLuma = iColor.LumaF() + ColorTolerance / 100.f;

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_G8);
    ctx.FilterInto(
        [this, minLuma, maxLuma](const ::ULIS::FPixel& iSrcPixel, ::ULIS::FPixel& iDstPixel, uint64 iNumPixels)
        {
            for (int i = 0; i < iNumPixels; i++, iSrcPixel.Next(), iDstPixel.Next())
            {
                if (iSrcPixel.LumaF() < minLuma || iSrcPixel.LumaF() > maxLuma)
                    iDstPixel.SetGrey8(255);
                else
                    iDstPixel.SetGrey8(0);
            }
        }
        , *iBlock
        , *block
    );
    ctx.Finish();

    return block;
}

bool
UOdysseyMagicWandShape::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    mOnBegin.Broadcast();
    return true;
}

bool
UOdysseyMagicWandShape::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (iKey != EKeys::LeftMouseButton || !mEditor)
        return false;

    FOdysseyMediaProvider mediaProvider = mEditor->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    if (!hasRaster)
        return false;

    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetOrCreateMedias<FOdysseyMediaRaster>();
    if (mediaRasters.IsEmpty())
        return false;

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = mediaRasters[0]->GetRasterBlock();

    mPaintEngine.RasterBlock(rasterBlock);

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> paintBlock = mPaintEngine.PaintBlock();
    ::ULIS::eFormat format = paintBlock->Format();
    if (!paintBlock)
        return false;

    if (iPointInTexture.x < 0 || iPointInTexture.y < 0)
        return false;

    if (iPointInTexture.x >= paintBlock->Width() || iPointInTexture.y >= paintBlock->Height())
        return false;

    TSharedPtr<::ULIS::FBlock> sourceBlock = GetSourceBlock();
    if (!sourceBlock)
        return false;

    TSharedPtr<::ULIS::FBlock> block = rasterBlock->GetBlock();
    ::ULIS::FPixel sourceColor = sourceBlock->Pixel(iPointInTexture.x, iPointInTexture.y);
    ::ULIS::FColor dstColor = GetEditor()->PaintColor().GetValue();
    TSharedPtr<::ULIS::FBlock> sourceMaskBlock = CreateSourceMaskBlock(sourceBlock, sourceColor);
    TSharedPtr<::ULIS::FBlock> maskBlock = MakeShared<::ULIS::FBlock>(block->Width(), block->Height(), ::ULIS::Format_G8);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_G8);

    ctx.Clear(*maskBlock);
    ctx.Finish();

    if (Contiguous)
    {
        // Gap tolerance is only relevant for contiguous selections
        if (GapTolerance > 0)
        {
            ctx.Dilate(
                *sourceMaskBlock,
                GapTolerance,
                ::ULIS::FColor::FromGrey8(255),
                ::ULIS::FColor::FromGrey8(255)
            );
            ctx.Finish();
        }

        // Select only matching pixels connected to the clicked position
        ctx.FloodFill(
            *sourceMaskBlock,
            *maskBlock,
            iPointInTexture.x,
            iPointInTexture.y,
            ::ULIS::FColor::FromGrey8(0),
            ::ULIS::FColor::FromGrey8(255)
        );
        ctx.Finish();
    }
    else // Check all image
    {
        for (int y = 0; y < sourceMaskBlock->Height(); y++)
        {
            uint8* src = (uint8*)sourceMaskBlock->ScanlineBits(y);
            uint8* dst = (uint8*)maskBlock->ScanlineBits(y);

            for (int x = 0; x < sourceMaskBlock->Width(); x++)
            {
                dst[x] = 255 - src[x];
            }
        }
    }

    //Expansion and final step of gap tolerance is managed in a single dilate operation
    if (Expansion != 0 || GapTolerance > 0)
    {
        float dilateAmount = Expansion + GapTolerance;

        ctx.Dilate(
            *maskBlock
            , dilateAmount
            , ::ULIS::FColor::FromGrey8(255)
            , dilateAmount > 0 ? ::ULIS::FColor::FromGrey8(255) : ::ULIS::FColor::FromGrey8(0)
        );
        ctx.Finish();
    }

    if (IncludeColors.Num() > 0)
    {
        TSharedPtr<::ULIS::FBlock> tmpMaskBlock = MakeShared<::ULIS::FBlock>(block->Width(), block->Height(), ::ULIS::Format_G8);
        ctx.Clear(*tmpMaskBlock);
        ctx.Finish();

        //add lines to maskBlock as 255
        IncludeColorsToMaskBlock(sourceBlock, maskBlock);

        //floodfill at same position with src 255 dst 255
        ctx.FloodFill(
            *maskBlock
            , *tmpMaskBlock
            , iPointInTexture.x
            , iPointInTexture.y
            , ::ULIS::FColor::FromGrey8(255)
            , ::ULIS::FColor::FromGrey8(255)
        );
        ctx.Finish();

        //use the resulting mask as the maskblock
        maskBlock = tmpMaskBlock;
    }

    mOnSelectionBlockCommit.Broadcast(maskBlock);

    return true;
}

TArray<IOdysseyRenderingAbility*>
UOdysseyMagicWandShape::GetBackgroundLayersToExclude(UOdysseyLayer* iLayer) const
{
    TArray<IOdysseyRenderingAbility*> resultLayers;

    UOdysseyLayerStack* layerStack = GetEditor()->LayerStack();
    if (!layerStack)
        return resultLayers;

    TArray<UOdysseyLayer*> layers = layerStack->GetLayers();
    for (int i = 0; i < layers.Num(); i++)
    {
        if (iLayer->IsChildOf(layers[i]) && (iLayer->GetParent() != layers[i] || iLayer->GetIndexInParent() != layers[i]->GetChildren().Num() - 1))
            continue;

        if (iLayer->IsChildOf(layers[i]))
            continue;

        resultLayers.Add(layers[i]);

        if (layers[i] == iLayer)
            break;
    }

    return resultLayers;
}

TArray<IOdysseyRenderingAbility*>
UOdysseyMagicWandShape::GetForegroundLayersToExclude(UOdysseyLayer* iLayer) const
{
    TArray<IOdysseyRenderingAbility*> resultLayers;

    UOdysseyLayerStack* layerStack = GetEditor()->LayerStack();
    if (!layerStack)
        return resultLayers;

    TArray<UOdysseyLayer*> layers = layerStack->GetLayers();

    for (int i = layers.Num() - 1; i >= 0; i--)
    {
        resultLayers.Add(layers[i]);

        if (layers[i] == iLayer)
            break;
    }

    return resultLayers;
}

#undef LOCTEXT_NAMESPACE
