// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTiledImage.h"

#include "Engine/Texture.h"
#include "Engine/TextureRenderTarget2D.h"
#include "RenderGraphUtils.h"
#include "ScreenPass.h"
#include "TextureResource.h"

#include "OdysseyTileUtils.h"

DECLARE_STATS_GROUP(TEXT("UOdysseyTiledImage"), STATGROUP_OdysseyTiledImage, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("UOdysseyTiledImage::Render"), STAT_Render, STATGROUP_OdysseyTiledImage);


BEGIN_SHADER_PARAMETER_STRUCT(FWriteTileParameters, )
    RDG_TEXTURE_ACCESS(Texture, ERHIAccess::CopyDest)
END_SHADER_PARAMETER_STRUCT()

void
AddWriteTilePass(FRDGBuilder& GraphBuilder, FRDGTextureRef OutTexture, FSharedBuffer InBuffer)
{
    FWriteTileParameters* Params = GraphBuilder.AllocParameters<FWriteTileParameters>();
    Params->Texture = OutTexture;

    GraphBuilder.AddPass(
        RDG_EVENT_NAME("UOdysseyTiledImage::AddWriteTilePass"),
        Params,
        ERDGPassFlags::Copy | ERDGPassFlags::NeverCull,
        [OutTexture, InBuffer](FRHICommandList& RHICmdList)
        {
            const FRDGTextureDesc& Desc = OutTexture->Desc;
            const FPixelFormatInfo& PixelFormatInfo = GPixelFormats[Desc.Format];
            uint32 BytesPerPixel = PixelFormatInfo.BlockBytes;
            uint32 BufferStride = BytesPerPixel * Desc.Extent.X;

            FUpdateTextureRegion2D region(0, 0, 0, 0, Desc.Extent.X, Desc.Extent.Y);
            RHICmdList.UpdateTexture2D(OutTexture->GetRHI(), 0, region, BufferStride, (const uint8*)InBuffer.GetData());
        }
    );
}

UOdysseyTiledImage::~UOdysseyTiledImage()
{
}

UOdysseyTiledImage::UOdysseyTiledImage()
{
}

void
UOdysseyTiledImage::Initialize(int InTileSize, EOdysseyTiledImageFormat InFormat)
{
    TileSize = InTileSize;
    Format = InFormat;

    Tiles.Empty();
}

EOdysseyTiledImageFormat
UOdysseyTiledImage::GetFormat() const
{
    return Format;
}

EPixelFormat
UOdysseyTiledImage::GetPixelFormat() const
{
    switch(Format)
    {
        case EOdysseyTiledImageFormat::RGBA8: return PF_R8G8B8A8;
        case EOdysseyTiledImageFormat::RGBA16F: return PF_FloatRGBA;
        case EOdysseyTiledImageFormat::RGBA32F: return PF_A32B32G32R32F; //Is actually RGBA32F not ABGR32F (not sure why)
    }
    return PF_B8G8R8A8;
}

void
UOdysseyTiledImage::CopyFromTexture(UTexture* InTexture, FIntRect InRect, FIntPoint InPosition)
{
    FOdysseyTileManager::FGetExistingTileId GetExistingTileId = FOdysseyTileManager::FGetExistingTileId::CreateLambda(
        [this](const FIntPoint& InTilePosition)
        {
            FOdysseyTileId* TileId = Tiles.Find(InTilePosition);
            if (!TileId)
                return FOdysseyTileId();

            return *TileId;
        }
    );

    TArray<FOdysseyTileManager::FCreatedTile> CreatedTiles = FOdysseyTileManager::Get().CreateOrUpdateTiles(
        InTexture,
        InRect,
        InPosition,
        TileSize,
        GetPixelFormat(),
        GetExistingTileId
    );

    for (const FOdysseyTileManager::FCreatedTile& CreatedTile : CreatedTiles)
    {
        //If the created tile is not valid
        //then we consider it is an empty tile
        if (CreatedTile.IsEmpty())
            continue;

        FOdysseyTileId& TileId = Tiles.FindOrAdd(CreatedTile.Pos);
        TileId = CreatedTile.Id;
    }
}

