// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPaintableTexture2D.h"
#include "UObject/ObjectSaveContext.h"
#include "RenderGraphUtils.h"
#include "GenerateMips.h"
#include "OdysseyBlendShader.h"
#include "ImageCoreUtils.h"
#include "ScreenPass.h"

void
UOdysseyPaintableTexture2D::Initialize(int InWidth, int InHeight, ETextureRenderTargetFormat InFormat)
{
    Width = (uint32)InWidth;
    Height = (uint32)InHeight;
    Format = InFormat;
    EPixelFormat pixelFormat = GetPixelFormatFromRenderTargetFormat(Format);
    ERawImageFormat::Type RawFormat = FImageCoreUtils::GetRawImageFormatForPixelFormat(pixelFormat);
    ETextureSourceFormat TextureFormat = FImageCoreUtils::ConvertToTextureSourceFormat(RawFormat);
    Texture = NewObject<UTexture2D>(this, TEXT("Texture"), RF_Public);
    Texture->Source.Init(Width, Height, 1, 1, TextureFormat);
    Texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    Texture->UpdateResource();

    WetTiles = FOdysseyTiledRenderTarget(64, Width, Height, Format);
    DryTiles = FOdysseyTiledRenderTarget(64, Width, Height, Format);
    OriginalTiles = FOdysseyTiledRenderTarget(64, Width, Height, Format);

    CancellableWetTiles = FOdysseyTiledRenderTarget(64, Width, Height, Format);
    CancellableDryTiles = FOdysseyTiledRenderTarget(64, Width, Height, Format);
}

void
UOdysseyPaintableTexture2D::SetBlendParamters( EOdysseyBlendingMode InBlendMode, EOdysseyAlphaMode InAlphaMode, float InOpacity, bool InPreserveAlpha )
{
    BlendMode = InBlendMode;
    AlphaMode = InAlphaMode;
    Opacity = InOpacity;
    PreserveAlpha = InPreserveAlpha;

    UpdateTexture();
}

int
UOdysseyPaintableTexture2D::GetWidth() const
{
    return Width;
}

int
UOdysseyPaintableTexture2D::GetHeight() const
{
    return Height;
}

ETextureRenderTargetFormat
UOdysseyPaintableTexture2D::GetFormat() const
{
    return Format;
}

EOdysseyBlendingMode
UOdysseyPaintableTexture2D::GetBlendMode() const
{
    return BlendMode;
}

EOdysseyAlphaMode
UOdysseyPaintableTexture2D::GetAlphaMode() const
{
    return AlphaMode;
}

float
UOdysseyPaintableTexture2D::GetOpacity() const
{
    return Opacity;
}

bool
UOdysseyPaintableTexture2D::GetPreserveAlpha() const
{
    return PreserveAlpha;
}

void
UOdysseyPaintableTexture2D::BeginDraw()
{
    checkf(!IsDrawing, TEXT("BeginDraw() cannot be called twice in a row, Call EndDraw() first"));

    if (IsDrawing)
        return;

    IsDrawing = true;
}

void
UOdysseyPaintableTexture2D::BeginUndoRecording()
{
    checkf(IsDrawing, TEXT("BeginUndoRecording cannot be called if BeginDraw() has not been called first"));

    if (!IsDrawing)
        return;

}

void
UOdysseyPaintableTexture2D::EndUndoRecording()
{
    checkf(IsDrawing, TEXT("EndUndoRecording cannot be called if BeginDraw() has not been called first"));

    if (!IsDrawing)
        return;

}

void
UOdysseyPaintableTexture2D::CancelUndoRecording()
{
    checkf(IsDrawing, TEXT("CancelUndoRecording cannot be called if BeginDraw() has not been called first"));

    if (!IsDrawing)
        return;

}

