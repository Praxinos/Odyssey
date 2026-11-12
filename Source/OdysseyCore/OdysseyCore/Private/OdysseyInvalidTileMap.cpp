// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
    mTileMap.Resize(
        FMath::Max(1, ((iWidth - 1) / mTileSize) + 1),
        FMath::Max(1, ((iHeight - 1) / mTileSize) + 1)
    );

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

    TArray<FIntPoint> tileIndexes = GetTileIndexesForRect(iRect);
    for (const FIntPoint& tileIndex : tileIndexes)
    {
        mTileMap.Set(tileIndex.X, tileIndex.Y, false);
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

TArray<FIntPoint>
FOdysseyInvalidTileMap::GetTileIndexesForRect(const FIntRect& iRect) const
{
    if (iRect.Min.X >= mWidth || iRect.Max.X < 0 || iRect.Min.Y >= mHeight || iRect.Max.Y < 0)
        return {};

    FIntPoint tileCount = mTileMap.Size();

    int x1 = FMath::Clamp(iRect.Min.X / mTileSize, 0, tileCount.X - 1);
    int y1 = FMath::Clamp(iRect.Min.Y / mTileSize, 0, tileCount.Y - 1);
    int x2 = FMath::Clamp(iRect.Max.X / mTileSize, 0, tileCount.X - 1);
    int y2 = FMath::Clamp(iRect.Max.Y / mTileSize, 0, tileCount.Y - 1);

    int w = x2 - x1 + 1;
    int h = y2 - y1 + 1;

    if (w <= 0 || h <= 0)
        return {};

    TArray<FIntPoint> tileIndexes;
    tileIndexes.Reserve(w * h);
    for( int y = y1; y <= y2; ++y ) //y
    {
        for( int x = x1; x <= x2; ++x ) //x
        {
            tileIndexes.Add({x, y});
        }
    }

    return tileIndexes;
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