void
UOdysseyTiledImage::Render(UTextureRenderTarget2D* OutRenderTarget, FIntRect InRect, FIntPoint InPosition) const
{
    FIntRect RenderTargetRect(0, 0, OutRenderTarget->GetSurfaceWidth(), OutRenderTarget->GetSurfaceHeight());
    FIntRect DestinationRect(
        InPosition,
        InPosition + InRect.Size()
    );
    DestinationRect.Clip(RenderTargetRect);

    //Load needed Tiles
    TArray<FIntPoint> TilePositions = Odyssey::TileUtils::GetTilePositionsFromRect(TileSize, InRect);
    TArray<FSharedBuffer> TileBuffers;
    TileBuffers.Reserve(TilePositions.Num());
    for (const FIntPoint& TilePosition : TilePositions)
    {
        FSharedBuffer TileBuffer;
        const FOdysseyTileId* TileId = Tiles.Find(TilePosition);
        if (TileId)
            FOdysseyTileManager::Get().GetTileBuffer(*TileId, TileBuffer);

        TileBuffers.Add(TileBuffer);
    }

    ENQUEUE_RENDER_COMMAND(UOdysseyTiledImage_Render)(
        [
            OutRenderTarget,
            TilePositions,
            TileBuffers,
            TileSize = TileSize,
            TilePixelFormat = GetPixelFormat(),
            DestinationRect,
            SourceRect = InRect
        ](FRHICommandListImmediate& RHICmdList)
        {
            SCOPE_CYCLE_COUNTER(STAT_Render);
            DECLARE_GPU_STAT(UOdysseyTiledImage_Render);

            FRDGBuilder GraphBuilder(RHICmdList);
            FRDGTextureRef DestinationTexture = OutRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture( GraphBuilder );

            FIntRect TileRect(0, 0, TileSize, TileSize);

            for (int i = 0; i < TilePositions.Num(); i++)
            {
                const FIntPoint& TilePosition = TilePositions[i];
                const FSharedBuffer& TileBuffer = TileBuffers[i];

                FIntPoint TileDstPos(TilePosition * TileRect.Size() - SourceRect.Min + DestinationRect.Min);
                FIntRect TileDstRect(TileDstPos, TileDstPos + TileRect.Size());
                TileDstRect.Clip(DestinationRect);

                FIntPoint TileSrcPos(
                    TileDstRect.Min.X - TileDstPos.X,
                    TileDstRect.Min.Y - TileDstPos.Y
                );
                FIntRect TileSrcRect(
                    TileSrcPos,
                    TileSrcPos + TileDstRect.Size()
                );

                //If the buffer is null, the tile is empty
                if (TileBuffer.IsNull())
                {
#ifdef UE_BUILD_DEBUG
                    FLinearColor ClearColor = FLinearColor::Red;
#else
                    FLinearColor ClearColor = FLinearColor::Transparent;
#endif
                    AddClearRenderTargetPass(GraphBuilder, DestinationTexture, ClearColor, TileDstRect);
                }
                else
                {
                    FRDGTextureDesc TileTextureDesc = FRDGTextureDesc::Create2D(
                        FIntPoint(TileSize, TileSize),
                        TilePixelFormat,
                        FClearValueBinding::Transparent,
                        ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable
                    );

                    FRDGTextureRef TileTexture = GraphBuilder.CreateTexture(TileTextureDesc, TEXT("UOdysseyTiledImage::TileTexture"));

                    AddWriteTilePass(
                        GraphBuilder,
                        TileTexture,
                        TileBuffer
                    );

                    AddDrawTexturePass(
                        GraphBuilder,
                        FScreenPassViewInfo(),
                        TileTexture,
                        DestinationTexture,
                        TileSrcRect.Min,
                        TileSrcRect.Size(),
                        TileDstRect.Min,
                        TileDstRect.Size()
                    );
                }
            }

            GraphBuilder.Execute();
        }
    );
}

void
UOdysseyTiledImage::Serialize(FArchive& Ar)
{

}
