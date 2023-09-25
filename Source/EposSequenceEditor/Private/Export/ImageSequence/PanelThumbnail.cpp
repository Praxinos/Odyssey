// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/ImageSequence/PanelThumbnail.h"

#include "Slate/SlateTextures.h"

#include "Board/BoardSequence.h"

#define LOCTEXT_NAMESPACE "ImageSequenceExportPanelThumbnail"

//---

FPanelThumbnail::FPanelThumbnail()
    : mDesiredSize()
{
}


FPanelThumbnail::~FPanelThumbnail()
{
    DestroyTexture();
}

void
FPanelThumbnail::DestroyTexture()
{
    if( !mThumbnailRenderTarget && !mThumbnailTexture )
        return;

    // UE-114425: Defer the destroy until the next tick to work around the RHI getting destroyed before the render command completes.
    FSlateTexture2DRHIRef*               InThumbnailTexture      = mThumbnailTexture;
    FSlateTextureRenderTarget2DResource* InThumbnailRenderTarget = mThumbnailRenderTarget;

    mThumbnailTexture      = nullptr;
    mThumbnailRenderTarget = nullptr;

    GEditor->GetTimerManager()->SetTimerForNextTick([this, InThumbnailRenderTarget, InThumbnailTexture]()
    {
        ENQUEUE_RENDER_COMMAND(DestroyTexture)(
            [InThumbnailRenderTarget, InThumbnailTexture](FRHICommandList& RHICmdList)
            {
                if (InThumbnailTexture)
                {
                    InThumbnailTexture->ReleaseResource();
                    delete InThumbnailTexture;
                }

                if (InThumbnailRenderTarget)
                {
                    InThumbnailRenderTarget->ReleaseResource();
                    delete InThumbnailRenderTarget;
                }
            }
        );
    });
}

void
FPanelThumbnail::ResizeRenderTarget(const FIntPoint& iSize)
{
    // Delay texture creation until we actually draw the thumbnail
    if( iSize.X <= 0 || iSize.Y <= 0 )
        return;

    if( mThumbnailTexture && mThumbnailRenderTarget && mThumbnailTexture->GetWidth() == iSize.X && mThumbnailTexture->GetHeight() == iSize.Y )
        return;

    FSlateTexture2DRHIRef*               InThumbnailTexture      = mThumbnailTexture;
    FSlateTextureRenderTarget2DResource* InThumbnailRenderTarget = mThumbnailRenderTarget;

    // Note this used to call DestroyTexture() but now that is a latent destroy, this can no longer call DestroyTexture() since the reallocation would happen first and then the latent destroy
    ENQUEUE_RENDER_COMMAND(DestroyTexture)(
        [InThumbnailRenderTarget, InThumbnailTexture](FRHICommandList& RHICmdList)
        {
            if (InThumbnailTexture)
            {
                InThumbnailTexture->ReleaseResource();
                delete InThumbnailTexture;
            }

            if (InThumbnailRenderTarget)
            {
                InThumbnailRenderTarget->ReleaseResource();
                delete InThumbnailRenderTarget;
            }
        }
    );

    mThumbnailTexture      = new FSlateTexture2DRHIRef( iSize.X, iSize.Y, PF_B8G8R8A8, NULL, TexCreate_Dynamic );
    mThumbnailRenderTarget = new FSlateTextureRenderTarget2DResource( FLinearColor::Black, iSize.X, iSize.Y, PF_B8G8R8A8, SF_Point, TA_Wrap, TA_Wrap, 0.0f );

    InThumbnailTexture      = mThumbnailTexture;
    InThumbnailRenderTarget = mThumbnailRenderTarget;

    ENQUEUE_RENDER_COMMAND(AssignRenderTarget)(
        [InThumbnailRenderTarget, InThumbnailTexture](FRHICommandList& RHICmdList)
        {
            if (InThumbnailTexture && InThumbnailRenderTarget)
            {
                InThumbnailTexture->InitResource( RHICmdList );
                InThumbnailRenderTarget->InitResource( RHICmdList );
                InThumbnailTexture->SetRHIRef(InThumbnailRenderTarget->GetTextureRHI(), InThumbnailRenderTarget->GetSizeX(), InThumbnailRenderTarget->GetSizeY());
            }
        }
    );
}

FSlateTextureRenderTarget2DResource*
FPanelThumbnail::GetRenderTarget() const
{
    return mThumbnailRenderTarget;
}

FIntPoint
FPanelThumbnail::GetDesiredSize() const
{
    return mDesiredSize;
}

//---

FIntPoint
FPanelThumbnail::GetSize() const //override
{
    if( mThumbnailTexture )
    {
        return FIntPoint( mThumbnailTexture->GetWidth(), mThumbnailTexture->GetHeight() );
    }
    return FIntPoint(0,0);
}

FSlateShaderResource*
FPanelThumbnail::GetViewportRenderTargetTexture() const //override
{
    return mThumbnailTexture;
}

bool
FPanelThumbnail::RequiresVsync() const //override
{
    return false;
}

//---

#undef LOCTEXT_NAMESPACE
