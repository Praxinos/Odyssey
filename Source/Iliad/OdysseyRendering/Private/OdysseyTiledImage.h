// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Math/IntRect.h"
#include "Math/IntPoint.h"
#include "Containers/OdysseyArray2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "OdysseyInvalidTileMap.h"

class FOdysseyTiledImage
{
public:
    FOdysseyTiledImage();

    /** Constructor
     *
     * Tiles are assumed to be squares
     *
     * @param iTileSize Size of a single tile in pixels
     * @param iWidth Image Width
     * @param iHeight Image Height
    */
    FOdysseyTiledImage(int iTileSize, int iWidth, int iHeight, ETextureRenderTargetFormat iRenderTargetFormat);

public:
    /** Draw the Rects of Source at Position */
    void Draw(UTexture* Source, const TArray<FIntRect> Rects, FIntPoint Position = FIntPoint(0, 0));

    /** Renders the Rects of this image into Destinaion at Position */
    void Render(UTextureRenderTarget2D* Destination, FIntRect Rect, FIntPoint Position = FIntPoint(0, 0)) const;

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

    /*
        Gets the Render Target Format
    */
    ETextureRenderTargetFormat GetRenderTargetFormat() const;

private:
    struct FTile
    {
        TStrongObjectPtr<UTextureRenderTarget2D> RenderTarget;
        FImage mTile;
    };

    TOdysseyArray2D<FTile> mTileMap;
    int mTileSize = 0;
    int mWidth = 0;
    int mHeight = 0;
    ETextureRenderTargetFormat mRenderTargetFormat = RTF_RGBA8;

    FOdysseyInvalidTileMap mExistingTiles;
};
