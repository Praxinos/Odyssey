// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "IOdysseyViewportDrawingEditorAdapter.h"

#include "OdysseyMeshPaintRendering.h"

/** Mesh Based Painting
* Batched element parameters for texture paint shaders used for paint blending and paint mask generation
*/
class FOdysseyMeshPaintBatchedElementParameters: public FBatchedElementParameters
{
public:
    /** Binds vertex and pixel shaders for this element */
    virtual void BindShaders(FRHICommandList& RHICmdList,FGraphicsPipelineStateInitializer& GraphicsPSOInit,ERHIFeatureLevel::Type InFeatureLevel,const FMatrix& InTransform,const float InGamma,const FMatrix& ColorWeights,const FTexture* Texture) override
    {
        OdysseyMeshPaintRendering::SetMeshPaintShaders(RHICmdList,GraphicsPSOInit,InFeatureLevel,InTransform,InGamma,ShaderParams);
    }

public:

    /** Shader parameters */
    OdysseyMeshPaintRendering::FOdysseyMeshPaintShaderParameters ShaderParams;
};

/** Painting adapter for the painter. Describes the method of mesh based painting in the viewport*/
class FOdysseyViewportDrawingEditorMeshBasedAdapter : public IOdysseyViewportDrawingEditorAdapter
{
public:
	/** destructor */
	~FOdysseyViewportDrawingEditorMeshBasedAdapter();

	/** constructor */
	FOdysseyViewportDrawingEditorMeshBasedAdapter(TSharedPtr<FOdysseyViewportDrawingEditor> iEditor);

public:      
    virtual void PrepareAdapterForPainting() override;
    virtual void StartPainting() override;
	virtual void Paint() override;
    virtual void FinishPainting() override;

    virtual void Tick() override;
};