void
UOdysseyPaintableTexture2D::Draw(UTexture* SourceTexture, FIntRect SourceRect, FIntPoint DestinationPosition)
{
    checkf(IsDrawing, TEXT("Draw cannot be called if BeginDraw() has not been called first"));

    if (!IsDrawing)
        return;

    TArray<FIntPoint> affectedTileIndexes = OriginalTiles.GetTileIndexesInRect(FIntRect(DestinationPosition, DestinationPosition + SourceRect.Size()));
    TArray<FIntPoint> originalDirtyTiles = OriginalTiles.GetDirtyTileIndexes();
    TArray<FIntPoint> originalTilesToCopy = affectedTileIndexes.FilterByPredicate(
        [originalDirtyTiles](const FIntPoint& iTileIndex)
        {
            return !originalDirtyTiles.Contains(iTileIndex);
        }
    );

    OriginalTiles.Draw(Texture, originalTilesToCopy);
    WetTiles.Draw(SourceTexture, SourceRect, DestinationPosition);

    //Blend WetTiles and DryTiles and Refresh the final Texture
    UpdateTexture();
}

void
UOdysseyPaintableTexture2D::Dry()
{
    checkf(IsDrawing, TEXT("Draw cannot be called if BeginDraw() has not been called first"));

    if (!IsDrawing)
        return;
}

void
UOdysseyPaintableTexture2D::ResetDraw()
{
    checkf(IsDrawing, TEXT("ResetDraw cannot be called if BeginDraw() has not been called first"));

    if (!IsDrawing)
        return;
}

void
UOdysseyPaintableTexture2D::CommitDraw()
{
    checkf(IsDrawing, TEXT("CommitDraw cannot be called if BeginDraw() has not been called first"));

    if (!IsDrawing)
        return;
}

void
UOdysseyPaintableTexture2D::EndDraw()
{
    checkf(IsDrawing, TEXT("EndDraw cannot be called if BeginDraw() has not been called first"));

    if (!IsDrawing)
        return;

    CommitToSource();

    //Cleanup
    WetTiles.Clear();
    DryTiles.Clear();
    OriginalTiles.Clear();
    CancellableWetTiles.Clear();
    CancellableDryTiles.Clear();

    IsDrawing = false;
}

bool
UOdysseyPaintableTexture2D::GetIsDrawing() const
{
    return IsDrawing;
}

UTexture2D*
UOdysseyPaintableTexture2D::GetTexture() const
{
    return Texture;
}

void
UOdysseyPaintableTexture2D::PreSave(FObjectPreSaveContext SaveContext)
{
    CommitToSource();
}

void
UOdysseyPaintableTexture2D::CommitToSource()
{

}

TArray<FRHITexture*>
GetTilesRHITextures(const TArray<UTexture*> Textures )
{
    TArray<FRHITexture*> rhiTextures;
    for (const UTexture* texture : Textures)
    {
        rhiTextures.Add(texture->GetResource()->TextureRHI);
    }

    return rhiTextures;
}

void RenderTilesToTexture_RenderThread(FRDGBuilder& GraphBuilder, TArray<FRHITexture*> Tiles, TArray<FIntRect> Rects, FRDGTextureRef DestinationTexture)
{
    for (int i = 0; i < Tiles.Num(); i++ )
    {
        FIntRect rect = Rects[i];
        FRHITexture* rhiTexture = Tiles[i];
        FRDGTextureRef tileTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(rhiTexture, TEXT("UOdysseyPaintableTexture2D::TileTexture")));
        AddCopyTexturePass(
            GraphBuilder,
            tileTexture,
            DestinationTexture,
            FIntPoint(0, 0),
            rect.Min,
            rect.Size()
        );
    }
}

void BlendTilesToTexture_RenderThread(FRDGBuilder& GraphBuilder, const ERHIFeatureLevel::Type FeatureLevel, TArray<FRHITexture*> Tiles, TArray<FIntRect> Rects, FRDGTextureRef DestinationTexture, EOdysseyBlendingMode InBlendMode, EOdysseyAlphaMode InAlphaMode, float InOpacity)
{
    for (int i = 0; i < Tiles.Num(); i++ )
    {
        FIntRect rect = Rects[i];
        FIntRect tileRect = rect - rect.Min;
        FRHITexture* rhiTexture = Tiles[i];
        FRDGTextureRef tileTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(rhiTexture, TEXT("UOdysseyPaintableTexture2D::TileTexture")));

        FOdysseyBlendShader::BlendRect(
            GraphBuilder,
            FeatureLevel,
            DestinationTexture,
            tileTexture,
            DestinationTexture,
            tileRect,
            rect,
            FMatrix::Identity,
            InBlendMode,
            InAlphaMode,
            InOpacity,
            EOdysseyAntiAliasing::NearestNeighbor
        );
    }
}


