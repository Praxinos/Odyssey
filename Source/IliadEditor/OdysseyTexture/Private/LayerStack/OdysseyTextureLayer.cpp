// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTextureLayer.h"

#include "OdysseyTextureLayerStack.h"
#include "ScreenPass.h"
#include "OdysseyBlendShader.h"
#include "TextureCompiler.h"

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
    if (!Texture)
    {
        const_cast<UOdysseyTextureLayer*>(this)->InitTexture();
        Texture->UpdateResource();
        FTextureCompilingManager::Get().FinishCompilation({ Texture });
    }

    return Texture;
}

void
UOdysseyTextureLayer::InitTexture()
{
    if ( !Texture )
    {
        UTexture2D* OwnerTexture = GetTexture();
        if (!OwnerTexture)
            return;

        ETextureSourceFormat ownerTextureFormat = OwnerTexture->Source.GetFormat();
        ETextureSourceFormat layerTextureFormat = TSF_BGRA8;
        switch(ownerTextureFormat)
        {
            case TSF_Invalid: checkf(false, TEXT("Invalid Texture Format")); break;

            case TSF_G8:
            case TSF_BGRA8:
            case TSF_BGRE8:
            case TSF_RGBA8_DEPRECATED:
            case TSF_RGBE8_DEPRECATED:
            {
                layerTextureFormat = TSF_BGRA8;
            }
            break;

            case TSF_G16:
            case TSF_RGBA16:
            {
                layerTextureFormat = TSF_RGBA16;
            }
            break;

            case TSF_RGBA16F:
            case TSF_RGBA32F:
            case TSF_R16F:
            case TSF_R32F:
            {
                layerTextureFormat = TSF_RGBA32F;
            }
            break;
        };

        Texture = NewObject<UTexture2D>(this, TEXT("Texture"));
        Texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
        Texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
        Texture->Filter = TextureFilter::TF_Nearest;
        Texture->Source.Init(
            OwnerTexture->Source.GetSizeX(),
            OwnerTexture->Source.GetSizeY(),
            1,
            1,
            layerTextureFormat
        );
    }
}

bool
UOdysseyTextureLayer::BuildRenderPipelineInternal(
    FFrameNumber iFrame,
    uint64 iType,
    IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
    const IOdysseyTextureRenderingAbility::FCanRenderFunction& iCanRenderFunction,
    const TArray<const IOdysseyTextureRenderingAbility*>& iParents
) const
{
    if (bCanHaveChildren)
        return Super::BuildRenderPipelineInternal(iFrame, iType, oRenderFunction, iCanRenderFunction, iParents);

#if WITH_EDITOR
    if ( !Texture )
    {
        const_cast<UOdysseyTextureLayer*>(this)->InitTexture();
        Texture->UpdateResource();
    }
#endif

    FTextureCompilingManager::Get().FinishCompilation({ Texture });

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

#endif
