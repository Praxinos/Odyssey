// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "ULISInvalidTileMap.h"

FULISInvalidTileMap::FULISInvalidTileMap()
    : mTileMap()
    , mTileSize(0)
    , mWidth(0)
    , mHeight(0)
    , mIsFullyInvalidated(false)
{
}

FULISInvalidTileMap::FULISInvalidTileMap(int iTileSize, int iWidth, int iHeight)
    : mTileMap()
    , mTileSize(iTileSize)
    , mWidth(0)
    , mHeight(0)
    , mIsFullyInvalidated(false)
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
FULISInvalidTileMap::Invalidate()
{
    mIsFullyInvalidated = true;
}

void
FULISInvalidTileMap::Invalidate(const FIntPoint& iTileIndex)
{
    if (mIsFullyInvalidated)
        return;

    FIntPoint tileCount = mTileMap.Size();

    if (iTileIndex.X < 0 || iTileIndex.Y < 0 || iTileIndex.X >= tileCount.X || iTileIndex.Y >= tileCount.Y)
        return;

    mTileMap.Set(iTileIndex.X, iTileIndex.Y, false);
}

void
FULISInvalidTileMap::Invalidate(const TArray<FIntPoint>& iTileIndexes)
{
    if (mIsFullyInvalidated)
        return;

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
    if (mIsFullyInvalidated)
        return;

    if (iRect.x >= mWidth || iRect.x + iRect.w < 0 || iRect.y >= mHeight || iRect.y + iRect.h < 0)
        return;

    if (iRect.x <= 0 && iRect.y <= 0 && iRect.x + iRect.w >= mWidth && iRect.y + iRect.h >= mHeight)
    {
        mIsFullyInvalidated = true;
        return;
    }

    float xf = float( iRect.x ) / mTileSize;
    float yf = float( iRect.y ) / mTileSize;
    float wf = float( iRect.w ) / mTileSize;
    float hf = float( iRect.h ) / mTileSize;

    FIntPoint tileCount = mTileMap.Size();

    int x = FMath::Max(0, xf);
    int y = FMath::Max(0, yf);
    int w = FMath::Min( tileCount.X, int( ceil( xf + wf ) ) ) - abs((int)xf);
    int h = FMath::Min( tileCount.Y, int( ceil( yf + hf ) ) ) - abs((int)yf);


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
    if (mIsFullyInvalidated)
        return;

    for (int i = 0; i < iRects.Num(); i++)
    {
        Invalidate(iRects[i]);
    }
}

void
FULISInvalidTileMap::Clear()
{
    mIsFullyInvalidated = false;

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
    return !mIsFullyInvalidated && mTileMap.Get(iTileIndex.X, iTileIndex.Y);
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
            if (mIsFullyInvalidated || !mTileMap.Get(x, y))
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
    if (mIsFullyInvalidated)
        return { ::ULIS::FRectI::FromXYWH(0, 0, mWidth, mHeight) };

    TArray<::ULIS::FRectI> invalidRects;
    FIntPoint tileCount = mTileMap.Size();

    TArray<int> prevRectIndexes;
    for( int y = 0; y < tileCount.Y; ++y )
    {
        TArray<::ULIS::FRectI> rects;
        bool wasInvalid = false;
        for( int x = 0; x < tileCount.X; ++x )
        {
            bool isInvalid = !mTileMap.Get(x, y);
            if (isInvalid)
            {
                ::ULIS::FRectI rect = GetTileRect({x, y});
                if (wasInvalid)
                {
                    rects.Last().w += rect.w;
                }
                else
                {
                    rects.Add(rect);
                }
            }
            wasInvalid = isInvalid;
        }

        TArray<int> rectIndexes;
        for (int i = 0; i < rects.Num(); i++)
        {
            const ::ULIS::FRectI& rect = rects[i];
            bool prevRectExtended = false;
            for (int prevIdx : prevRectIndexes)
            {
                ::ULIS::FRectI& prevRect = invalidRects[prevIdx];
                if (rect.x == prevRect.x && rect.w == prevRect.w )
                {
                    prevRect.h += rect.h;
                    rectIndexes.Add(prevIdx);
                    prevRectExtended = true;
                    break;
                }
            }

            if (!prevRectExtended)
            {
                invalidRects.Add(rect);
                rectIndexes.Add(invalidRects.Num() - 1);
            }
        }
        prevRectIndexes = rectIndexes;
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
