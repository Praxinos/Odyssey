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
    void ODYSSEYMESHPAINTRENDERING_API SetSeamsPaintShaders(  FRHICommandList& iRHICmdList,
                                            FGraphicsPipelineStateInitializer& iGraphicsPSOInit,
                                            ERHIFeatureLevel::Type iFeatureLevel,
                                            const FMatrix& iTransform,
                                            const float iGamma,
                                            const FOdysseySeamsPaintShaderParameters& iShaderParams );

}
