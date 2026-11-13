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

UOdysseyTiledImage::FRenderFunction UOdysseyTiledImage::BuildRenderFunction(FIntRect SourceRect) const
{
    check(IsInGameThread());
    if (!Odyssey::TileUtils::IsValidTileSize(TileSize) || SourceRect.IsEmpty())
    {
        return {};
    }

    TArray<FIntPoint> TilePositions = Odyssey::TileUtils::GetTilePositionsFromRect(TileSize, SourceRect);
    TArray<FOdysseyTileId> TileIds;
    TileIds.Reserve(TilePositions.Num());
    for (const FIntPoint& TilePosition : TilePositions)
    {
        const FOdysseyTileId* TileId = Tiles.Find(TilePosition);
        TileIds.Add(TileId ? *TileId : FOdysseyTileId());
    }
    auto TileHandles = FOdysseyTileManager::Get().GetTileTextures(TileIds, TileSize, GetPixelFormat());
    return [TilePositions = MoveTemp(TilePositions), TileHandles = MoveTemp(TileHandles),
        TileSize = TileSize, SourceRect](FRDGBuilder& GraphBuilder, FRDGTextureRef Destination, FIntPoint Position)
    {
        RDG_EVENT_SCOPE(GraphBuilder, "Odyssey.TiledImage.Render");
        FIntRect DestinationRect(Position, Position + SourceRect.Size());
        DestinationRect.Clip(FIntRect(FIntPoint::ZeroValue, Destination->Desc.Extent));
        if (DestinationRect.IsEmpty())
        {
            return;
        }
        for (int32 Index = 0; Index < TilePositions.Num(); ++Index)
        {
            const FIntPoint TileOrigin = TilePositions[Index] * TileSize - SourceRect.Min + Position;
            FIntRect TileDestination(TileOrigin, TileOrigin + FIntPoint(TileSize, TileSize));
            TileDestination.Clip(DestinationRect);
            if (TileDestination.IsEmpty())
            {
                continue;
            }
            const auto Handle = TileHandles[Index].IsValid() ? TileHandles[Index].Get() : nullptr;
            if (Handle && Handle->GetTextureRHI().IsValid())
            {
                FRDGTextureRef TileTexture = GraphBuilder.RegisterExternalTexture(
                    CreateRenderTarget(Handle->GetTextureRHI(), TEXT("Odyssey.TiledImage.Tile")));
                AddDrawTexturePass(GraphBuilder, FScreenPassViewInfo(), TileTexture, Destination,
                    TileDestination.Min - TileOrigin + Handle->GetPositionInTexture(), TileDestination.Size(),
                    TileDestination.Min, TileDestination.Size());
            }
            else
            {
                AddClearRenderTargetPass(GraphBuilder, Destination, FLinearColor::Transparent, TileDestination);
            }
        }
    };
}

void UOdysseyTiledImage::Render(UTextureRenderTarget2D* Destination, FIntRect Rect, FIntPoint Position) const
{
    if (!Destination)
    {
        return;
    }
    // Clip before preparing handles, so offscreen tiles are not loaded.
    FIntRect DestinationRect(Position, Position + Rect.Size());
    DestinationRect.Clip(FIntRect(0, 0, Destination->GetSurfaceWidth(), Destination->GetSurfaceHeight()));
    if (DestinationRect.IsEmpty())
    {
        return;
    }
    FRenderFunction RenderFunction = BuildRenderFunction(DestinationRect + (Rect.Min - Position));
    if (!RenderFunction)
    {
        return;
    }
    ENQUEUE_RENDER_COMMAND(OdysseyTiledImageRender)(
        [Destination, Position = DestinationRect.Min, RenderFunction = MoveTemp(RenderFunction)]
        (FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder GraphBuilder(RHICmdList);
            RenderFunction(GraphBuilder, Destination->GetRenderTargetResource()->GetRenderTargetTexture(GraphBuilder), Position);
            GraphBuilder.Execute();
        });
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
