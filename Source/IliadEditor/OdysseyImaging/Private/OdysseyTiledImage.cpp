// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTiledImage.h"

#include "Engine/Texture.h"
#include "Engine/TextureRenderTarget2D.h"
#include "RenderGraphUtils.h"
#include "ScreenPass.h"
#include "TextureResource.h"

#include "OdysseyTileUtils.h"
#include "OdysseyTileManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogOdysseyTiledImage, Log, All);

UOdysseyTiledImageFactoryNew::UOdysseyTiledImageFactoryNew()
{
    bCreateNew = true;
    bEditAfterNew = true;
    SupportedClass = UOdysseyTiledImage::StaticClass();
}

UObject*
UOdysseyTiledImageFactoryNew::FactoryCreateNew( UClass* iClass, UObject* iParent, FName iName, EObjectFlags iFlags, UObject* iContext, FFeedbackContext* iWarn )
{
    return NewObject<UOdysseyTiledImage>( iParent, iName, iFlags | RF_Transactional );
}

bool
UOdysseyTiledImageFactoryNew::ShouldShowInNewMenu() const
{
    return false;
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
    if (!Odyssey::TileUtils::IsValidTileSize(InTileSize)
        || InFormat > EOdysseyTiledImageFormat::RGBA32F)
    {
        UE_LOG(LogOdysseyTiledImage, Warning, TEXT("Invalid tiled image size or format: %d, %d."), InTileSize, int32(InFormat));
        return;
    }

    TileSize = InTileSize;
    Format = InFormat;

    //DEBUG only : Allows to undo a TiledImage created in a blueprint
    SetFlags(GetFlags() | RF_Public | RF_Transactional);

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
UOdysseyTiledImage::PostEditUndo()
{
    OnChanged.Broadcast();
}

void
UOdysseyTiledImage::CopyFromTexture(UTexture* InTexture, FIntRect InRect, FIntPoint InPosition)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyTiledImage::CopyFromTexture);
    if (!InTexture || !Odyssey::TileUtils::IsValidTileSize(TileSize))
    {
        return;
    }
    Modify();

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
        FOdysseyTileId& TileId = Tiles.FindOrAdd(CreatedTile.Pos);
        TileId = CreatedTile.Id;
    }

    OnChanged.Broadcast();
}

void
UOdysseyTiledImage::Render(UTextureRenderTarget2D* OutRenderTarget, FIntRect InRect, FIntPoint InPosition) const
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyTiledImage::Render);

    if (!OutRenderTarget || !Odyssey::TileUtils::IsValidTileSize(TileSize)
        || InRect.Width() <= 0 || InRect.Height() <= 0)
    {
        return;
    }

    FIntRect RenderTargetRect(0, 0, OutRenderTarget->GetSurfaceWidth(), OutRenderTarget->GetSurfaceHeight());
    FIntRect DestinationRect(
        InPosition,
        InPosition + InRect.Size()
    );
    DestinationRect.Clip(RenderTargetRect);
    if (DestinationRect.Width() <= 0 || DestinationRect.Height() <= 0)
    {
        return;
    }

    const FIntRect VisibleSourceRect = DestinationRect + (InRect.Min - InPosition);

    //Load needed Tiles
    TArray<FIntPoint> TilePositions = Odyssey::TileUtils::GetTilePositionsFromRect(TileSize, VisibleSourceRect);
    TArray<FOdysseyTileId> TileIds;
    TileIds.Reserve(TilePositions.Num());
    for (const FIntPoint& TilePosition : TilePositions)
    {
        const FOdysseyTileId* TileId = Tiles.Find(TilePosition);
        TileIds.Add(TileId ? *TileId : FOdysseyTileId());
    }

    TArray<TSharedFuture<TSharedPtr<FOdysseyTileManager::FTileTextureHandle>>> TileTextureHandles = FOdysseyTileManager::Get().GetTileTextures(TileIds, TileSize, GetPixelFormat());

    ENQUEUE_RENDER_COMMAND(UOdysseyTiledImage_Render)(
        [
            OutRenderTarget,
            TilePositions,
            TileTextureHandles = MoveTemp(TileTextureHandles),
            TileSize = TileSize,
            TilePixelFormat = GetPixelFormat(),
            DestinationRect,
            DestinationPosition = InPosition,
            SourceRect = InRect
        ](FRHICommandListImmediate& RHICmdList)
        {
            TRACE_CPUPROFILER_EVENT_SCOPE(Odyssey::TiledImage::Render);

            FRDGBuilder GraphBuilder(RHICmdList);
            FRDGTextureRef DestinationTexture = OutRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture( GraphBuilder );

            const FGlobalShaderMap* ShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
            FIntRect TileRect(0, 0, TileSize, TileSize);

            for (int i = 0; i < TilePositions.Num(); i++)
            {
                const FIntPoint& TilePosition = TilePositions[i];
                FIntPoint TileDstPos(TilePosition * TileRect.Size() - SourceRect.Min + DestinationPosition);
                FIntRect TileDstRect(TileDstPos, TileDstPos + TileRect.Size());
                TileDstRect.Clip(DestinationRect);
                if (TileDstRect.Width() <= 0 || TileDstRect.Height() <= 0)
                {
                    continue;
                }

                FIntPoint TileSrcPos(
                    TileDstRect.Min.X - TileDstPos.X,
                    TileDstRect.Min.Y - TileDstPos.Y
                );
                FIntRect TileSrcRect(
                    TileSrcPos,
                    TileSrcPos + TileDstRect.Size()
                );

                //If the buffer is null, the tile is empty
                TSharedPtr<FOdysseyTileManager::FTileTextureHandle> TileTextureHandle;
                if (TileTextureHandles[i].IsValid())
                    TileTextureHandle = TileTextureHandles[i].Get();

                FTextureRHIRef TileTextureRHI;
                FIntPoint TileTexturePosition;
                if (TileTextureHandle.IsValid())
                {
                    TileTextureRHI = TileTextureHandle->GetTextureRHI();
                    TileTexturePosition = TileTextureHandle->GetPositionInTexture();
                }

                if (TileTextureRHI.IsValid())
                {
                    FRDGTextureRef TileTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(TileTextureRHI, TEXT("UOdysseyTiledImage::TileTexture")));

                    AddDrawTexturePass(
                        GraphBuilder,
                        FScreenPassViewInfo(),
                        TileTexture,
                        DestinationTexture,
                        TileSrcRect.Min + TileTexturePosition,
                        TileSrcRect.Size(),
                        TileDstRect.Min,
                        TileSrcRect.Size()
                    );
                }
                else
                {
/*#ifdef UE_BUILD_DEBUG
                    FLinearColor ClearColor = FLinearColor::Red;
#else*/
                    FLinearColor ClearColor = FLinearColor::Transparent;
//#endif
                    AddClearRenderTargetPass(GraphBuilder, DestinationTexture, ClearColor, TileDstRect);
                }
            }

            GraphBuilder.Execute();
        }
    );
}

