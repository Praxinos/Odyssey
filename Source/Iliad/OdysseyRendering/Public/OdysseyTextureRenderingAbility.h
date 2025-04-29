// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "OdysseyRenderingAbility.h"

#include "OdysseyTextureRenderingAbility.generated.h"

UINTERFACE(BlueprintType)
class ODYSSEYRENDERING_API UOdysseyTextureRenderingAbility : public UOdysseyRenderingAbility
{
    GENERATED_BODY()
};

class ODYSSEYRENDERING_API IOdysseyTextureRenderingAbility : public IOdysseyRenderingAbility
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Odyssey|Rendering")
    void Render(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame) const;
    void Render_Implementation(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame) const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Odyssey|Rendering")
    void RenderRects(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const TArray<FIntRect>& iRects) const;
    void RenderRects_Implementation(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const TArray<FIntRect>& iRects) const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Odyssey|Rendering")
    void RenderRectsAtPosition(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const TArray<FIntRect>& iRects, const FIntPoint& iPos) const;
    void RenderRectsAtPosition_Implementation(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const TArray<FIntRect>& iRects, const FIntPoint& iPos) const;

protected:
    virtual void RenderToTextureFromRects(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const TArray<FIntRect>& iRects, const FIntPoint& iPos) const = 0;
};
