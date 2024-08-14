// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayer.h"

#include "UObject/OdysseyObjectPropertyTracker.h"
#include "Misc/OdysseyHandle.h"
#include "OdysseyMediaProvider.h"
#include <ULIS>

#include "OdysseyTextureLayer.generated.h"

UCLASS(BlueprintType)
class ODYSSEYTEXTURE_API UOdysseyTextureLayer
    : public UOdysseyLayer
{
    GENERATED_BODY()

protected:
    virtual void IsActivatedChanged() override;
    virtual void ChildrenChanged() override;

public:
	UFUNCTION(BlueprintPure, Category="Odyssey | LayerStack")
	UTexture2D* GetTexture() const;

public:
	//FOdysseyImageRenderingAbility overrides
	virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame = 0, FImageRendererFilter iFilter = FImageRendererFilter()) const override;
	virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame = 0) const override;
	virtual TArray<::ULIS::FRectI> GetImageRenderingRects() const override;
};
