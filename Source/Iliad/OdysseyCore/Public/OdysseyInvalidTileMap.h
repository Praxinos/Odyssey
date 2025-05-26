// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Math/IntRect.h"
#include "Math/IntPoint.h"
#include "Containers/OdysseyArray2D.h"

class ODYSSEYCORE_API FOdysseyInvalidTileMap
{
public:
    /** Constructor
     *
     * Tiles are assumed to be squares
     *
     * @param iTileSize Size of a single tile in pixels
     * @param iWidth Image Width
     * @param iHeight Image Height
    */
    FOdysseyInvalidTileMap();

    /** Constructor
     *
     * Tiles are assumed to be squares
     *
     * @param iTileSize Size of a single tile in pixels
     * @param iWidth Image Width
     * @param iHeight Image Height
    */
    FOdysseyInvalidTileMap(int iTileSize, int iWidth, int iHeight);

public:
    /** Resize the underlying image size
     *
     * @param iWidth Image Width
     * @param iHeight Image Height
    */
    void Resize(int iWidth, int iHeight);

    /** Invalidates the whole map
     */
    void Invalidate();

    /** Invalidates the tile at iTileIndex
     *
     * @param iTileIndex the index (X, Y) of the tile to invalidate
     */
    void Invalidate(const FIntPoint& iTileIndex);

    /** Invalidates the tiles at iTileIndexes
     *
     * @param iTileIndexes the indexes (X, Y) of the tiles to invalidate
     */
    void Invalidate(const TArray<FIntPoint>& iTileIndexes);

    /** Invalidates the tiles affected by iRect
     *
     * @param iRect the rectangle to invalidate in the image coordinate system
     */
    void Invalidate(const FIntRect& iRect);

    /** Invalidates the tiles affected by iRects
     *
     * @param iRect the rectangles to invalidate in the image coordinate system
     */
    void Invalidate(const TArray<FIntRect>& iRects);

    /** Clear all the tiles (Makes all the tiles valid) */
    void Clear();

    /** Returns the rect affected by the tile at iTileIndex
     *
     * @param iTileIndex the tileIndex (X, Y) of the tile to retrieve the rect from
     * @return The rect corresponding to the given tileindex
     */
    FIntRect GetTileRect(const FIntPoint& iTileIndex) const;

    /** Wether the given tile index is valid
     *
     * @param iTileIndex the index of the tile
     * @return True if the tile is valid, false otherwise
    */
    bool IsValidTile(const FIntPoint& iTileIndex) const;

    /** Get the list of all invalid tiles indexes
     *
     * @return an array of points containing the X and Y indexes of all invalid tiles
    */
    TArray<FIntPoint> InvalidTiles() const;

    /** Get the list of all invalid rectangles in image coordinates system
     *
     * @return an array of rects corresponding to all invalid tiles in image coordinates system
    */
    TArray<FIntRect> InvalidRects() const;

public:
    // Getters

    /* Gets the Width of the TileMap
    *
    * @return Width of the tilemap
    */
    int Width() const;

    /* Gets the Height of the TileMap
    *
    * @return Height of the tilemap
    */
    int Height() const;

    /* Gets the Tile Size of the TileMap
    *
    * @return Tile Size of the tilemap
    */
    int TileSize() const;

private:
    TOdysseyArray2D<bool> mTileMap;
    int mTileSize;
    int mWidth;
    int mHeight;
    bool mIsFullyInvalidated;
};
