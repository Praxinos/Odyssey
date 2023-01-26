// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "IOdysseyViewportDrawingEditorAdapter.h"

#include "OdysseyMeshPaintRendering.h"
#include "OdysseySeamsPaintRendering.h"
#include "MeshPaintTypes.h"
#include "TexturePaintHelpers.h"

#include "OdysseyViewportDrawingEditorMeshBasedAdapter.Generated.h"

/** Helper Struct for mesh paint rendering --------------------------------- */

USTRUCT()
struct FOdysseyPaintTexture2DData
{
    GENERATED_BODY()

        /** The original texture that we're painting */
        UPROPERTY(Transient)
        UTexture2D* PaintingTexture2D;

    bool bIsPaintingTexture2DModified;

    /** A copy of the original texture we're painting, used for restoration. */
    UPROPERTY(Transient)
        UTexture2D* PaintingTexture2DDuplicate;

    /** Render target texture for painting */
    UPROPERTY(Transient)
        UTextureRenderTarget2D* PaintRenderTargetTexture;

    /** Render target texture used as an input while painting that contains a clone of the original image */
    UPROPERTY(Transient)
        UTextureRenderTarget2D* CloneRenderTargetTexture;

    /** List of materials we are painting on */
    UPROPERTY(Transient)
        TArray< UMaterialInterface* > PaintingMaterials;

    /** Default ctor */
    FOdysseyPaintTexture2DData() :
        PaintingTexture2D(NULL),
        bIsPaintingTexture2DModified(false),
        PaintingTexture2DDuplicate(nullptr),
        PaintRenderTargetTexture(nullptr),
        CloneRenderTargetTexture(nullptr)
    {}

    FOdysseyPaintTexture2DData(UTexture2D* InPaintingTexture2D, bool InbIsPaintingTexture2DModified = false) :
        PaintingTexture2D(InPaintingTexture2D),
        bIsPaintingTexture2DModified(InbIsPaintingTexture2DModified),
        PaintRenderTargetTexture(nullptr),
        CloneRenderTargetTexture(nullptr)
    {}

    /** Serializer */
    void AddReferencedObjects(FReferenceCollector& Collector)
    {
        // @todo MeshPaint: We're relying on GC to clean up render targets, can we free up remote memory more quickly?
        Collector.AddReferencedObject(PaintingTexture2D);
        Collector.AddReferencedObject(PaintRenderTargetTexture);
        Collector.AddReferencedObject(CloneRenderTargetTexture);
        for (int32 Index = 0; Index < PaintingMaterials.Num(); Index++)
        {
            Collector.AddReferencedObject(PaintingMaterials[Index]);
        }
    }
};

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
    void BuildPaintingTexture2DRenderTarget();
    void GatherTextureTriangles(IMeshPaintGeometryAdapter* iAdapter, int32 iTriangleIndex, const int32 iVertexIndices[3], TArray<FTexturePaintTriangleInfo>* iTriangleInfo, TArray<FTexturePaintMeshSectionInfo>* iSectionInfos, int32 iUVChannelIndex);
    TArray<::ULIS::FRectI> GetMinimalRectanglesForTriangleSet( TArray<FTexturePaintTriangleInfo>& iTriangles, int iMaxWidth, int iMaxHeight );

private:
    virtual ::ULIS::FEvent StampOverride( UOdysseyBrushAssetBase::FStampParams iStampParams ) override;

protected:
    virtual void BindStampBrushInstance(UOdysseyBrushAssetBase* iBindBrush) override;

private:
    /** Stores data associated with our paint target textures */
    UPROPERTY(Transient)
    TMap< UTexture2D*, FOdysseyPaintTexture2DData > mPaintTargetData;

    UTexture2D* mStrokeBufferTexture2D;

    /** A render target to store the stroke pixels we want to stamp */
    UTextureRenderTarget2D* mStrokeBufferRenderTarget2D;

    /** A render target to store the pixels of the seams */
    UTextureRenderTarget2D* mSeamRenderTarget2D;

    TArray<TArray<FLinearColor>> mColorData;
    //TArray<FColor*> mColorDataPtr;
    FRenderCommandFence mPixelFence;
};
