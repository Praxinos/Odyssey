#pragma once

#include "CoreMinimal.h"
#include "RHIDefinitions.h"

class FRHICommandList;
class UTextureRenderTarget2D;
class FGraphicsPipelineStateInitializer;

namespace OdysseyMeshPaintRendering
{
	/** Batched element parameters for mesh paint shaders */
	struct FOdysseyMeshPaintShaderParameters
	{
	public:
        UTexture2D* Stroke2D;

		FMatrix WorldToBrushMatrix;
        FVector2D TextureHitPoint;
	};


	/** Binds the mesh paint vertex and pixel shaders to the graphics device */
	void ODYSSEYMESHPAINTRENDERING_API SetMeshPaintShaders(  FRHICommandList& iRHICmdList,
											FGraphicsPipelineStateInitializer& iGraphicsPSOInit,
											ERHIFeatureLevel::Type iFeatureLevel, 
											const FMatrix& iTransform,
											const float iGamma,
											const FOdysseyMeshPaintShaderParameters& iShaderParams );

}
