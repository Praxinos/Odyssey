// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "OdysseyRenderingAbility.h"
#include "OdysseyTextureRenderer.h"

#include "OdysseyTextureRenderingAbility.generated.h"

UENUM()
enum class EOdysseyTextureRenderingMode
{
    Runtime,
    Editor,

};

typedef TFunction< void(
    FRDGBuilder& /*iGraphBuilder*/,
    ERHIFeatureLevel::Type /*iFeatureLevel*/,
    FRDGTextureRef /*iDestinationTexture*/,
    const FIntRect& /*iSrcRect*/,
    const FIntRect& /*iDstRect*/,
    const FMatrix& /*iSrcTransform*/) > FOdysseyTextureRenderFunction;

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
    void RenderRect(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const FIntRect& iSrcRect) const;
    void RenderRect_Implementation(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const FIntRect& iRect) const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Odyssey|Rendering")
    void RenderRectAtPosition(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const FIntRect& iRect, const FIntPoint& iPos) const;
    void RenderRectAtPosition_Implementation(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const FIntRect& iSrcRect, const FIntPoint& iPos) const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Odyssey|Rendering")
    void RenderRectAtRect(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const FIntRect& iRect, const FIntRect& iDstRect) const;
    void RenderRectAtRect_Implementation(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const FIntRect& iSrcRect, const FIntRect& iDstRect) const;

public:
    void Render_GameThread(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, EOdysseyRenderingType iType) const;
    void Render_GameThread(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, EOdysseyRenderingType iType, const FIntRect& iSrcRect) const;
    void Render_GameThread(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, EOdysseyRenderingType iType, const FIntRect& iSrcRect, const FIntPoint& iPos) const;
    void Render_GameThread(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, EOdysseyRenderingType iType, const FIntRect& iSrcRect, const FIntRect& iDstRect) const;

    virtual bool BuildRenderPipeline(
        FFrameNumber iFrame,
        EOdysseyRenderingType iType,
        FOdysseyTextureRenderFunction& oRenderFunction
    ) const = 0;
};
