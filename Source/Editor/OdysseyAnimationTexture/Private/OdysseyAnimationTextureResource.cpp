// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationTextureResource.h"

#include "OdysseyAnimationTexture.h"

FOdysseyAnimationTextureResource::~FOdysseyAnimationTextureResource()
{
}

FOdysseyAnimationTextureResource::FOdysseyAnimationTextureResource(UOdysseyAnimationTexture& iOwner, FIntPoint& oDimensions)
	: mOwner(iOwner)
	, mOwnerDimensions(oDimensions)
{
}

FString
FOdysseyAnimationTextureResource::GetFriendlyName() const
{
	return mOwner.GetPathName();
}

uint32
FOdysseyAnimationTextureResource::GetSizeX() const
{
	return mOwnerDimensions.X;
}

uint32
FOdysseyAnimationTextureResource::GetSizeY() const
{
	return mOwnerDimensions.Y;
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