struct FOdysseyTiledImageObjectVersion
{
    enum Type
    {
        // Before any version changes were made
        SerializeTiles,

        // -----<new versions can be added above this line>-------------------------------------------------
        VersionPlusOne,
        LatestVersion = VersionPlusOne - 1
    };

    // The GUID for this custom version number
    const static FGuid GUID;

private:
    FOdysseyTiledImageObjectVersion() {}
};

const FGuid FOdysseyTiledImageObjectVersion::GUID(0x369704bc, 0xd2364b57, 0x8d73527e, 0x2fe77950);
FDevVersionRegistration GRegisterOdysseyTiledImageObjectVersion(FOdysseyTiledImageObjectVersion::GUID, FOdysseyTiledImageObjectVersion::LatestVersion, TEXT("OdysseyTiledImage"));

void
UOdysseyTiledImage::Serialize(FArchive& Ar)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyTiledImage::Serialize);

    Super::Serialize(Ar);

    //We don't need to save Tiles data for undo
    //(even if UOdysseyTiledImage will probably never be cooked)
    if (Ar.IsTransacting() || !Ar.IsPersistent())
        return;

    //As we are doing a custom serialization
    //We use an identifiable version to prevent future problems
    //if this custom serialization is changed
    Ar.UsingCustomVersion(FOdysseyTiledImageObjectVersion::GUID);

    if (!Odyssey::TileUtils::IsValidTileSize(TileSize) || Format > EOdysseyTiledImageFormat::RGBA32F)
    {
        UE_LOG(LogOdysseyTiledImage, Error, TEXT("Cannot serialize tiled image with invalid size or format."));
        Ar.SetError();
        return;
    }

    TArray<FIntPoint> TilePositions;
    TArray<FCompressedBuffer> TileCompressedBuffers;
    TArray<FIoHash> TileHashes;

    if( Ar.IsSaving() )
    {
        for (const auto& Element : Tiles)
        {
            const FIntPoint& TilePosition = Element.Key;
            const FOdysseyTileId& TileId = Element.Value;

            FCompressedBuffer CompressedBuffer;
            if (!FOdysseyTileManager::Get().GetTileCompressedBuffer(TileId, CompressedBuffer))
            {
                UE_LOG(LogOdysseyTiledImage, Error, TEXT("Cannot save tiled image: tile (%d, %d) could not be read."), TilePosition.X, TilePosition.Y);
                Ar.SetError();
                return;
            }
            // Empty tiles have no payload. An unavailable non-empty tile is an error above.
            if (CompressedBuffer.IsNull())
            {
                continue;
            }

            FIoHash Hash;
            if (!FOdysseyTileManager::Get().GetTileHash(TileId, Hash))
            {
                Ar.SetError();
                return;
            }

            TilePositions.Add(TilePosition);
            TileCompressedBuffers.Add(CompressedBuffer);
            TileHashes.Add(Hash);
        }
    }

    Ar << TilePositions;
    Ar << TileCompressedBuffers;
    Ar << TileHashes;

    if( Ar.IsLoading() )
    {
        if (Ar.IsError() || TilePositions.Num() != TileCompressedBuffers.Num() || TilePositions.Num() != TileHashes.Num())
        {
            Ar.SetError();
            return;
        }

        const uint64 ExpectedTileBytes = uint64(TileSize) * TileSize * GPixelFormats[GetPixelFormat()].BlockBytes;
        for (const FCompressedBuffer& Buffer : TileCompressedBuffers)
        {
            if (Buffer.IsNull() || Buffer.GetRawSize() != ExpectedTileBytes)
            {
                Ar.SetError();
                return;
            }
        }

        Tiles.Empty(TilePositions.Num());

        for (int i = 0; i < TilePositions.Num(); i++)
        {
            const FIntPoint& TilePosition = TilePositions[i];
            const FCompressedBuffer& TileCompressedBuffer = TileCompressedBuffers[i];
            const FIoHash& TileHash = TileHashes[i];

            FOdysseyTileId TileId = FOdysseyTileManager::Get().CreateTile(TileHash, TileCompressedBuffer);
            Tiles.Add(TilePosition, TileId);
        }
    }
}
