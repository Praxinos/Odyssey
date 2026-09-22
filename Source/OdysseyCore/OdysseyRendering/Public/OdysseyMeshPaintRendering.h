// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "RHIDefinitions.h"
#include "RHIFeatureLevel.h"

class FRHICommandList;
class UTextureRenderTarget2D;
class FGraphicsPipelineStateInitializer;
class UTexture2D;

namespace OdysseyMeshPaintRendering
{
    /** Batched element parameters for mesh paint shaders */
    struct FOdysseyMeshPaintShaderParameters
    {
    public:
        UTexture2D* Stroke2D;

        FMatrix WorldToBrushMatrix;
        FVector2D TextureHitPoint;
        float StampQuality;
    };


    /** Binds the mesh paint vertex and pixel shaders to the graphics device */
    ODYSSEYRENDERING_API void SetMeshPaintShaders(  FRHICommandList& iRHICmdList,
                                            FGraphicsPipelineStateInitializer& iGraphicsPSOInit,
                                            ERHIFeatureLevel::Type iFeatureLevel,
                                            const FMatrix& iTransform,
                                            const float iGamma,
                                            const FOdysseyMeshPaintShaderParameters& iShaderParams );

}
