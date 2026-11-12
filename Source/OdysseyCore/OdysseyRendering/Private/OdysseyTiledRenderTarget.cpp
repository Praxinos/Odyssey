// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTiledRenderTarget.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "ScreenPass.h"

FOdysseyTiledRenderTarget::FOdysseyTiledRenderTarget()
    : mTileMap()
    , mTileSize(0)
    , mWidth(0)
    , mHeight(0)
{
}

FOdysseyTiledRenderTarget::FOdysseyTiledRenderTarget(int iTileSize, int iWidth, int iHeight, ETextureRenderTargetFormat iRenderTargetFormat)
    : mTileMap(ceil( (float)iWidth / iTileSize ), ceil( (float)iHeight / iTileSize ))
    , mTileSize(iTileSize)
    , mWidth(iWidth)
    , mHeight(iHeight)
    , mRenderTargetFormat(iRenderTargetFormat)
{
}

TArray<FIntPoint>
FOdysseyTiledRenderTarget::GetTileIndexesInRect(const FIntRect& Rect) const
{
    float x1f = float( Rect.Min.X ) / mTileSize;
    float y1f = float( Rect.Min.Y ) / mTileSize;
    float x2f = float( Rect.Max.X ) / mTileSize;
    float y2f = float( Rect.Max.Y ) / mTileSize;

    FIntPoint tileCount = mTileMap.Size();

    int x1 = FMath::Clamp(x1f, 0, tileCount.X - 1);
    int y1 = FMath::Clamp(y1f, 0, tileCount.Y - 1);
    int x2 = FMath::Clamp(x2f, 0, tileCount.X - 1);
    int y2 = FMath::Clamp(y2f, 0, tileCount.Y - 1);

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

void
FOdysseyTiledRenderTarget::Draw(UTexture* Source, FIntRect SourceRect, FIntPoint DestinationPosition)
{
    if (!Source)
        return;

    if (SourceRect.Min.X >= Source->GetSurfaceWidth() || SourceRect.Max.X < 0 || SourceRect.Min.Y >= Source->GetSurfaceHeight() || SourceRect.Max.Y < 0)
        return;

    float x1f = float( DestinationPosition.X ) / mTileSize;
    float y1f = float( DestinationPosition.Y ) / mTileSize;
    float x2f = float( DestinationPosition.X + SourceRect.Size().X ) / mTileSize;
    float y2f = float( DestinationPosition.Y + SourceRect.Size().Y ) / mTileSize;

    FIntPoint tileCount = mTileMap.Size();

    /* int x = FMath::Max(0, x1f);
    int y = FMath::Max(0, y1f);
    int w = FMath::Clamp( int( ceil( x2f ) ) - abs((int)x1f), 0, tileCount.X);
    int h = FMath::Clamp( int( ceil( y2f ) ) - abs((int)y1f), 0, tileCount.Y); */

    int x1 = FMath::Clamp(x1f, 0, tileCount.X - 1);
    int y1 = FMath::Clamp(y1f, 0, tileCount.Y - 1);
    int x2 = FMath::Clamp(x2f, 0, tileCount.X - 1);
    int y2 = FMath::Clamp(y2f, 0, tileCount.Y - 1);

    for( int y = y1; y <= y2; ++y ) //y
    {
        for( int x = x1; x <= x2; ++x ) //x
        {
            FTile& tile = mTileMap.Get(x, y);
            if (!tile.RenderTarget)
            {
                tile.RenderTarget = TStrongObjectPtr<UTextureRenderTarget2D>(NewObject<UTextureRenderTarget2D>());

                tile.RenderTarget->RenderTargetFormat = mRenderTargetFormat;
                tile.RenderTarget->ClearColor = FLinearColor::Transparent;
                tile.RenderTarget->UpdateResource();
                tile.RenderTarget->ResizeTarget(mTileSize, mTileSize);
                tile.RenderTarget->UpdateResourceImmediate();

                mTileMap.Set(x, y, tile);
            }

            FIntPoint tileIndex = FIntPoint(x, y);
            FIntRect tileRect = GetTileRect(tileIndex);

            ENQUEUE_RENDER_COMMAND(IOdysseyTextureRenderingAbility_RenderRectAtRect)(
                [renderTarget = tile.RenderTarget, tileRect, Source, SourceRect, DestinationPosition](FRHICommandListImmediate& RHICmdList)
                {
                    FRDGBuilder graphBuilder(RHICmdList);
                    FRDGTextureRef sourceTexture = graphBuilder.RegisterExternalTexture(CreateRenderTarget(Source->GetResource()->TextureRHI, TEXT("UOdysseyAnimation::sourceTexture")));
                    FRDGTextureRef destinationTexture = renderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

                    FIntRect tileSourceRect = SourceRect;
                    tileSourceRect.Clip(tileRect - DestinationPosition);

                    FIntRect tileDestinationRect = tileRect;
                    tileDestinationRect.Clip(SourceRect + DestinationPosition);
                    tileDestinationRect -= tileRect.Min;

                    AddDrawTexturePass(
                        graphBuilder,
                        FScreenPassViewInfo(),
                        sourceTexture,
                        destinationTexture,
                        tileSourceRect.Min,
                        tileSourceRect.Size(),
                        tileDestinationRect.Min,
                        tileDestinationRect.Size()
                    );
                    graphBuilder.Execute();
                }
            );

            tile.IsDirty = true;
        }
    }
}

void
FOdysseyTiledRenderTarget::Draw(UTexture* Source, TArray<FIntPoint> TileIndexes)
{
    if (!Source)
        return;

    for (FIntPoint tileIndex : TileIndexes)
    {
        FTile& tile = mTileMap.Get(tileIndex.X, tileIndex.Y);
        if (!tile.RenderTarget)
        {
            tile.RenderTarget = TStrongObjectPtr<UTextureRenderTarget2D>(NewObject<UTextureRenderTarget2D>());

            tile.RenderTarget->RenderTargetFormat = mRenderTargetFormat;
            tile.RenderTarget->UpdateResource();
            tile.RenderTarget->ResizeTarget(mTileSize, mTileSize);
            tile.RenderTarget->UpdateResourceImmediate();

            mTileMap.Set(tileIndex.X, tileIndex.Y, tile);
        }
        FIntRect tileRect = GetTileRect(tileIndex);

        ENQUEUE_RENDER_COMMAND(IOdysseyTextureRenderingAbility_RenderRectAtRect)(
            [renderTarget = tile.RenderTarget, tileRect, Source](FRHICommandListImmediate& RHICmdList)
            {
                FRDGBuilder graphBuilder(RHICmdList);
                FRDGTextureRef sourceTexture = graphBuilder.RegisterExternalTexture(CreateRenderTarget(Source->GetResource()->TextureRHI, TEXT("UOdysseyAnimation::sourceTexture")));
                FRDGTextureRef destinationTexture = renderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

                FIntRect tileSourceRect = tileRect;
                FIntRect tileDestinationRect = tileRect - tileRect.Min;

                AddDrawTexturePass(
                    graphBuilder,
                    FScreenPassViewInfo(),
                    sourceTexture,
                    destinationTexture,
                    tileSourceRect.Min,
                    tileSourceRect.Size(),
                    tileDestinationRect.Min,
                    tileDestinationRect.Size()
                );
                graphBuilder.Execute();
            }
        );

        tile.IsDirty = true;
    }
}

/* void
FOdysseyTiledRenderTarget::Draw(FOdysseyTiledRenderTarget Source, FIntRect SourceRect, FIntPoint DestinationPosition)
{
    FIntPoint tileCount = mTileMap.Size();

    if (SourceRect.Min.X >= mWidth || SourceRect.Max.X < 0 || SourceRect.Min.Y >= mHeight || SourceRect.Max.Y < 0)
        return;

    TStrongObjectPtr<UTextureRenderTarget2D> renderTarget = mTileMap.Get(iTileIndex.X, iTileIndex.Y);
    if (!renderTarget)
    {
        UTextureRenderTarget2D* newRenderTarget = NewObject<UTextureRenderTarget2D>();
        newRenderTarget->RenderTargetFormat = RenderTargetFormat;
        newRenderTarget->UpdateResource();
        newRenderTarget->ResizeTarget(mTileSize, mTileSize);

        renderTarget = TStrongObjectPtr<UTextureRenderTarget2D>(newRenderTarget);

        mTileMap.Set(iTileIndex.X, iTileIndex.Y, renderTarget);
    }

    //TODO: Draw
} */

void
FOdysseyTiledRenderTarget::Clean()
{
    FIntPoint tileCount = mTileMap.Size();

    for( int y = 0; y < tileCount.Y; ++y )
    {
        for( int x = 0; x < tileCount.X; ++x )
        {
            FTile& tile = mTileMap.Get(x, y);
            tile.IsDirty = false;
        }
    }
}

void
FOdysseyTiledRenderTarget::Clear()
{
    FIntPoint tileCount = mTileMap.Size();

    for( int y = 0; y < tileCount.Y; ++y )
    {
        for( int x = 0; x < tileCount.X; ++x )
        {
            mTileMap.Set(x, y, FTile());
        }
    }
}

FIntRect
FOdysseyTiledRenderTarget::GetTileRect(const FIntPoint& iTileIndex) const
{
    return FIntRect(
        iTileIndex.X * mTileSize,
        iTileIndex.Y * mTileSize,
        FMath::Min( (iTileIndex.X + 1) * mTileSize, mWidth),
        FMath::Min( (iTileIndex.Y + 1) * mTileSize, mHeight)
    );
}

UTexture*
FOdysseyTiledRenderTarget::GetTileTexture(const FIntPoint& iTileIndex) const
{
    return mTileMap.Get(iTileIndex.X, iTileIndex.Y).RenderTarget.Get();
}

bool
FOdysseyTiledRenderTarget::IsTileDirty(const FIntPoint& iTileIndex) const
{
    return mTileMap.Get(iTileIndex.X, iTileIndex.Y).IsDirty;
}

TArray<FIntPoint>
FOdysseyTiledRenderTarget::GetDirtyTileIndexes() const
{
    TArray<FIntPoint> invalidTiles;
    FIntPoint tileCount = mTileMap.Size();

    for( int y = 0; y < tileCount.Y; ++y )
    {
        for( int x = 0; x < tileCount.X; ++x )
        {
            if (IsTileDirty(FIntPoint(x, y)))
            {
                invalidTiles.Add(FIntPoint(x, y));
            }
        }
    }

    return invalidTiles;
}

TArray<FIntRect>
FOdysseyTiledRenderTarget::GetDirtyRects() const
{
    TArray<FIntRect> invalidRects;
    FIntPoint tileCount = mTileMap.Size();

    TArray<int> prevRectIndexes;
    for( int y = 0; y < tileCount.Y; ++y )
    {
        TArray<FIntRect> rects;
        bool wasInvalid = false;
        for( int x = 0; x < tileCount.X; ++x )
        {
            bool isInvalid = IsTileDirty(FIntPoint(x, y));
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

void
FOdysseyTiledRenderTarget::GetDirtyTiles(TArray<FIntPoint>& TileIndexes, TArray<FIntRect>& Rects, TArray<UTexture*>& Textures ) const
{
    FIntPoint tileCount = mTileMap.Size();
    for (int y = 0; y < tileCount.Y; y++)
    {
        for (int x = 0; x < tileCount.X; x++)
        {
            bool isInvalid = IsTileDirty(FIntPoint(x, y));
            if (!isInvalid)
                continue;

            TileIndexes.Add({x, y});
            Rects.Add(GetTileRect({x, y}));
            Textures.Add(GetTileTexture({x, y}));
        }
    }
}

int
FOdysseyTiledRenderTarget::GetWidth() const
{
    return mWidth;
}

int
FOdysseyTiledRenderTarget::GetHeight() const
{
    return mHeight;
}

int
FOdysseyTiledRenderTarget::GetTileSize() const
{
    return mTileSize;
}
