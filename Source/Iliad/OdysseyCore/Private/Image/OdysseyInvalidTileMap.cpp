// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyInvalidTileMap.h"

FOdysseyInvalidTileMap::FOdysseyInvalidTileMap()
    : mTileMap()
    , mTileSize(0)
    , mWidth(0)
    , mHeight(0)
    , mIsFullyInvalidated(false)
{
}

FOdysseyInvalidTileMap::FOdysseyInvalidTileMap(int iTileSize, int iWidth, int iHeight)
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
FOdysseyInvalidTileMap::Resize(int iWidth, int iHeight)
{
    mTileMap.Resize(ceil( (float)iWidth / mTileSize ), ceil( (float)iHeight / mTileSize ));

    mWidth = iWidth;
    mHeight = iHeight;
}

void
FOdysseyInvalidTileMap::Invalidate()
{
    mIsFullyInvalidated = true;
}

void
FOdysseyInvalidTileMap::Invalidate(const FIntPoint& iTileIndex)
{
    if (mIsFullyInvalidated)
        return;

    FIntPoint tileCount = mTileMap.Size();

    if (iTileIndex.X < 0 || iTileIndex.Y < 0 || iTileIndex.X >= tileCount.X || iTileIndex.Y >= tileCount.Y)
        return;

    mTileMap.Set(iTileIndex.X, iTileIndex.Y, false);
}

void
FOdysseyInvalidTileMap::Invalidate(const TArray<FIntPoint>& iTileIndexes)
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
FOdysseyInvalidTileMap::Invalidate(const FIntRect& iRect)
{
    if (mIsFullyInvalidated)
        return;

    if (iRect.Min.X >= mWidth || iRect.Max.X < 0 || iRect.Min.Y >= mHeight || iRect.Max.Y < 0)
        return;

    if (iRect.Min.X <= 0 && iRect.Min.Y <= 0 && iRect.Max.X >= mWidth && iRect.Max.Y >= mHeight)
    {
        mIsFullyInvalidated = true;
        return;
    }

    float x1f = float( iRect.Min.X ) / mTileSize;
    float y1f = float( iRect.Min.Y ) / mTileSize;
    float x2f = float( iRect.Max.X ) / mTileSize;
    float y2f = float( iRect.Max.Y ) / mTileSize;

    FIntPoint tileCount = mTileMap.Size();

    int x = FMath::Max(0, x1f);
    int y = FMath::Max(0, y1f);
    int w = FMath::Min( tileCount.X, int( ceil( x2f ) ) ) - abs((int)x1f);
    int h = FMath::Min( tileCount.Y, int( ceil( y2f ) ) ) - abs((int)y1f);

    for( int i = 0; i < h; ++i ) //y
    {
        for( int j = 0; j < w; ++j ) //x
        {
            mTileMap.Set(x+j, y+i, false);
        }
    }
}

void
FOdysseyInvalidTileMap::Invalidate(const TArray<FIntRect>& iRects)
{
    if (mIsFullyInvalidated)
        return;

    for (int i = 0; i < iRects.Num(); i++)
    {
        Invalidate(iRects[i]);
    }
}

void
FOdysseyInvalidTileMap::Clear()
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

FIntRect
FOdysseyInvalidTileMap::GetTileRect(const FIntPoint& iTileIndex) const
{
    return FIntRect(
        iTileIndex.X * mTileSize,
        iTileIndex.Y * mTileSize,
        FMath::Min( (iTileIndex.X + 1) * mTileSize, mWidth),
        FMath::Min( (iTileIndex.Y + 1) * mTileSize, mHeight)
    );
}

bool
FOdysseyInvalidTileMap::IsValidTile(const FIntPoint& iTileIndex) const
{
    return !mIsFullyInvalidated && mTileMap.Get(iTileIndex.X, iTileIndex.Y);
}

TArray<FIntPoint>
FOdysseyInvalidTileMap::InvalidTiles() const
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

TArray<FIntRect>
FOdysseyInvalidTileMap::InvalidRects() const
{
    if (mIsFullyInvalidated)
        return { FIntRect(0, 0, mWidth, mHeight) };

    TArray<FIntRect> invalidRects;
    FIntPoint tileCount = mTileMap.Size();

    TArray<int> prevRectIndexes;
    for( int y = 0; y < tileCount.Y; ++y )
    {
        TArray<FIntRect> rects;
        bool wasInvalid = false;
        for( int x = 0; x < tileCount.X; ++x )
        {
            bool isInvalid = !mTileMap.Get(x, y);
            if (isInvalid)
            {
                FIntRect rect = GetTileRect({x, y});
                if (wasInvalid)
                {
                    rects.Last().Max.X += rect.Width();
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
            const FIntRect& rect = rects[i];
            bool prevRectExtended = false;
            for (int prevIdx : prevRectIndexes)
            {
                FIntRect& prevRect = invalidRects[prevIdx];
                if (rect.Min.X == prevRect.Min.X && rect.Width() == prevRect.Width() )
                {
                    prevRect.Max.Y += rect.Height();
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
FOdysseyInvalidTileMap::Width() const
{
    return mWidth;
}

int
FOdysseyInvalidTileMap::Height() const
{
    return mHeight;
}

int
FOdysseyInvalidTileMap::TileSize() const
{
    return mTileSize;
}
