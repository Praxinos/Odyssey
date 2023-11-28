// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayer.h"

#include "UObject/OdysseyObjectPropertyTracker.h"
#include "Misc/OdysseyHandle.h"
#include "OdysseyMediaProvider.h"
#include "OdysseyStaticImageRenderingAbility.h"
#include <ULIS>

#include "OdysseyTextureLayer.generated.h"

UCLASS(BlueprintType)
class ODYSSEYTEXTURE_API UOdysseyTextureLayer
    : public UOdysseyLayer
    , public FOdysseyStaticImageRenderingAbility
{
    GENERATED_BODY()

protected:
    virtual void IsActivatedChanged() override;
    virtual void ChildrenChanged() override;

public:
	//FOdysseyImageRenderingAbility overrides
	virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, FImageRendererFilter iFilter = FImageRendererFilter()) const override;
	virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType) const override;
	virtual TArray<::ULIS::FRectI> GetImageRenderingRects() const override;
};
