// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPaintableTexture2D.h"
#include "UObject/ObjectSaveContext.h"
#include "RenderGraphUtils.h"
#include "GenerateMips.h"
#include "OdysseyBlendShader.h"
#include "ImageCoreUtils.h"
#include "ScreenPass.h"

void
UOdysseyPaintableTexture2D::Initialize(int InWidth, int InHeight, ETextureRenderTargetFormat InFormat)
{
    Width = (uint32)InWidth;
    Height = (uint32)InHeight;
    Format = InFormat;

    mEditedTiles = FOdysseyInvalidTileMap(64, Width, Height);
    mResetTiles = FOdysseyInvalidTileMap(64, Width, Height);
    mUndoTiles = FOdysseyInvalidTileMap(64, Width, Height);
    mImage = FOdysseyTiledImage(64, Width, Height, Format);
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
    mRenderTarget->bForceLinearGamma = false;
    mRenderTarget->ClearColor = FLinearColor::Transparent;
    mRenderTarget->InitAutoFormat(Width, Height);
    mRenderTarget->UpdateResource();

    mUndoRenderTarget = TStrongObjectPtr(NewObject<UTextureRenderTarget2D>());
    mUndoRenderTarget->RenderTargetFormat = Format;
    mUndoRenderTarget->bForceLinearGamma = false;
    mUndoRenderTarget->ClearColor = FLinearColor::Transparent;
    mUndoRenderTarget->InitAutoFormat(Width, Height);
    mUndoRenderTarget->UpdateResource();

    //Initialize RenderTargets contents
    mImage.Render(mUndoRenderTarget.Get(), FIntRect(0, 0, Width, Height) );
    mImage.Render(mRenderTarget.Get(), FIntRect(0, 0, Width, Height) );

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

    mEditedTiles.Invalidate(destinationRect);
    mUndoTiles.Invalidate(destinationRect);

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
    mUndoTiles.Clear();

    mEditedTiles = mResetTiles;
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
    mUndoTiles.Clear();
    mResetTiles = mEditedTiles;

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

    CommitRenderTargetToImage();

    mUndoTiles.Clear();
    mResetTiles.Clear();
    mEditedTiles.Clear();
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
    if (IsDrawing)
    {
        CommitRenderTargetToImage();
    }
}

void
UOdysseyPaintableTexture2D::CommitRenderTargetToImage()
{
    mImage.Draw(mRenderTarget.Get(), mEditedTiles.InvalidRects());
}

void
UOdysseyPaintableTexture2D::Render(UTextureRenderTarget2D* Destination, FIntRect Rect, FIntPoint Position) const
{
    if (mRenderTarget)
    {

        FIntRect sourceRect = Rect;
        FIntRect fullRect(0, 0, mRenderTarget->SizeX, mRenderTarget->SizeY);
        FIntRect destinationRect(0, 0, mRenderTarget->SizeX, mRenderTarget->SizeY);

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
        return;
    }

    mImage.Render(Destination, Rect, Position);
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

/* TArray<FRHITexture*>
GetTilesRHITextures(const TArray<UTexture*> Textures )
{
    TArray<FRHITexture*> rhiTextures;
    for (const UTexture* texture : Textures)
    {
        rhiTextures.Add(texture->GetResource()->TextureRHI);
    }

    return rhiTextures;
}

void RenderTilesToTexture_RenderThread(FRDGBuilder& GraphBuilder, TArray<FRHITexture*> Tiles, TArray<FIntRect> Rects, FRDGTextureRef DestinationTexture)
{
    for (int i = 0; i < Tiles.Num(); i++ )
    {
        FIntRect rect = Rects[i];
        FRHITexture* rhiTexture = Tiles[i];
        FRDGTextureRef tileTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(rhiTexture, TEXT("UOdysseyPaintableTexture2D::TileTexture")));
        AddCopyTexturePass(
            GraphBuilder,
            tileTexture,
            DestinationTexture,
            FIntPoint(0, 0),
            rect.Min,
            rect.Size()
        );
    }
} */
