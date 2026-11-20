// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "PixelFormat.h"
#include "RenderGraphFwd.h"
#include "Serialization/EditorBulkData.h"

#include "OdysseyTileId.h"

#include "OdysseyTiledImage.generated.h"

class UTexture;
class UTextureRenderTarget2D;

UENUM()
enum class EOdysseyTiledImageFormat : uint8
{
    RGBA8,
    RGBA16F,
    RGBA32F,
};

/**
 * Raster data split in tiles, positioned in space to provide a technically unbounded image.
 *
 * Meant to store the pixels of raster texture layers and raster animation cells, through FOdysseyRasterContent;
 * it is not exposed to Blueprints nor created as an asset. Its tiles are transactional: Modify() makes changes undoable.
 */
UCLASS()
class ODYSSEYIMAGING_API UOdysseyTiledImage
    : public UObject
{
    GENERATED_BODY()

public:
    virtual ~UOdysseyTiledImage();
    UOdysseyTiledImage();

    virtual void PostEditUndo() override;

public:
    /** Sets the tile size and format, and removes every tile. */
    void Initialize(int TileSize, EOdysseyTiledImageFormat Format);

    EOdysseyTiledImageFormat GetFormat() const;

    EPixelFormat GetPixelFormat() const;

    /**
     * Copies an area of a texture into the image, replacing the touched tiles. Calls Modify(), so the copy is undoable within a transaction.
     * @param Texture The source texture.
     * @param Rect The area of the texture to copy.
     * @param Position The position, in image pixels, of the top-left corner of Rect.
     */
    void CopyFromTexture(UTexture* Texture, FIntRect Rect, FIntPoint Position);

    /** Removes every tile, keeping the tile size and format. Calls Modify(), so the removal is undoable within a transaction. */
    void RemoveAllTiles();

    /**
     * Renders an area of the image into a render target. Pixels without a tile are transparent.
     * @param Destination The render target.
     * @param Rect The area of the image to render, in image pixels.
     * @param Position The position of the top-left corner of Rect in the destination.
     */
    void Render(UTextureRenderTarget2D* Destination, FIntRect Rect, FIntPoint Position) const;

    /** Returns the bounds of the tiles of the image, in image pixels; empty when the image has no tile. */
    FIntRect GetTileBounds() const;

    /** Prepare on the game thread, invoke on the render thread in an existing graph.
     * Captures tile handles rather than the UObject, so subsequent edits cannot change this render.
     * The destination position is in pixels; this operation does not scale or blend the image.
     */
    using FRenderFunction = TFunction<void(FRDGBuilder&, FRDGTextureRef, FIntPoint)>;
    FRenderFunction BuildRenderFunction(FIntRect SourceRect) const;

    /**
     * Creates a linear render target whose pixel format is the tile pixel format,
     * so that Render() into it and CopyFromTexture() from it involve no conversion.
     *
     * @param Size The size of the render target, in pixels.
     * @param Outer The outer of the render target; the transient package when null.
     * @return The render target, or null when Size is empty.
     */
    UTextureRenderTarget2D* CreateRenderTarget(FIntPoint Size, UObject* Outer = nullptr) const;

    /** Native notification broadcast after an undo or a redo restored the tiles. */
    DECLARE_MULTICAST_DELEGATE(FOnUndone);
    FOnUndone& OnUndone();

public:
    virtual void Serialize(FArchive& Ar) override;

private:
    UPROPERTY()
    uint32 TileSize = 64;

    /**
     *
     */
    UPROPERTY()
    EOdysseyTiledImageFormat Format = EOdysseyTiledImageFormat::RGBA8;

    /** TODO:
     * Replace this map with a structure
     * optimized for binary searching
     *
     * struct FTileRegistryRedirector
     * {
     *      int Value; //can be X or Y
     *      uint32 index;
     * };
     *
     * struct FTileRegistry
     * {
     *      TArray<FTileRegistryRedirector> YRedirector; //Redirects Y positions to an index in XRedirector
     *      TArray<TArray<FTileRegistryRedirector>> XRedirector; //Redirects (Y*, X) positions to an index in Tiles;
     *      TArray<FTile> Tiles;
     * }
    */

    UPROPERTY(Transient)
    TMap<FIntPoint, FOdysseyTileId> Tiles;

    FOnUndone OnUndoneDelegate;
};
