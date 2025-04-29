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
    DECLARE_DELEGATE_RetVal_TwoParams(bool, FCanRenderFunction, const IOdysseyTextureRenderingAbility* /*iAbility*/, TArray<const IOdysseyTextureRenderingAbility*> /* iParents (from highest to lowest) */);

    typedef TFunction< void(
        FRDGBuilder& /*iGraphBuilder*/,
        ERHIFeatureLevel::Type /*iFeatureLevel*/,
        FRDGTextureRef /*iDestinationTexture*/,
        const FIntRect& /*iSrcRect*/,
        const FIntRect& /*iDstRect*/,
        const FMatrix& /*iSrcTransform*/) > FRenderFunction;

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
    void Render_GameThread(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, uint64 iType, FCanRenderFunction iCanRenderFunction = FCanRenderFunction()) const;
    void Render_GameThread(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, uint64 iType, const FIntRect& iSrcRect, FCanRenderFunction iCanRenderFunction = FCanRenderFunction()) const;
    void Render_GameThread(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, uint64 iType, const FIntRect& iSrcRect, const FIntPoint& iPos, FCanRenderFunction iCanRenderFunction = FCanRenderFunction()) const;
    void Render_GameThread(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, uint64 iType, const FIntRect& iSrcRect, const FIntRect& iDstRect, FCanRenderFunction iCanRenderFunction = FCanRenderFunction()) const;

    bool BuildRenderPipeline(
        FFrameNumber iFrame,
        uint64 iType,
        FRenderFunction& oRenderFunction,
        const FCanRenderFunction& iCanRenderFunction = FCanRenderFunction(),
        TArray<const IOdysseyTextureRenderingAbility*> iParents = {}
    ) const
    {
        if (iCanRenderFunction.IsBound() && !iCanRenderFunction.Execute(this, iParents))
            return false;

        iParents.Add(this);
        return BuildRenderPipelineInternal(iFrame, iType, oRenderFunction, iCanRenderFunction, iParents);
    }

protected:
    virtual bool BuildRenderPipelineInternal(
        FFrameNumber iFrame,
        uint64 iType,
        FRenderFunction& oRenderFunction,
        const FCanRenderFunction& iCanRenderFunction,
        const TArray<const IOdysseyTextureRenderingAbility*>& iParents
    ) const = 0;
};
