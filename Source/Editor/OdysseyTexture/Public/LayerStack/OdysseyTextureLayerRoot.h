// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyTextureLayer.h"
#include "OdysseyTextureLayerRoot.generated.h"

UCLASS()
class ODYSSEYTEXTURE_API UOdysseyTextureLayerRoot
	: public UOdysseyTextureLayer
{
    GENERATED_BODY()

public:
    UOdysseyTextureLayerRoot();

protected:
    virtual void RenderImageChanged(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive) override;
};