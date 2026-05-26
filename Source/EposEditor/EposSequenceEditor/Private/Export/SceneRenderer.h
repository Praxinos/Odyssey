// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"

//---

class FTextureRenderTargetResource;
class ISequencer;
class UCameraComponent;
struct FExportPanel;

class FSceneRenderer
{
public:
    FSceneRenderer( TWeakPtr<ISequencer> iSequencer, const FExportPanel* iPanel, const FIntPoint& iSize, EViewModeIndex iViewMode );

    bool RenderPlane( TArray<FColor>& oSamples );
    bool RenderPlane( FTextureRenderTargetResource* oRenderTarget );

private:
    void PreDraw();
    void PostDraw();
    void GetViewCamera();

    void RenderToTexture( TArray<FColor>& oSamples, FTextureRenderTargetResource* oRenderTarget = nullptr );

private:
    TWeakPtr<ISequencer>                mSequencer;
    const FExportPanel*                 mCurrentPanel = nullptr;
    FIntPoint                           mSize;
    EViewModeIndex                      mViewMode;

private:
    UCameraComponent*                   mCameraComponent = nullptr;
};