void
UOdysseyPaintableTexture2D::UpdateTexture()
{
#if WITH_EDITOR
    Texture->BlockOnAnyAsyncBuild();
#endif

    TArray<FIntPoint> originalTileIndexes;
    TArray<FIntRect> originalTileRects;
    TArray<UTexture*> originalTileTextures;
    OriginalTiles.GetDirtyTiles( originalTileIndexes, originalTileRects, originalTileTextures );

    TArray<FIntPoint> dryTileIndexes;
    TArray<FIntRect> dryTileRects;
    TArray<UTexture*> dryTileTextures;
    DryTiles.GetDirtyTiles( dryTileIndexes, dryTileRects, dryTileTextures );

    TArray<FIntPoint> wetTileIndexes;
    TArray<FIntRect> wetTileRects;
    TArray<UTexture*> wetTileTextures;
    WetTiles.GetDirtyTiles( wetTileIndexes, wetTileRects, wetTileTextures );

    EPixelFormat pixelFormat = GetPixelFormatFromRenderTargetFormat(Format);
    const ERHIFeatureLevel::Type featureLevel = Texture->GetWorld() ? Texture->GetWorld()->GetFeatureLevel() : GMaxRHIFeatureLevel;
    ENQUEUE_RENDER_COMMAND(IOdysseyTextureRenderingAbility_RenderRectAtRect)(
        [
            width = Width,
            height = Height,
            pixelFormat,
            originalTileTextures,
            originalTileRects,
            dryTileTextures,
            dryTileRects,
            wetTileTextures,
            wetTileRects,
            textureRHI = Texture->GetResource()->TextureRHI,
            blendMode = BlendMode,
            alphaMode = AlphaMode,
            opacity = Opacity,
            featureLevel
        ](FRHICommandListImmediate& RHICmdList)
        {
            TArray<FRHITexture*> originalTileRhiTextures = GetTilesRHITextures(originalTileTextures);
            TArray<FRHITexture*> dryTileRhiTextures = GetTilesRHITextures(dryTileTextures);
            TArray<FRHITexture*> wetTileRhiTextures = GetTilesRHITextures(wetTileTextures);

            FRDGBuilder graphBuilder(RHICmdList);

            uint32 numMips = FMath::CeilLogTwo(FMath::Max(width, height));

            FRDGTextureDesc renderTextureDesc = FRDGTextureDesc::Create2D(
                FIntPoint(width, height),
                pixelFormat,
                FClearValueBinding::Transparent,
                ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable,
                numMips
            );
            FRDGTextureRef renderTexture = graphBuilder.CreateTexture( renderTextureDesc, TEXT( "UOdysseyPaintableTexture2D::renderTexture" ) );
            FRDGTextureRef sourceTexture = graphBuilder.RegisterExternalTexture(CreateRenderTarget(textureRHI, TEXT("UOdysseyPaintableTexture2D::sourceTexture")));

            //Copy the actual texture first
            AddDrawTexturePass(
                graphBuilder,
                FScreenPassViewInfo(),
                sourceTexture,
                renderTexture
            );

            //Copy the dirty original tiles
            RenderTilesToTexture_RenderThread(graphBuilder, originalTileRhiTextures, originalTileRects, renderTexture);

            //Copy the dirty dry tiles
            RenderTilesToTexture_RenderThread(graphBuilder, dryTileRhiTextures, dryTileRects, renderTexture);

            //Blend the dirty Wet tiles
            BlendTilesToTexture_RenderThread(graphBuilder, featureLevel, wetTileRhiTextures, wetTileRects, renderTexture, blendMode, alphaMode, opacity);

            //Generate MipMap
            FGenerateMips::Execute(graphBuilder, featureLevel, renderTexture);

            //Copy all mipmaps from renderTexture to Texture
            //AddDrawTexturePass copies all the mips and does format conversion if needed (just for info, because we don't need format conversion here)

            FGlobalShaderMap* globalShaderMap = GetGlobalShaderMap(featureLevel);
            FRDGDrawTextureInfo drawInfo;
            drawInfo.Size = FIntPoint(width, height);
            drawInfo.NumMips = numMips;
            AddDrawTexturePass(graphBuilder, globalShaderMap, renderTexture, sourceTexture, drawInfo);

            graphBuilder.Execute();
        }
    );
}
