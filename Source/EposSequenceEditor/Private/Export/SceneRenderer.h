// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

//---

struct FExportImageSequenceSettings;
struct FExportImageSequenceStruct;
class ISequencer;
class UCameraComponent;

class FSceneRenderer
{
public:
    FSceneRenderer( TWeakPtr<ISequencer> iSequencer, const FExportImageSequencePanel* iImageSequencePanel, const FExportImageSequenceOptions* iOptions );

    void OverrideSize( const FIntPoint& iSize );

    bool RenderPlane( TArray<FColor>& oSamples );
    bool RenderPlane( FTextureRenderTargetResource* oRenderTarget );

private:
    void PreDraw();
    void PostDraw();
    void GetViewCamera();

    void RenderToTexture( TArray<FColor>& oSamples, FTextureRenderTargetResource* oRenderTarget = nullptr );

private:
    TWeakPtr<ISequencer>                mSequencer;
    const FExportImageSequencePanel*    mCurrentPanel { nullptr };
    const FExportImageSequenceOptions*  mImageSequenceOptions { nullptr };
    FIntPoint                           mSize;

private:
    UCameraComponent*                   mCameraComponent { nullptr };
};
