// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPaintableTexture2D.h"
#include "UObject/ObjectSaveContext.h"
#include "RenderGraphUtils.h"
#include "GenerateMips.h"
#include "OdysseyBlendShader.h"
#include "ImageCoreUtils.h"
#include "ScreenPass.h"

#define TileSize 64

FIntRect
GetTileRect(int tileX, int tileY)
{
    int rectX = tileX * TileSize;
    int rectY = tileY * TileSize;
    int rectW = FMath::Min(TileSize, Width - tileX * TileSize);
    int rectH = FMath::Min(TileSize, Height - tileY * TileSize);

    return FIntRect(rectX, rectY, rectx + rectW, recty + rectH);
}

void
UOdysseyPaintableTexture2D::Initialize(int InWidth, int InHeight, ETextureRenderTargetFormat InFormat)
{
    Width = (uint32)InWidth;
    Height = (uint32)InHeight;
    Format = InFormat;

    mUndoTileMap = FOdysseyInvalidTileMap(64, Width, Height);

    int numTilesX = (Width - 1) / TileSize,
    int numTilesY = (Height - 1) / TileSize;

    mTiles.AddDefaulted(numTilesY);
    for (int y = 0; y < numTilesY; y++)
    {
        mTiles[y].AddDefaulted(numTilesX);
        for (int y = 0; y < numTilesY; y++)
        {
            mTiles[y][x].mRect = GetTileRect(x, y);
        }
    }
}

int
UOdysseyPaintableTexture2D::GetWidth() const
{
    return Width;
}

int
UOdysseyPaintableTexture2D::GetHeight() const
{
    return Height;
}

ETextureRenderTargetFormat
UOdysseyPaintableTexture2D::GetFormat() const
{
    return Format;
}

void
UOdysseyPaintableTexture2D::BeginDraw()
{
    checkf(!IsDrawing, TEXT("BeginDraw() cannot be called twice in a row, Call EndDraw() first"));

    if (IsDrawing)
        return;

    mRenderTarget = TStrongObjectPtr(NewObject<UTextureRenderTarget2D>());
    mRenderTarget->RenderTargetFormat = Format;
    mRenderTarget->bForceLinearGamma = Format != RTF_RGBA8_SRGB;
    mRenderTarget->ClearColor = FLinearColor::Transparent;
    mRenderTarget->InitAutoFormat(Width, Height);
    mRenderTarget->UpdateResource();

    mUndoRenderTarget = TStrongObjectPtr(NewObject<UTextureRenderTarget2D>());
    mUndoRenderTarget->RenderTargetFormat = Format;
    mUndoRenderTarget->bForceLinearGamma = Format != RTF_RGBA8_SRGB;
    mUndoRenderTarget->ClearColor = FLinearColor::Transparent;
    mUndoRenderTarget->InitAutoFormat(Width, Height);
    mUndoRenderTarget->UpdateResource();

    //Initialize RenderTargets contents
    RenderTiles(mUndoRenderTarget.Get(), FIntRect(0, 0, Width, Height));
    RenderTiles(mRenderTarget.Get(), FIntRect(0, 0, Width, Height));

    IsDrawing = true;
}

void
UOdysseyPaintableTexture2D::Draw(UTexture* SourceTexture, FIntRect Rect, FIntPoint Position)
{
    checkf(IsDrawing, TEXT("Draw cannot be called if BeginDraw() has not been called first"));

    if (!IsDrawing)
        return;

    FIntRect sourceRect = Rect;
    FIntRect fullRect(0, 0, Width, Height);
    FIntRect destinationRect(0, 0, Width, Height);

    sourceRect.Clip(fullRect - Position);
    destinationRect.Clip(Rect + Position);

    mUndoTileMap.Invalidate(destinationRect);

    ENQUEUE_RENDER_COMMAND(UOdysseyPaintableTexture2D_Draw)(
        [source = SourceTexture, destination = mRenderTarget, sourceRect, destinationRect](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder(RHICmdList);
            FRDGTextureRef sourceTexture = graphBuilder.RegisterExternalTexture(CreateRenderTarget(source->GetResource()->TextureRHI, TEXT("Source")));
            FRDGTextureRef destinationTexture = destination->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

            AddDrawTexturePass(
                graphBuilder,
                FScreenPassViewInfo(),
                sourceTexture,
                destinationTexture,
                sourceRect.Min,
                sourceRect.Size(),
                destinationRect.Min,
                destinationRect.Size()
            );
            graphBuilder.Execute();
        }
    );
}

