// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "ULISInvalidTileMap.h"

FULISInvalidTileMap::FULISInvalidTileMap()
    : mTileMap()
    , mTileSize(0)
    , mWidth(0)
    , mHeight(0)
{
}

FULISInvalidTileMap::FULISInvalidTileMap(int iTileSize, int iWidth, int iHeight)
    : mTileMap()
    , mTileSize(iTileSize)
    , mWidth(0)
    , mHeight(0)
{
    Resize(iWidth, iHeight);
    Clear();
}

void
FULISInvalidTileMap::Resize(int iWidth, int iHeight)
{
    mTileMap.Resize(ceil( (float)iWidth / mTileSize ), ceil( (float)iHeight / mTileSize ));
    
    mWidth = iWidth;
    mHeight = iHeight;
}

void
FULISInvalidTileMap::Invalidate(const FIntPoint& iTileIndex)
{
    FIntPoint tileCount = mTileMap.Size();

    if (iTileIndex.X < 0 || iTileIndex.Y < 0 || iTileIndex.X >= tileCount.X || iTileIndex.Y >= tileCount.Y)
        return;

    mTileMap.Set(iTileIndex.X, iTileIndex.Y, false);
}

void
FULISInvalidTileMap::Invalidate(const TArray<FIntPoint>& iTileIndexes)
{
    FIntPoint tileCount = mTileMap.Size();

    for (int i = 0; i < iTileIndexes.Num(); i++)
    {
        const FIntPoint& tileIndex = iTileIndexes[i];
        if (tileIndex.X < 0 || tileIndex.Y < 0 || tileIndex.X >= tileCount.X || tileIndex.Y >= tileCount.Y)
            return;
    }

    for (int i = 0; i < iTileIndexes.Num(); i++)
    {
        const FIntPoint& tileIndex = iTileIndexes[i];
        mTileMap.Set(tileIndex.X, tileIndex.Y, false);
    }
}

void
FULISInvalidTileMap::Invalidate(const ::ULIS::FRectI& iRect)
{
    if (iRect.x >= mWidth || iRect.x + iRect.w < 0 || iRect.y >= mHeight || iRect.y + iRect.y < 0)
        return;

    float xf = float( iRect.x ) / mTileSize;
    float yf = float( iRect.y ) / mTileSize;
    float wf = float( iRect.w ) / mTileSize;
    float hf = float( iRect.h ) / mTileSize;

    FIntPoint tileCount = mTileMap.Size();

    int x = xf;
    int y = yf;
    int w = FMath::Min( tileCount.X, int( ceil( xf + wf ) ) ) - x;
    int h = FMath::Min( tileCount.Y, int( ceil( yf + hf ) ) ) - y;

    for( int i = 0; i < h; ++i ) //y
    {
        for( int j = 0; j < w; ++j ) //x
        {
            mTileMap.Set(x+j, y+i, false);
        }
    }
}

void
FULISInvalidTileMap::Invalidate(const TArray<::ULIS::FRectI>& iRects)
{
    for (int i = 0; i < iRects.Num(); i++)
    {
        Invalidate(iRects[i]);
    }
}

void
FULISInvalidTileMap::Clear()
{
    FIntPoint tileCount = mTileMap.Size();

    for( int y = 0; y < tileCount.Y; ++y )
    {
        for( int x = 0; x < tileCount.X; ++x )
        {
            mTileMap.Set(x, y, true);
        }
    }
}

::ULIS::FRectI
FULISInvalidTileMap::GetTileRect(const FIntPoint& iTileIndex) const
{
    return ::ULIS::FRectI(
        iTileIndex.X * mTileSize,
        iTileIndex.Y * mTileSize,
        FMath::Min( (iTileIndex.X + 1) * mTileSize, mWidth) - iTileIndex.X * mTileSize,
        FMath::Min( (iTileIndex.Y + 1) * mTileSize, mHeight) - iTileIndex.Y * mTileSize
    );
}

bool
FULISInvalidTileMap::IsValidTile(const FIntPoint& iTileIndex) const
{
    return mTileMap.Get(iTileIndex.X, iTileIndex.Y);
}

TArray<FIntPoint>
FULISInvalidTileMap::InvalidTiles() const
{
    TArray<FIntPoint> invalidTiles;
    FIntPoint tileCount = mTileMap.Size();

    for( int y = 0; y < tileCount.Y; ++y )
    {
        for( int x = 0; x < tileCount.X; ++x )
        {
            if (!mTileMap.Get(x, y))
            {
                invalidTiles.Add(FIntPoint(x, y));
            }
        }
    }

    return invalidTiles;
}

TArray<::ULIS::FRectI>
FULISInvalidTileMap::InvalidRects() const
{
    TArray<::ULIS::FRectI> invalidRects;
    FIntPoint tileCount = mTileMap.Size();

    for( int y = 0; y < tileCount.Y; ++y )
    {
        for( int x = 0; x < tileCount.X; ++x )
        {
            if (!mTileMap.Get(x, y))
            {
                invalidRects.Add(
                    GetTileRect({x, y})
                );
            }
        }
    }

    return invalidRects;
}

int
FULISInvalidTileMap::Width() const
{
    return mWidth;
}

int
FULISInvalidTileMap::Height() const
{
    return mHeight;
}

int
FULISInvalidTileMap::TileSize() const
{
    return mTileSize;
}