// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "IOdysseyViewportDrawingEditorAdapter.h"

#include "OdysseyMeshPaintRendering.h"
#include "OdysseySeamsPaintRendering.h"
#include "MeshPaintTypes.h"
#include "TexturePaintHelpers.h"

/* -------------------------------------------------------------------  */

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


/** Mesh Based Painting, seams
* Batched element parameters for texture paint on the seams of the mesh
*/
class FOdysseySeamsPaintBatchedElementParameters : public FBatchedElementParameters
{
public:
    /** Binds vertex and pixel shaders for this element */
    virtual void BindShaders(FRHICommandList& RHICmdList, FGraphicsPipelineStateInitializer& GraphicsPSOInit, ERHIFeatureLevel::Type InFeatureLevel, const FMatrix& InTransform, const float InGamma, const FMatrix& ColorWeights, const FTexture* Texture) override
    {
        OdysseySeamsPaintRendering::SetSeamsPaintShaders(RHICmdList, GraphicsPSOInit, InFeatureLevel, InTransform, InGamma, ShaderParams);
    }

public:

    /** Shader parameters */
    OdysseySeamsPaintRendering::FOdysseySeamsPaintShaderParameters ShaderParams;
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

    virtual void Tick(float iDelta) override;

    virtual void RenderInteractorWidget(const FSceneView* iView, FViewport* iViewport, FPrimitiveDrawInterface* iPDI) override;

    void BuildPaintingTexture2DRenderTarget();
    void GatherTextureTriangles(IMeshPaintGeometryAdapter* iAdapter, int32 iTriangleIndex, const int32 iVertexIndices[3], TArray<FTexturePaintTriangleInfo>* iTriangleInfo, TArray<FTexturePaintMeshSectionInfo>* iSectionInfos, int32 iUVChannelIndex);
    TArray<::ULIS::FRectI> GetMinimalRectanglesForTriangleSet( TArray<FTexturePaintTriangleInfo>& iTriangles, int iMaxWidth, int iMaxHeight );

private:
    //A multiplier to the size of the brush so that we keep a good resolution for the stamp in cases where the size of the texture and the size of the mesh are pretty different
    float GetStampQuality();
    virtual ::ULIS::FEvent StampOverride( UOdysseyBrushAssetBase::FStampParams iStampParams ) override;

protected:
    virtual void BindStampBrushInstance(UOdysseyBrushAssetBase* iBindBrush) override;

private:
    UTexture2D* mStrokeBufferTexture2D;

    /** A render target to store the stroke pixels we want to stamp */
    UTextureRenderTarget2D* mStrokeBufferRenderTarget2D;

    /** A render target to store the pixels of the seams */
    UTextureRenderTarget2D* mSeamRenderTarget2D;

    TArray<TArray<FLinearColor>> mColorData;
    //TArray<FColor*> mColorDataPtr;
    FRenderCommandFence mPixelFence;
};