void
UOdysseyPaintableTexture2D::ResetDraw()
{
    checkf(IsDrawing, TEXT("CommitDraw cannot be called if BeginDraw() has not been called first"));

    if (!IsDrawing)
        return;

    CopyUndoRTToRenderTarget();
    mUndoTileMap.Clear();
}

void
UOdysseyPaintableTexture2D::CommitDraw(bool IsUndoable)
{
    checkf(IsDrawing, TEXT("CommitDraw cannot be called if BeginDraw() has not been called first"));

    if (!IsDrawing)
        return;

    if (IsUndoable)
    {
        //TODO: Register Undo
    }
    mUndoTileMap.Clear();

    CopyRenderTargetToUndoRT();
}

void
UOdysseyPaintableTexture2D::EndDraw(bool IsUndoable)
{
    checkf(IsDrawing, TEXT("EndDraw cannot be called if BeginDraw() has not been called first"));

    if (!IsDrawing)
        return;

    if (IsUndoable)
    {
        //TODO: Register Undo
    }

    mUndoTileMap.Clear();
    IsDrawing = false;
    mRenderTarget = nullptr;
    mUndoRenderTarget = nullptr;
}

bool
UOdysseyPaintableTexture2D::GetIsDrawing() const
{
    return IsDrawing;
}

void
UOdysseyPaintableTexture2D::PreSave(FObjectPreSaveContext SaveContext)
{
    SyncRenderTargetAndImage();
    /* if (IsDrawing)
    {
        CommitRenderTargetToImage();
    } */
}

void
UOdysseyPaintableTexture2D::CommitRenderTargetToImage()
{
    FImage OutImage;
    if (!FImageUtils::GetRenderTargetImage(mRenderTarget.Get(), OutImage))
        return;

    for (int y = 0; y < mTiles.Num(); y++)
    {
        for (int x = 0; x < mTiles[y].Num(); x++)
        {
            FTile& tile = mTiles[y][x];
            ExtractTile(OutImage, tile, x, y);
        }
    }
}

void
UOdysseyPaintableTexture2D::Render(UTextureRenderTarget2D* Destination, FIntRect Rect, FIntPoint Position) const
{
    LoadRenderTarget();

    FIntRect sourceRect = Rect;
    FIntRect fullRect(0, 0, Destination->SizeX, Destination->SizeY);
    FIntRect destinationRect(0, 0, Destination->SizeX, Destination->SizeY);

    sourceRect.Clip(fullRect - Position);
    destinationRect.Clip(Rect + Position);

    ENQUEUE_RENDER_COMMAND(UOdysseyPaintableTexture2D_Draw)(
        [source = mRenderTarget, destination = Destination, sourceRect, destinationRect](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder(RHICmdList);
            FRDGTextureRef sourceTexture = source->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );
            FRDGTextureRef destinationTexture = destination->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

            AddDrawTexturePass(
                graphBuilder,
                FScreenPassViewInfo(),
                sourceTexture,
                destinationTexture,
                sourceRect.Min,
                sourceRect.Size(),
                destinationRect.Min,
                destinationRect.Size()
            );
            graphBuilder.Execute();
        }
    );
}

