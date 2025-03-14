// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayer.h"

#include "Misc/OdysseyHandle.h"
#include "OdysseyMediaProvider.h"

#include "OdysseyTextureLayer.generated.h"

UCLASS(Abstract, HideDropdown, BlueprintType)
class ODYSSEYTEXTURE_API UOdysseyTextureLayer
    : public UOdysseyLayer
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    UTexture2D* GetTexture() const;
};
