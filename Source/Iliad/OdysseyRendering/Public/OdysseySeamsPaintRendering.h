// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "RHI.h"

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
    ODYSSEYRENDERING_API void SetSeamsPaintShaders(  FRHICommandList& iRHICmdList,
                                            FGraphicsPipelineStateInitializer& iGraphicsPSOInit,
                                            ERHIFeatureLevel::Type iFeatureLevel,
                                            const FMatrix& iTransform,
                                            const float iGamma,
                                            const FOdysseySeamsPaintShaderParameters& iShaderParams );

}