void
UOdysseyPaintableTexture2D::RenderTiles(UTextureRenderTarget2D* Destination, FIntRect Rect, FIntPoint Position) const
{
    FIntRect sourceRect = Rect;
    FIntRect fullRect(0, 0, Destination->SizeX, Destination->SizeY);
    sourceRect.Clip(fullRect - Position);

    if (sourceRect.Min.X >= mWidth || sourceRect.Max.X < 0 || sourceRect.Min.Y >= mHeight || sourceRect.Max.Y < 0)
        return {};

    int tileCountX = (iWidth - 1) / mTileSize;
    int tileCountY = (iHeight - 1) / mTileSize;

    int x1 = FMath::Clamp(sourceRect.Min.X / TileSize, 0, tileCountX - 1);
    int y1 = FMath::Clamp(sourceRect.Min.Y / TileSize, 0, tileCountY - 1);
    int x2 = FMath::Clamp(sourceRect.Max.X / TileSize, 0, tileCountX - 1);
    int y2 = FMath::Clamp(sourceRect.Max.Y / TileSize, 0, tileCountY - 1);

    int w = x2 - x1 + 1;
    int h = y2 - y1 + 1;

    if (w <= 0 || h <= 0)
        return {};

    struct FRenderTile
    {
        FIntRect mRect;
        TSharedPtr<FImage> mImage;
    }
    TArray<TArray<FRenderTile>> tilesToRender;


    ENQUEUE_RENDER_COMMAND(UOdysseyPaintableTexture2D_RenderTiles)(
        [tiles = mTiles, x1, y1, x2, y2, sourceRect = Rect, position = Position, destination = Destination](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder(RHICmdList);
            FRDGTextureRef destinationTexture = destination->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

            for(int y = y1; y <= y2; y++)
            {
                for(int x = x1; x <= x2; x++)
                {
                    FTile& tile = tiles[y][x];
                    if (tile.mIsEmpty)
                    {
                        //TODO: Clear destination for this tile
                        continue;
                    }

                    FRDGTextureRef sourceTexture = source->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

                    FIntRect tileRect = tile->mRect;
                    FIntRect fullRect(0, 0, destination->SizeX, destination->SizeY);
                    FIntRect destinationRect(0, 0, destination->SizeX, destination->SizeY);

                    sourceRect.Clip(fullRect - Position);
                    destinationRect.Clip(Rect + Position);

                    AddDrawTexturePass(
                        graphBuilder,
                        FScreenPassViewInfo(),
                        sourceTexture,
                        destinationTexture,
                        sourceRect.Min,
                        sourceRect.Size(),
                        destinationRect.Min,
                        destinationRect.Size()
                    );
                }
            }

            graphBuilder.Execute();
        }
    );
}

void
UOdysseyPaintableTexture2D::CopyRenderTargetToUndoRT()
{
    FIntRect sourceRect(0, 0, Width, Height);
    FIntRect destinationRect(0, 0, Width, Height);

    ENQUEUE_RENDER_COMMAND(UOdysseyPaintableTexture2D_Draw)(
        [source = mRenderTarget, destination = mUndoRenderTarget, sourceRect, destinationRect](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder(RHICmdList);
            FRDGTextureRef sourceTexture = source->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );
            FRDGTextureRef destinationTexture = destination->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

            AddDrawTexturePass(
                graphBuilder,
                FScreenPassViewInfo(),
                sourceTexture,
                destinationTexture,
                sourceRect.Min,
                sourceRect.Size(),
                destinationRect.Min,
                destinationRect.Size()
            );
            graphBuilder.Execute();
        }
    );
}

void
UOdysseyPaintableTexture2D::CopyUndoRTToRenderTarget()
{
    FIntRect sourceRect(0, 0, Width, Height);
    FIntRect destinationRect(0, 0, Width, Height);

    ENQUEUE_RENDER_COMMAND(UOdysseyPaintableTexture2D_Draw)(
        [source = mUndoRenderTarget, destination = mRenderTarget, sourceRect, destinationRect](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder(RHICmdList);
            FRDGTextureRef sourceTexture = source->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );
            FRDGTextureRef destinationTexture = destination->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

            AddDrawTexturePass(
                graphBuilder,
                FScreenPassViewInfo(),
                sourceTexture,
                destinationTexture,
                sourceRect.Min,
                sourceRect.Size(),
                destinationRect.Min,
                destinationRect.Size()
            );
            graphBuilder.Execute();
        }
    );
}

void
UOdysseyPaintableTexture2D::ExtractTile(const FImage& InImage, FTile& OutTile) const
{
    //Clear the tile data
    OutTile->mData = nullptr;
    FIntRect tileRect = OutTile->mRect;

    bool isEmpty = true;
    for (int y = 0; isEmpty && y < tileRect.Height(); y++)
    {
        for (int x = 0; isEmpty && x < tileRect.Width(); x++)
        {
            if (InImage.GetOnePixelLinear(tileRect.Min.X + x, tileRect.Min.Y + y) != FLinearColor::Transparent)
                isEmpty = false;
        }
    }

    if (isEmpty)
        return;

    OutTile->mData = MakeShared<FTileData>();
    OutTile->mData->mImage.Init(tileRect.Width(), tileRect.Height(), InImage.Format, InImage.GammaSpace);

    for (int y = 0; y < tileRect.Height(); y++)
    {
        void* srcLine = InImage.GetPixelPointer(tileRect.Min.X, tileRect.Min.Y + y);
        void* dstLine = OutTile->mData->mImage.GetPixelPointer(0, y);

        FMemory::Memcpy(dstLine, srcLine, tileRect.Width() * InImage.GetBytesPerPixel());
    }
}
