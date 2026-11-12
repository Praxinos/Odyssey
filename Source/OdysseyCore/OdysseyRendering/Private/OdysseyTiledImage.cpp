// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTiledImage.h"

FOdysseyTiledImage::FOdysseyTiledImage()
{
}

FOdysseyTiledImage::FOdysseyTiledImage(int iTileSize, int iWidth, int iHeight, ETextureRenderTargetFormat iRenderTargetFormat)
    : mTileMap(ceil( (float)iWidth / iTileSize ), ceil( (float)iHeight / iTileSize ))
    , mTileSize(iTileSize)
    , mWidth(iWidth)
    , mHeight(iHeight)
    , mRenderTargetFormat(iRenderTargetFormat)
{
}

void
FOdysseyTiledImage::Draw(UTexture* Source, const TArray<FIntRect> Rects, FIntPoint Position)
{
    //Draw to tiles render targets
    //Convert tiles to FImage asynchronously
}

void
FOdysseyTiledImage::Render(UTextureRenderTarget2D* Destination, FIntRect Rect, FIntPoint Position) const
{
    //Draw using RenderTarget if possible
    //Otherwise draw by uploading FImage in a temporary rendertarget
}

int
FOdysseyTiledImage::GetWidth() const
{
    return mWidth;
}

int
FOdysseyTiledImage::GetHeight() const
{
    return mHeight;
}

ETextureRenderTargetFormat
FOdysseyTiledImage::GetRenderTargetFormat() const
{
    return mRenderTargetFormat;
}
