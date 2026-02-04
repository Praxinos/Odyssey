// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyImportTexturesData.h"
#include "Engine/TextureRenderTarget2D.h"
#include "CanvasItem.h"
#include "CanvasRender.h"
#include "CanvasTypes.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "ScreenPass.h"

#define LOCTEXT_NAMESPACE "PainterEditor"


FOdysseyImportTexturesData::FOdysseyImportTexturesData()
{

}

FOdysseyImportTexturesData::FOdysseyImportTexturesData(const TArray< UTexture2D* > iSourceTextures, uint32 iDestinationWidth, uint32 iDestinationHeight)
    : mSourceTextures(iSourceTextures)
    , mDestinationWidth(iDestinationWidth)
    , mDestinationHeight(iDestinationHeight)
{

}

UTextureRenderTarget2D*
FOdysseyImportTexturesData::CreateRT() const
{
    UTextureRenderTarget2D* renderTarget = NewObject<UTextureRenderTarget2D>();
    renderTarget->InitAutoFormat(mDestinationWidth, mDestinationHeight);
    renderTarget->UpdateResourceImmediate();
    return renderTarget;
}

FVector2D
FOdysseyImportTexturesData::GetTextureScaledSize(int iSourceTextureIndex) const
{
    UTexture2D* sourceTexture = mSourceTextures[iSourceTextureIndex];
    sourceTexture->BlockOnAnyAsyncBuild();
    sourceTexture->SetForceMipLevelsToBeResident( 30.0f );
    sourceTexture->WaitForStreaming();

    FVector2D size(sourceTexture->GetSurfaceWidth(), sourceTexture->GetSurfaceHeight());

    switch(mScaling)
    {
        case EOdysseyImportTextureScaling::None: break;

        case EOdysseyImportTextureScaling::Scale:
        {
            size = FVector2D(mDestinationWidth, mDestinationHeight);
        }
        break;

        case EOdysseyImportTextureScaling::ScaleAndFit:
        {
            float ratio = FMath::Min(float(mDestinationWidth) / sourceTexture->GetSurfaceWidth(), float(mDestinationHeight) / sourceTexture->GetSurfaceHeight());
            size = FVector2D(sourceTexture->GetSurfaceWidth() * ratio, sourceTexture->GetSurfaceHeight() * ratio);
        }
        break;
    }

    return size;
}

FVector2D
FOdysseyImportTexturesData::GetTexturePosition(const FVector2D& iTextureSize) const
{
    FVector2D texturePosition;
    switch(mAlignment)
    {
        case FOdysseyImportTexturesData::EAlignment::TopLeft: texturePosition = FVector2D::ZeroVector; break;
        case FOdysseyImportTexturesData::EAlignment::Top: texturePosition = FVector2D((mDestinationWidth - iTextureSize.X) / 2.f, 0); break;
        case FOdysseyImportTexturesData::EAlignment::TopRight: texturePosition = FVector2D(mDestinationWidth - iTextureSize.X, 0); break;
        case FOdysseyImportTexturesData::EAlignment::Left: texturePosition = FVector2D(0, (mDestinationHeight - iTextureSize.Y) / 2.f); break;
        case FOdysseyImportTexturesData::EAlignment::Center: texturePosition = FVector2D((mDestinationWidth - iTextureSize.X) / 2.f, (mDestinationHeight - iTextureSize.Y) / 2.f); break;
        case FOdysseyImportTexturesData::EAlignment::Right: texturePosition = FVector2D(mDestinationWidth - iTextureSize.X, (mDestinationHeight - iTextureSize.Y) / 2.f); break;
        case FOdysseyImportTexturesData::EAlignment::BottomLeft: texturePosition = FVector2D(0, mDestinationHeight - iTextureSize.Y); break;
        case FOdysseyImportTexturesData::EAlignment::Bottom: texturePosition = FVector2D((mDestinationWidth - iTextureSize.X) / 2.f, mDestinationHeight - iTextureSize.Y); break;
        case FOdysseyImportTexturesData::EAlignment::BottomRight: texturePosition = FVector2D(mDestinationWidth - iTextureSize.X, mDestinationHeight - iTextureSize.Y); break;
    }

    return texturePosition;
}

FVector2D
FOdysseyImportTexturesData::GetTexturePosition(int iSourceTextureIndex) const
{
    FVector2D textureSize = GetTextureScaledSize(iSourceTextureIndex);
    return GetTexturePosition(textureSize);
}

void
FOdysseyImportTexturesData::Render(UTextureRenderTarget2D* oRenderTarget, int iSourceTextureIndex) const
{
    UTexture2D* sourceTexture = mSourceTextures[iSourceTextureIndex];
    sourceTexture->BlockOnAnyAsyncBuild();
    sourceTexture->SetForceMipLevelsToBeResident( 30.0f );
    sourceTexture->WaitForStreaming();

    FTextureRenderTargetResource* renderTargetResource = oRenderTarget->GameThread_GetRenderTargetResource();

    //Clear RenderTarget
    ENQUEUE_RENDER_COMMAND(SOdysseyImportTexturesDialog_Render)(
        [renderTargetResource](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder(RHICmdList);
            FRDGTextureRef destinationTexture = renderTargetResource->GetRenderTargetTexture( graphBuilder );
            AddClearRenderTargetPass(graphBuilder, destinationTexture, FLinearColor::Transparent);
            graphBuilder.Execute();
        }
    );

    FCanvas canvas(renderTargetResource, nullptr, FGameTime(), GMaxRHIFeatureLevel);
    FCanvasRenderThreadScope canvasRenderThreadScope(canvas);

    FVector2D scaledSize = GetTextureScaledSize(iSourceTextureIndex);
    FVector2D texturePosition = GetTexturePosition(scaledSize);

    FTexture* tileTexture = new FTexture();
    canvasRenderThreadScope.DeferredDelete(tileTexture); //ensures deletion of tileTexture when canvas has finished to draw
    tileTexture->TextureRHI = sourceTexture->GetResource()->TextureRHI;
    tileTexture->SamplerStateRHI = Odyssey::GetSamplerStateForAntiAliasing(mResamplingMethod);

    FCanvasTileItem TileItem(
        texturePosition,
        tileTexture,
        scaledSize,
        FColor::White
    );

    canvas.DrawItem(TileItem);
    canvas.Flush_GameThread();
}

uint32
FOdysseyImportTexturesData::GetDestinationWidth() const
{
    return mDestinationWidth;
}

uint32
FOdysseyImportTexturesData::GetDestinationHeight() const
{
    return mDestinationHeight;
}

TArray< UTexture2D* >
FOdysseyImportTexturesData::GetSourceTextures() const
{
    return mSourceTextures;
}

FOdysseyImportTexturesData::EAlignment
FOdysseyImportTexturesData::GetAlignment() const
{
    return mAlignment;
}

EOdysseyImportTextureScaling
FOdysseyImportTexturesData::GetScaling() const
{
    return mScaling;
}

EOdysseyAntiAliasing
FOdysseyImportTexturesData::GetResamplingMethod() const
{
    return mResamplingMethod;
}

void
FOdysseyImportTexturesData::SetAlignment(EAlignment iAlignment)
{
    mAlignment = iAlignment;
}

void
FOdysseyImportTexturesData::SetScaling(EOdysseyImportTextureScaling iScaling)
{
    mScaling = iScaling;
}

void
FOdysseyImportTexturesData::SetResamplingMethod(EOdysseyAntiAliasing iMethod)
{
    mResamplingMethod = iMethod;
}


#undef LOCTEXT_NAMESPACE
