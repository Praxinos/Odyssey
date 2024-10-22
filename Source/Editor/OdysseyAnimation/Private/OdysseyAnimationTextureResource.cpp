// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

/* #include "OdysseyAnimationTextureResource.h"

#include "OdysseyAnimationTexture.h"

FOdysseyAnimationTextureResource::~FOdysseyAnimationTextureResource()
{
}

FOdysseyAnimationTextureResource::FOdysseyAnimationTextureResource(UOdysseyAnimationTexture& iOwner)
	: mOwner(iOwner)
{
	FTextureCompilingManager::Get().FinishCompilation({playerTexture});
	FTextureResource* playerResource = playerTexture->GetResource();
	if ( !playerResource )
		TextureReferenceRHI = nullptr;
}

FOdysseyAnimationTextureResource::FOdysseyAnimationTextureResource(UTexture2D* InOwner, const FTexture2DResource* InProxiedResource)
	: mOwner(InOwner)
	, mOwnerDimensions(oDimensions)
{
	TextureReferenceRHI = InOwner->TextureReference.TextureReferenceRHI;
}

void
FOdysseyAnimationTextureResource::InitRHI(FRHICommandListBase& iCommandList)
{
	RefreshSamplerStates();
	RHIUpdateTextureReference(TextureReferenceRHI, TextureRHI);
	FTextureResource::InitRHI(iCommandList);
}

void
FStreamableTextureResource::ReleaseRHI()
{
	if (ensure(TextureReferenceRHI.IsValid()))
	{
		RHIUpdateTextureReference(TextureReferenceRHI, nullptr);
	}

	TextureRHI.SafeRelease();
	FTextureResource::ReleaseRHI();
}

void
FOdysseyAnimationTextureResource::RefreshSamplerStates()
{
	// Create the sampler state RHI resource.
	FSamplerStateInitializerRHI SamplerStateInitializer
	(
		Filter,
		AddressU,
		AddressV,
		AddressW,
		MipBias,
		MaxAniso
	);
	SamplerStateRHI = GetOrCreateSamplerState(SamplerStateInitializer);

	// Create a custom sampler state for using this texture in a deferred pass, where ddx / ddy are discontinuous
	FSamplerStateInitializerRHI DeferredPassSamplerStateInitializer
	(
		Filter,
		AddressU,
		AddressV,
		AddressW,
		MipBias,
		// Disable anisotropic filtering, since aniso doesn't respect MaxLOD
		1,
		0,
		// Prevent the less detailed mip levels from being used, which hides artifacts on silhouettes due to ddx / ddy being very large
		// This has the side effect that it increases minification aliasing on light functions
		2
	);

	DeferredPassSamplerStateRHI = GetOrCreateSamplerState(DeferredPassSamplerStateInitializer);
}

FString
FOdysseyAnimationTextureResource::GetFriendlyName() const
{
	return mOwner.GetPathName();
}

uint32
FOdysseyAnimationTextureResource::GetSizeX() const
{
	if (!TextureRHI)
		return 0;

	return TextureRHI->GetSizeX();
}

uint32
FOdysseyAnimationTextureResource::GetSizeY() const
{
	if (!TextureRHI)
		return 0;

	return TextureRHI->GetSizeY();
}

void
FOdysseyAnimationTextureResource::UpdateTextureReference(FRHITexture2D* iNewTexture)
{
	ENQUEUE_RENDER_COMMAND(UpdateTextureReferenceData)(
		[this, iNewTexture](FRHICommandListImmediate& RHICmdList)
		{
			TextureRHI = iNewTexture;
			RHIUpdateTextureReference(mOwner.TextureReference.TextureReferenceRHI, iNewTexture);
			if ( TextureRHI != nullptr )
				mOwnerDimensions = FIntPoint(TextureRHI->GetSizeX(), TextureRHI->GetSizeY());
			else
				mOwnerDimensions = FIntPoint::ZeroValue;
		}
	);
    
    FRenderCommandFence fence;
    fence.BeginFence();
    fence.Wait();
}
*/