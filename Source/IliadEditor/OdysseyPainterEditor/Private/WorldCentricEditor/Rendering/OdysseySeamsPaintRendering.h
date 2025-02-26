// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "RHIDefinitions.h"

class FRHICommandList;
class UTextureRenderTarget2D;
class FGraphicsPipelineStateInitializer;

namespace OdysseySeamsPaintRendering
{
    /** Batched element parameters for mesh paint shaders */
    struct FOdysseySeamsPaintShaderParameters
    {
    public:
        UTextureRenderTarget2D* Stroke2D;
        UTextureRenderTarget2D* SeamMaskRenderTarget;

        float WidthPixelOffset;
        float HeightPixelOffset;
    };


    /** Binds the mesh paint vertex and pixel shaders to the graphics device */
    void SetSeamsPaintShaders(  FRHICommandList& iRHICmdList,
                                            FGraphicsPipelineStateInitializer& iGraphicsPSOInit,
                                            ERHIFeatureLevel::Type iFeatureLevel,
                                            const FMatrix& iTransform,
                                            const float iGamma,
                                            const FOdysseySeamsPaintShaderParameters& iShaderParams );

}
