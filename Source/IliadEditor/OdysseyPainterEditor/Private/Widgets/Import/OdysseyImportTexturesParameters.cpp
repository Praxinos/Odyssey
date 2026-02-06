// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyImportTexturesParameters.h"
#include "Engine/TextureRenderTarget2D.h"
#include "CanvasItem.h"
#include "CanvasRender.h"
#include "CanvasTypes.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "ScreenPass.h"
#include "Factories/TextureFactory.h"

#define LOCTEXT_NAMESPACE "PainterEditor"


FOdysseyImportTexturesParameters::FOdysseyImportTexturesParameters()
{

}

void
FOdysseyImportTexturesParameters::Init(const TArray< UTexture2D* >& iTextures, uint32 iDestinationWidth, uint32 iDestinationHeight)
{
    mSourceTextures = iTextures;
    mDestinationWidth = iDestinationWidth;
    mDestinationHeight = iDestinationHeight;
}

void
FOdysseyImportTexturesParameters::Init(const TArray<FString>& iFilenames, uint32 iDestinationWidth, uint32 iDestinationHeight)
{
    mDestinationWidth = iDestinationWidth;
    mDestinationHeight = iDestinationHeight;

    mSourceTextures.Empty();

    //Convert to textures
    FScopedSlowTask progressBar(iFilenames.Num(), LOCTEXT("texture-editor.import-images.progress-bar.title", "Importing Images"));
    progressBar.MakeDialog();

    TStrongObjectPtr<UTextureFactory> TextureFactory(NewObject<UTextureFactory>());
    TArray<TStrongObjectPtr<UTexture2D>> importedTextures;
    importedTextures.Reserve(iFilenames.Num());
    for (const FString& filename : iFilenames)
    {
        progressBar.EnterProgressFrame();

        UObject* importedObject = UFactory::StaticImportObject(UTexture2D::StaticClass(), GetTransientPackage(), NAME_None, EObjectFlags::RF_NoFlags, *filename, nullptr, TextureFactory.Get());
        UTexture2D* importedTexture = Cast<UTexture2D>(importedObject);
        if (!importedTexture)
            continue;

        importedTextures.Emplace(importedTexture);
    }

    TArray<UTexture2D*> textures;

    for (int i = 0; i < importedTextures.Num(); i++)
    {
        mSourceTextures.Add(importedTextures[i].Get());
    }
}

UTextureRenderTarget2D*
FOdysseyImportTexturesParameters::CreateRT() const
{
    UTextureRenderTarget2D* renderTarget = NewObject<UTextureRenderTarget2D>();
    renderTarget->InitAutoFormat(mDestinationWidth, mDestinationHeight);
    renderTarget->UpdateResourceImmediate();
    return renderTarget;
}

FVector2D
FOdysseyImportTexturesParameters::GetTextureScaledSize(int iSourceTextureIndex) const
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
FOdysseyImportTexturesParameters::GetTexturePosition(const FVector2D& iTextureSize) const
{
    FVector2D texturePosition;
    switch(mAlignment)
    {
        case EOdysseyImportTextureAlignment::TopLeft: texturePosition = FVector2D::ZeroVector; break;
        case EOdysseyImportTextureAlignment::Top: texturePosition = FVector2D((mDestinationWidth - iTextureSize.X) / 2.f, 0); break;
        case EOdysseyImportTextureAlignment::TopRight: texturePosition = FVector2D(mDestinationWidth - iTextureSize.X, 0); break;
        case EOdysseyImportTextureAlignment::Left: texturePosition = FVector2D(0, (mDestinationHeight - iTextureSize.Y) / 2.f); break;
        case EOdysseyImportTextureAlignment::Center: texturePosition = FVector2D((mDestinationWidth - iTextureSize.X) / 2.f, (mDestinationHeight - iTextureSize.Y) / 2.f); break;
        case EOdysseyImportTextureAlignment::Right: texturePosition = FVector2D(mDestinationWidth - iTextureSize.X, (mDestinationHeight - iTextureSize.Y) / 2.f); break;
        case EOdysseyImportTextureAlignment::BottomLeft: texturePosition = FVector2D(0, mDestinationHeight - iTextureSize.Y); break;
        case EOdysseyImportTextureAlignment::Bottom: texturePosition = FVector2D((mDestinationWidth - iTextureSize.X) / 2.f, mDestinationHeight - iTextureSize.Y); break;
        case EOdysseyImportTextureAlignment::BottomRight: texturePosition = FVector2D(mDestinationWidth - iTextureSize.X, mDestinationHeight - iTextureSize.Y); break;
    }

    return texturePosition;
}

FVector2D
FOdysseyImportTexturesParameters::GetTexturePosition(int iSourceTextureIndex) const
{
    FVector2D textureSize = GetTextureScaledSize(iSourceTextureIndex);
    return GetTexturePosition(textureSize);
}

void
FOdysseyImportTexturesParameters::Render(UTextureRenderTarget2D* oRenderTarget, int iSourceTextureIndex) const
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
FOdysseyImportTexturesParameters::GetDestinationWidth() const
{
    return mDestinationWidth;
}

uint32
FOdysseyImportTexturesParameters::GetDestinationHeight() const
{
    return mDestinationHeight;
}

TArray< UTexture2D* >
FOdysseyImportTexturesParameters::GetSourceTextures() const
{
    return mSourceTextures;
}

EOdysseyImportTextureAlignment
FOdysseyImportTexturesParameters::GetAlignment() const
{
    return mAlignment;
}

EOdysseyImportTextureScaling
FOdysseyImportTexturesParameters::GetScaling() const
{
    return mScaling;
}

EOdysseyAntiAliasing
FOdysseyImportTexturesParameters::GetResamplingMethod() const
{
    return mResamplingMethod;
}

bool
FOdysseyImportTexturesParameters::GetIsScanCleanerActivated() const
{
    return mIsScanCleanerActivated;
}

void
FOdysseyImportTexturesParameters::SetIsScanCleanerActivated(bool iIsActivated)
{
    mIsScanCleanerActivated = iIsActivated;
}

void
FOdysseyImportTexturesParameters::SetAlignment(EOdysseyImportTextureAlignment iAlignment)
{
    mAlignment = iAlignment;
}

void
FOdysseyImportTexturesParameters::SetScaling(EOdysseyImportTextureScaling iScaling)
{
    mScaling = iScaling;
}

void
FOdysseyImportTexturesParameters::SetResamplingMethod(EOdysseyAntiAliasing iMethod)
{
    mResamplingMethod = iMethod;
}


#undef LOCTEXT_NAMESPACE
