// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "IOdysseyViewportDrawingEditorAdapter.h"

#include "Rendering/OdysseyMeshPaintRendering.h"
#include "Rendering/OdysseySeamsPaintRendering.h"
#include "MeshPaintTypes.h"
#include "TexturePaintHelpers.h"
#include "OdysseySurfaceTexture2DEditable.h"

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
class FOdysseyViewportDrawingEditorMeshBasedAdapter
    : public IOdysseyViewportDrawingEditorAdapter
    , public FTickableEditorObject //Allows us to react to Tick events
{
public:
    /** destructor */
    ~FOdysseyViewportDrawingEditorMeshBasedAdapter();

    /** constructor */
    FOdysseyViewportDrawingEditorMeshBasedAdapter(FOdysseyViewportDrawingEditorExtension* iExtension);

public:
    virtual void Initialize() override;
    virtual void Finalize() override;
    virtual void SetTool(UOdysseyPainterEditorTool* iTool) override;
    virtual void SetTexture(UTexture* iTexture) override;

    void InitializeRenderTarget();
    void FinalizeRenderTarget();

    virtual void RenderInteractorWidget(const FSceneView* iView, FViewport* iViewport, FPrimitiveDrawInterface* iPDI) override;

    void GatherTextureTriangles(IMeshPaintGeometryAdapter* iAdapter, int32 iTriangleIndex, const int32 iVertexIndices[3], TArray<FTexturePaintTriangleInfo>* iTriangleInfo, TArray<FTexturePaintMeshSectionInfo>* iSectionInfos, int32 iUVChannelIndex);
    TArray<::ULIS::FRectI> GetMinimalRectanglesForTriangleSet( TArray<FTexturePaintTriangleInfo>& iTriangles, int iMaxWidth, int iMaxHeight );

public:
    /** Painting Methods **/
    virtual void StartPainting() override;
    virtual void Paint() override;
    virtual void FinishPainting() override;

private:
    // FTickableEditorObject implementation
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT( FOdysseyViewportDrawingEditorMeshBasedAdapter, STATGROUP_Tickables); }

private:
    //A multiplier to the size of the brush so that we keep a good resolution for the stamp in cases where the size of the texture and the size of the mesh are pretty different
    float GetStampQuality();
    virtual ::ULIS::FEvent StampOverride( UOdysseyBrushAssetBase::FStampParams iStampParams ) override;

protected:
    virtual void BindStampBrushInstance(UOdysseyBrushAssetBase* iBindBrush) override;

private:
    UTexture2D* mStrokeBufferTexture2D;
    FOdysseySurfaceTexture2DEditable* mStrokeBufferSurfaceTexture2DEditable;

    TArray<TArray<FLinearColor>> mColorData;
    //TArray<FColor*> mColorDataPtr;
    FRenderCommandFence mPixelFence;

    UTextureRenderTarget2D* mPaintingTexture2DRenderTarget;
    UTextureRenderTarget2D* mStrokeBufferRenderTarget2D;
    UTextureRenderTarget2D* mSeamRenderTarget2D;
};
