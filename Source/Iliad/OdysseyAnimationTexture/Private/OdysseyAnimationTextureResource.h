// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "TextureResource.h"

class UOdysseyAnimationTexture;

class FOdysseyAnimationTextureResource
    : public FTextureResource
{
public:
    ~FOdysseyAnimationTextureResource();
    FOdysseyAnimationTextureResource(UOdysseyAnimationTexture& iOwner, FIntPoint& oDimensions);

public:
    //~ FTextureResource interface
    virtual FString GetFriendlyName() const override;
    virtual uint32 GetSizeX() const override;
    virtual uint32 GetSizeY() const override;

public:
    void UpdateTextureReference(FRHITexture2D* iNewTexture);

private:
    UOdysseyAnimationTexture& mOwner;
    FIntPoint& mOwnerDimensions;
};
