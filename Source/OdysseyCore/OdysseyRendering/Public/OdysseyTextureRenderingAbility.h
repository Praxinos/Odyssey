// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "RenderGraphFwd.h"
#include "RHI.h"

#include "OdysseyRenderingAbility.h"

#include "OdysseyTextureRenderingAbility.generated.h"

struct FOdysseyRenderContext;
class FRDGBuilder;
class UFactory;
class UTexture2D;
class UTextureRenderTarget2D;

UINTERFACE(BlueprintType)
class ODYSSEYRENDERING_API UOdysseyTextureRenderingAbility : public UOdysseyRenderingAbility
{
    GENERATED_BODY()
};

class ODYSSEYRENDERING_API IOdysseyTextureRenderingAbility : public IOdysseyRenderingAbility
{
    GENERATED_BODY()

public:
    typedef TFunction< void(
        FRDGBuilder& /*iGraphBuilder*/,
        ERHIFeatureLevel::Type /*iFeatureLevel*/,
        FRDGTextureRef /*iDestinationTexture*/,
        const FIntRect& /*iSrcRect*/,
        const FIntRect& /*iDstRect*/,
        const FMatrix& /*iSrcTransform*/) > FRenderFunction;

public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Odyssey|Rendering", meta=(DisplayName="CreateRenderTarget"))
    UTextureRenderTarget2D* CreateRenderTarget_Blueprint();
    UTextureRenderTarget2D* CreateRenderTarget_Blueprint_Implementation();

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
    void Render_GameThread(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, uint64 iType, const FOdysseyRenderContext* RenderContext = nullptr) const;
    void Render_GameThread(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, uint64 iType, const FIntRect& iSrcRect, const FOdysseyRenderContext* RenderContext = nullptr) const;
    void Render_GameThread(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, uint64 iType, const FIntRect& iSrcRect, const FIntPoint& iPos, const FOdysseyRenderContext* RenderContext = nullptr) const;
    void Render_GameThread(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, uint64 iType, const FIntRect& iSrcRect, const FIntRect& iDstRect, const FOdysseyRenderContext* RenderContext = nullptr) const;

    void RenderAndResize_GameThread(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, uint64 iType, const FOdysseyRenderContext* RenderContext = nullptr) const;
    void RenderAndResize_GameThread(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, uint64 iType, const FIntRect& iSrcRect, const FIntRect& iDstRect, const FOdysseyRenderContext* RenderContext = nullptr) const;

    bool BuildRenderPipeline(FRenderFunction& OutRenderFunction,
        TArray<const IOdysseyTextureRenderingAbility*> Parents = {},
        const FOdysseyRenderContext* RenderContext = nullptr) const;

public:
    virtual UTextureRenderTarget2D* CreateRenderingRenderTarget() const = 0;
#if WITH_EDITOR
    virtual UTexture2D* CreateExportTexture( const FString& iAssetName, const FString& iPackagePath, UClass* iAssetClass, UFactory* iFactory ) = 0;
#endif

protected:
    virtual bool BuildsCompositeContent() const { return false; }
    virtual bool BuildRenderPipelineInternal(
        FRenderFunction& oRenderFunction,
        const TArray<const IOdysseyTextureRenderingAbility*>& iParents, const FOdysseyRenderContext* RenderContext
    ) const = 0;
};
