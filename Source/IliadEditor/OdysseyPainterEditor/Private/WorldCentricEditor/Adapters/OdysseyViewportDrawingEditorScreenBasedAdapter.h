// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "MeshPaintTypes.h"
#include "TexturePaintHelpers.h"

#include "IOdysseyViewportDrawingEditorAdapter.h"
#include "OdysseyScreenPaintRendering.h"
#include "OdysseySeamsPaintRendering.h"

class UTexture;
class FViewport;

/* -------------------------------------------------------------------  */

/** Screen Based Painting
* Batched element parameters for texture paint shaders used for paint blending and paint mask generation
*/
class FOdysseyScreenPaintBatchedElementParameters: public FBatchedElementParameters
{
public:
    /** Binds vertex and pixel shaders for this element */
    virtual void BindShaders(FRHICommandList& RHICmdList,FGraphicsPipelineStateInitializer& GraphicsPSOInit, EShaderPlatform InShaderPlatform,const FMatrix& InTransform,const float InGamma,const FMatrix& ColorWeights,const FTexture* Texture) override
    {
        OdysseyScreenPaintRendering::SetMeshPaintShaders(RHICmdList,GraphicsPSOInit, InShaderPlatform,InTransform,InGamma,ShaderParams);
    }

public:

    /** Shader parameters */
    OdysseyScreenPaintRendering::FOdysseyScreenPaintShaderParameters ShaderParams;
};


/** Mesh Based Painting, seams
* Batched element parameters for texture paint on the seams of the mesh
*/
class FOdysseyScreenSeamsPaintBatchedElementParameters : public FBatchedElementParameters
{
public:
    /** Binds vertex and pixel shaders for this element */
    virtual void BindShaders(FRHICommandList& RHICmdList, FGraphicsPipelineStateInitializer& GraphicsPSOInit, EShaderPlatform InShaderPlatform, const FMatrix& InTransform, const float InGamma, const FMatrix& ColorWeights, const FTexture* Texture) override
    {
        OdysseySeamsPaintRendering::SetSeamsPaintShaders(RHICmdList, GraphicsPSOInit, InShaderPlatform, InTransform, InGamma, ShaderParams);
    }

public:

    /** Shader parameters */
    OdysseySeamsPaintRendering::FOdysseySeamsPaintShaderParameters ShaderParams;
};

/** Painting adapter for the painter. Describes the method of mesh based painting in the viewport*/
class FOdysseyViewportDrawingEditorScreenBasedAdapter
    : public IOdysseyViewportDrawingEditorAdapter
{
public:
    /** destructor */
    ~FOdysseyViewportDrawingEditorScreenBasedAdapter();

    /** constructor */
    FOdysseyViewportDrawingEditorScreenBasedAdapter(FOdysseyViewportDrawingEditorExtension* iExtension);

public:
    virtual void Initialize() override;
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
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT( OdysseyViewportDrawingEditorScreenBasedAdapter, STATGROUP_Tickables); }

private:
    float GetStampQuality();
    virtual ::ULIS::FEvent StampOverride( UOdysseyBrushAssetBase::FStampParams iStampParams ) override;

private:
    UTexture2D* mStrokeBufferTexture2D;

    TArray<TArray<FLinearColor>> mColorData;
    //TArray<FColor*> mColorDataPtr;
    FRenderCommandFence mPixelFence;

    UTextureRenderTarget2D* mPaintingTexture2DRenderTarget;
    UTextureRenderTarget2D* mStrokeBufferRenderTarget2D;
    UTextureRenderTarget2D* mSeamRenderTarget2D;
};
