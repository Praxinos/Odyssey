// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureLayer.h"

#include "OdysseyTextureLayerStack.h"
#include "ScreenPass.h"
#include "OdysseyBlendShader.h"

UTexture2D*
UOdysseyTextureLayer::GetTexture() const
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
    if(!layerStack)
        return nullptr;

    return layerStack->GetTexture();
}

UTexture2D*
UOdysseyTextureLayer::GetRenderTexture() const
{
    return Texture;
}

bool
UOdysseyTextureLayer::BuildRenderPipeline(
    FFrameNumber iFrame,
    uint64 iType,
    FOdysseyTextureRenderFunction& oRenderFunction
) const
{
    if (bCanHaveChildren)
        return Super::BuildRenderPipeline(iFrame, iType, oRenderFunction);

    oRenderFunction = [this, iType](
        FRDGBuilder& iGraphBuilder,
        ERHIFeatureLevel::Type iFeatureLevel,
        FRDGTextureRef iDestinationTexture,
        const FIntRect& iSrcRect,
        const FIntRect& iDstRect,
        const FMatrix& iSrcTransform
        )
    {
        AddClearRenderTargetPass(iGraphBuilder, iDestinationTexture, FLinearColor::Transparent, iDstRect);

        FRDGTextureRef sourceTexture = iGraphBuilder.RegisterExternalTexture(CreateRenderTarget(Texture->GetResource()->TextureRHI, TEXT("UOdysseyTextureLayer::sourceTexture")));

        FOdysseyBlendShader::BlendRect(
            iGraphBuilder,
            iFeatureLevel,
            iDestinationTexture,
            sourceTexture,
            iDestinationTexture,
            iSrcRect,
            iDstRect,
            iSrcTransform,
            EOdysseyBlendingMode::kNormal,
            EOdysseyAlphaMode::kNormal,
            1.f,
            EOdysseyAntiAliasing::Anisotropic
        );
    };

    return true;
}


#if WITH_EDITOR
FSimpleMulticastDelegate&
UOdysseyTextureLayer::OnThumbnailChanged()
{
    return mOnThumbnailChanged;
}

FSimpleMulticastDelegate&
UOdysseyTextureLayer::OnThumbnailDirtied()
{
    return mOnThumbnailDirtied;
}

void
UOdysseyTextureLayer::DirtyThumbnail()
{
    if (ThumbnailIsDirty)
        return;

    ThumbnailIsDirty = true;
    mOnThumbnailDirtied.Broadcast();
}

void
UOdysseyTextureLayer::UndirtyThumbnail()
{
    ThumbnailIsDirty = false;
}

bool
UOdysseyTextureLayer::IsThumbnailDirty() const
{
    return ThumbnailIsDirty;
}

#endif
