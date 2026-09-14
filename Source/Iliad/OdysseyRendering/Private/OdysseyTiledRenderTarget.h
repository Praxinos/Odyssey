// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Math/IntRect.h"
#include "Math/IntPoint.h"
#include "Containers/OdysseyArray2D.h"
#include "Engine/TextureRenderTarget2D.h"

class FOdysseyTiledRenderTarget
{
public:
    FOdysseyTiledRenderTarget();

    /** Constructor
     *
     * Tiles are assumed to be squares
     *
     * @param iTileSize Size of a single tile in pixels
     * @param iWidth Image Width
     * @param iHeight Image Height
    */
    FOdysseyTiledRenderTarget(int iTileSize, int iWidth, int iHeight, ETextureRenderTargetFormat iRenderTargetFormat);

public:
    void Draw(UTexture* Source, FIntRect SourceRect, FIntPoint DestinationPosition);

    void Draw(UTexture* Source, TArray<FIntPoint> TileIndexes);

    //void Draw(FOdysseyTiledRenderTarget Source, FIntRect SourceRect, FIntPoint DestinationPosition);

    /** Clean all the tiles (DOES NOT destroy underlying render targets) */
    void Clean();

    /** Clear all the tiles (destroys underlying render targets) */
    void Clear();

    /** Returns the rect affected by the tile at iTileIndex
     *
     * @param iTileIndex the tileIndex (X, Y) of the tile to retrieve the rect from
     * @return The rect corresponding to the given tileindex
     */
    FIntRect GetTileRect(const FIntPoint& iTileIndex) const;

    /** Returns the rect affected by the tile at iTileIndex
     *
     * @param iTileIndex the tileIndex (X, Y) of the tile to retrieve the rect from
     * @return The rect corresponding to the given tileindex
     */
    UTexture* GetTileTexture(const FIntPoint& iTileIndex) const;

    /** Wether the given tile index is valid
     *
     * @param iTileIndex the index of the tile
     * @return True if the tile is valid, false otherwise
    */
    bool IsTileDirty(const FIntPoint& iTileIndex) const;

    /** Get the list of all invalid tiles indexes
     *
     * @return an array of points containing the X and Y indexes of all invalid tiles
    */
    TArray<FIntPoint> GetDirtyTileIndexes() const;

    /** Get the list of all invalid rectangles in image coordinates system
     *
     * @return an array of rects corresponding to all invalid tiles in image coordinates system
    */
    TArray<FIntRect> GetDirtyRects() const;

    void GetDirtyTiles(TArray<FIntPoint>& TileIndexes, TArray<FIntRect>& Rects, TArray<UTexture*>& Textures ) const;

    TArray<FIntPoint> GetTileIndexesInRect(const FIntRect& Rect) const;

public:
    // Getters

    /* Gets the Width of the TileMap
    *
    * @return Width of the tilemap
    */
    int GetWidth() const;

    /* Gets the Height of the TileMap
    *
    * @return Height of the tilemap
    */
    int GetHeight() const;

    /* Gets the Tile Size of the TileMap
    *
    * @return Tile Size of the tilemap
    */
    int GetTileSize() const;

private:
    struct FTile
    {
        TStrongObjectPtr<UTextureRenderTarget2D> RenderTarget;
        bool IsDirty = false;
    };

    TOdysseyArray2D<FTile> mTileMap;
    int mTileSize = 0;
    int mWidth = 0;
    int mHeight = 0;
    ETextureRenderTargetFormat mRenderTargetFormat = RTF_RGBA8;
};
