// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyViewportDrawingEditorMeshBasedAdapter.h"
#include "MeshPaintHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "CanvasTypes.h"
#include "CanvasItem.h"
#include "OdysseyViewportDrawingEditorUtils.h"
#include <ULIS>
#include "ULISLoaderModule.h"
#include "ULISUtils.h"
#include "ULISEventBuilder.h"


#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorMeshBasedAdapter"

FOdysseyViewportDrawingEditorMeshBasedAdapter::~FOdysseyViewportDrawingEditorMeshBasedAdapter()
{
    RemoveTextureOverride();

    mStrokeBufferRenderTarget2D->ConditionalBeginDestroy();
    mStrokeBufferRenderTarget2D = nullptr;
}

FOdysseyViewportDrawingEditorMeshBasedAdapter::FOdysseyViewportDrawingEditorMeshBasedAdapter(TSharedPtr<FOdysseyViewportDrawingEditor> iEditor) :
    IOdysseyViewportDrawingEditorAdapter::IOdysseyViewportDrawingEditorAdapter(iEditor)
{
    PrepareAdapterForPainting();
}

void FOdysseyViewportDrawingEditorMeshBasedAdapter::PrepareAdapterForPainting()
{    
    if (mEditor->Component() == nullptr || mEditor->Texture() == nullptr)
        return;

    IOdysseyViewportDrawingEditorAdapter::PrepareAdapterForPainting();

    if( mState == eState::kIdle )
    {
        BuildPaintingTexture2DRenderTarget();
        TexturePaintHelpers::GenerateSeamMask(mEditor->Component(), mEditor->GetUVIndexUsedByCurrentTexture(), mSeamRenderTarget2D, mEditor->Texture(), mPaintingTexture2DRenderTarget);
    }

    //We're using percentage size for this adapter, so we put the true maximum mesh size
    mEditor->GetGUI()->GetTopTab()->SetMeshMaxSize(mEditor->GetMeshComponentMaxSize() * GetStampQuality());

    mState = eState::kIdleReady;
}

void FOdysseyViewportDrawingEditorMeshBasedAdapter::StartPainting()
{
    mEditor->GetSelectedTool()->OnMouseDown(mCurrentStrokeRay.mPoint, EKeys::LeftMouseButton);

    if (mPaintingTexture2DRenderTarget)
        TexturePaintHelpers::CopyTextureToRenderTargetTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, GEditor->GetEditorWorldContext().World()->FeatureLevel);
}

void FOdysseyViewportDrawingEditorMeshBasedAdapter::Paint()
{
    mEditor->GetSelectedTool()->OnMouseDrag(mCurrentStrokeRay.mPoint);
}

void FOdysseyViewportDrawingEditorMeshBasedAdapter::FinishPainting()
{
    mEditor->GetSelectedTool()->OnMouseUp( mCurrentStrokeRay.mPoint, EKeys::LeftMouseButton );

    if (mPaintingTexture2DRenderTarget)
        TexturePaintHelpers::CopyTextureToRenderTargetTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, GEditor->GetEditorWorldContext().World()->FeatureLevel);
}

void FOdysseyViewportDrawingEditorMeshBasedAdapter::Tick(float iDelta)
{
    if (mEditor->Texture() && mPaintingTexture2DRenderTarget)
        TexturePaintHelpers::CopyTextureToRenderTargetTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, GEditor->GetEditorWorldContext().World()->FeatureLevel);

    mEditor->GetSelectedTool()->Tick(iDelta);
}


void FOdysseyViewportDrawingEditorMeshBasedAdapter::RenderInteractorWidget(const FSceneView* iView, FViewport* iViewport, FPrimitiveDrawInterface* iPDI)
{
    UOdysseyPainterEditorRasterDrawingTool* drawingTool = nullptr;
    if (mEditor->GetSelectedTool() && mEditor->GetSelectedTool()->IsA(UOdysseyPainterEditorRasterDrawingTool::StaticClass()))
        drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetSelectedTool());
    else
        return;

    const TSharedPtr<IMeshPaintGeometryAdapter>* meshAdapterPtr = mEditor->ComponentToAdapterMap().Find(mEditor->Component());
    if (!meshAdapterPtr)
        return;

    TSharedPtr<IMeshPaintGeometryAdapter> meshAdapter = *meshAdapterPtr;

    TArray<MeshPaintHelpers::FPaintRay> paintRays;
    MeshPaintHelpers::RetrieveViewportPaintRays(iView, iViewport, iPDI, paintRays);

    for (const MeshPaintHelpers::FPaintRay& paintRay : paintRays)
    {
        FHitResult traceHitResult(1.0f);
        const FVector rayEnd(paintRay.CameraLocation + paintRay.RayDirection * HALF_WORLD_MAX);

        meshAdapter->LineTraceComponent(traceHitResult, paintRay.CameraLocation, rayEnd, FCollisionQueryParams(SCENE_QUERY_STAT(Paint), true));

        // Display settings
        const float visualBiasDistance = 0.15f;
        const FLinearColor normalLineColor(0.3f, 1.0f, 0.3f);
        const FLinearColor brushCueColor = FLinearColor(1.0f, 1.0f, 0.3f);

        FVector brushXAxis, brushYAxis;
        traceHitResult.Normal.FindBestAxisVectors(brushXAxis, brushYAxis);
        const FVector brushVisualPosition = traceHitResult.Location + traceHitResult.Normal * visualBiasDistance;

        if (iPDI != NULL)
        {
            int numCircleSides = 128;
            // Draw brush circle
            DrawCircle(iPDI, brushVisualPosition, brushXAxis, brushYAxis, brushCueColor, drawingTool->GetBrushInstance()->GetSizeModifier() / GetStampQuality(), numCircleSides, SDPG_World, 0.1f);

            const FVector normalLineEnd(brushVisualPosition + traceHitResult.Normal * drawingTool->GetBrushInstance()->GetSizeModifier() / GetStampQuality() * 0.2f);
            iPDI->DrawLine(brushVisualPosition, normalLineEnd, normalLineColor, SDPG_World, 0.1f);
        }
    }
}

void FOdysseyViewportDrawingEditorMeshBasedAdapter::BuildPaintingTexture2DRenderTarget()
{
    if (mEditor->Component() == nullptr || mEditor->Texture() == nullptr)
        return;

    const int32 textureWidth = mEditor->Texture()->Source.GetSizeX();
    const int32 textureHeight = mEditor->Texture()->Source.GetSizeY();

    mPaintingTexture2DRenderTarget = NewObject<UTextureRenderTarget2D>(GetTransientPackage(), NAME_None, RF_Transient);
    mPaintingTexture2DRenderTarget->ClearColor = FLinearColor(0, 0, 0, 0);
    mPaintingTexture2DRenderTarget->bNeedsTwoCopies = false;
    mPaintingTexture2DRenderTarget->InitCustomFormat(textureWidth, textureHeight, mEditor->Texture()->GetPixelFormat(), false);
    mPaintingTexture2DRenderTarget->UpdateResourceImmediate();
    mPaintingTexture2DRenderTarget->AddToRoot();


    mStrokeBufferRenderTarget2D = NewObject<UTextureRenderTarget2D>(GetTransientPackage(), NAME_None, RF_Transient);
    mStrokeBufferRenderTarget2D->ClearColor = FLinearColor(0, 0, 0, 0);
    mStrokeBufferRenderTarget2D->bNeedsTwoCopies = false;
    mStrokeBufferRenderTarget2D->InitCustomFormat(textureWidth, textureHeight, mEditor->Texture()->GetPixelFormat(), false);
    mStrokeBufferRenderTarget2D->UpdateResourceImmediate();
    mStrokeBufferRenderTarget2D->AddToRoot();

    mSeamRenderTarget2D = NewObject<UTextureRenderTarget2D>(GetTransientPackage(), NAME_None, RF_Transient);
    mSeamRenderTarget2D->ClearColor = FLinearColor(0, 0, 0, 0);
    mSeamRenderTarget2D->bNeedsTwoCopies = false;
    mSeamRenderTarget2D->InitCustomFormat(textureWidth, textureHeight, mEditor->Texture()->GetPixelFormat(), false);
    mSeamRenderTarget2D->UpdateResourceImmediate();
    mSeamRenderTarget2D->AddToRoot();


    const ERHIFeatureLevel::Type FeatureLevel = mEditor->Component()->GetWorld()->FeatureLevel;
    mEditor->Material()->OverrideTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, FeatureLevel);

    TexturePaintHelpers::CopyTextureToRenderTargetTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, GEditor->GetEditorWorldContext().World()->FeatureLevel);
}

void FOdysseyViewportDrawingEditorMeshBasedAdapter::GatherTextureTriangles(IMeshPaintGeometryAdapter* iAdapter, int32 iTriangleIndex, const int32 iVertexIndices[3], TArray<FTexturePaintTriangleInfo>* iTriangleInfo, TArray<FTexturePaintMeshSectionInfo>* iSectionInfos, int32 iUVChannelIndex)
{
    /** Retrieve triangles eligible for texture painting */
    bool bAdd = iSectionInfos->Num() == 0;
    for (const FTexturePaintMeshSectionInfo& sectionInfo : *iSectionInfos)
    {
        if (iTriangleIndex >= sectionInfo.FirstIndex && iTriangleIndex < sectionInfo.LastIndex)
        {
            bAdd = true;
            break;
        }
    }

    if (bAdd)
    {
        FTexturePaintTriangleInfo info;
        iAdapter->GetVertexPosition(iVertexIndices[0], info.TriVertices[0]);
        iAdapter->GetVertexPosition(iVertexIndices[1], info.TriVertices[1]);
        iAdapter->GetVertexPosition(iVertexIndices[2], info.TriVertices[2]);
        info.TriVertices[0] = iAdapter->GetComponentToWorldMatrix().TransformPosition(info.TriVertices[0]);
        info.TriVertices[1] = iAdapter->GetComponentToWorldMatrix().TransformPosition(info.TriVertices[1]);
        info.TriVertices[2] = iAdapter->GetComponentToWorldMatrix().TransformPosition(info.TriVertices[2]);
        iAdapter->GetTextureCoordinate(iVertexIndices[0], iUVChannelIndex, info.TriUVs[0]);
        iAdapter->GetTextureCoordinate(iVertexIndices[1], iUVChannelIndex, info.TriUVs[1]);
        iAdapter->GetTextureCoordinate(iVertexIndices[2], iUVChannelIndex, info.TriUVs[2]);
        iTriangleInfo->Add(info);
    }
}

TArray<::ULIS::FRectI> FOdysseyViewportDrawingEditorMeshBasedAdapter::GetMinimalRectanglesForTriangleSet(TArray<FTexturePaintTriangleInfo>& iTriangles, int iMaxWidth, int iMaxHeight)
{
    if( iTriangles.Num() == 0 )
        return TArray<::ULIS::FRectI>();

    TArray<::ULIS::FRectI> rects;

    //Get Biggest rectangle
    /*int minX = 100000000;
    int minY = 100000000;
    int maxX = 0;
    int maxY = 0;

    for (int i = 0; i < iTriangles.Num(); i++)
    {
        minX = ::ULIS::FMath::Min4(double(minX), iTriangles[i].TrianglePoints[0].X, iTriangles[i].TrianglePoints[1].X, iTriangles[i].TrianglePoints[2].X);
        minY = ::ULIS::FMath::Min4(double(minY), iTriangles[i].TrianglePoints[0].Y, iTriangles[i].TrianglePoints[1].Y, iTriangles[i].TrianglePoints[2].Y);
        maxX = ::ULIS::FMath::Max4(double(maxX), iTriangles[i].TrianglePoints[0].X, iTriangles[i].TrianglePoints[1].X, iTriangles[i].TrianglePoints[2].X);
        maxY = ::ULIS::FMath::Max4(double(maxY), iTriangles[i].TrianglePoints[0].Y, iTriangles[i].TrianglePoints[1].Y, iTriangles[i].TrianglePoints[2].Y);
    }
    rects.Add( ::ULIS::FRectI::FromMinMax(minX, minY, maxX, maxY) );
    return rects;*/

    //First step, we get all bounding rectangles from the triangles
    for (int i = 0; i < iTriangles.Num(); i++)
    {
        rects.Add(::ULIS::FRectI::FromMinMax( FMath::Max( FMath::Min3(iTriangles[i].TrianglePoints[0].X, iTriangles[i].TrianglePoints[1].X, iTriangles[i].TrianglePoints[2].X) - 1, 0 ),
                                              FMath::Max( FMath::Min3(iTriangles[i].TrianglePoints[0].Y, iTriangles[i].TrianglePoints[1].Y, iTriangles[i].TrianglePoints[2].Y) - 1, 0 ),
                                              FMath::Min( FMath::Max3(iTriangles[i].TrianglePoints[0].X, iTriangles[i].TrianglePoints[1].X, iTriangles[i].TrianglePoints[2].X) + 1, iMaxWidth ),
                                              FMath::Min( FMath::Max3(iTriangles[i].TrianglePoints[0].Y, iTriangles[i].TrianglePoints[1].Y, iTriangles[i].TrianglePoints[2].Y) + 1, iMaxHeight ) ));
    }
    
    TArray< ::ULIS::FRectI > finalRects;
    ::ULIS::TArray<::ULIS::FRectI> rectsToExclude;
    if( rects.Num() != 0 )
    {
        for (int i = 0; i < rects.Num()-1; i++)
        {
            rectsToExclude.PushBack(rects[i]);

            ::ULIS::TArray<::ULIS::FRectI> exclusionRectsTemp;
            for (int k = 0; k < rectsToExclude.Size(); k++)
            {
                rectsToExclude[k].Exclusion(rects[i+1], &exclusionRectsTemp);

                if ( exclusionRectsTemp[0] != rectsToExclude[k] )
                {
                    rectsToExclude.Erase(k);
                    int sizeOfRectsNotEmpty = 0;

                    for (int l = 0; l < exclusionRectsTemp.Size(); l++)
                    {
                        if( exclusionRectsTemp[l].Area() > 0 )
                        {
                            rectsToExclude.Insert(k + sizeOfRectsNotEmpty, exclusionRectsTemp[l]);
                            sizeOfRectsNotEmpty++;
                        }
                    }
                    k = k + sizeOfRectsNotEmpty - 1;
                }
            }
        }
        rectsToExclude.PushBack(rects.Last());
    }

        
    for (int j = 0; j < rectsToExclude.Size(); j++)
    {
        if ( rectsToExclude[j].Area() > 0 )
        {
            finalRects.Add(rectsToExclude[j]);
        }
    }

    
    return finalRects;
}

float FOdysseyViewportDrawingEditorMeshBasedAdapter::GetStampQuality()
{
    if (mEditor->Texture() && mEditor->Component())
    {
        return ((FMath::Max( mEditor->Texture()->GetSizeX(), mEditor->Texture()->GetSizeY() ) / mEditor->GetMeshComponentMaxSize()) + 1);
    }

    return 1.f;
}

::ULIS::FEvent FOdysseyViewportDrawingEditorMeshBasedAdapter::StampOverride(UOdysseyBrushAssetBase::FStampParams iStampParams)
{
    UOdysseyPainterEditorRasterDrawingTool* drawingTool = nullptr;
    if (mEditor->GetSelectedTool()->IsA(UOdysseyPainterEditorRasterDrawingTool::StaticClass()))
        drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetSelectedTool());

    if( !drawingTool )
        return iStampParams.mEvent;

    if (!GCurrentLevelEditingViewportClient->GetEditorViewportWidget()->GetSceneViewport()->HasFocus())
        return drawingTool->GetBrushInstance()->StampInternal(iStampParams);

    const TSharedPtr<IMeshPaintGeometryAdapter>* meshAdapterPtr = mEditor->ComponentToAdapterMap().Find(mEditor->Component());
    if (!meshAdapterPtr)
        return iStampParams.mEvent;

    TSharedPtr<IMeshPaintGeometryAdapter> meshAdapter = *meshAdapterPtr;


    FEditorViewportClient* viewportClient = (FEditorViewportClient*)mLastKnownViewport->GetClient();
    // Compute a world space ray from the screen space mouse coordinates
    FSceneViewFamilyContext viewFamily(FSceneViewFamily::ConstructionValues(
        viewportClient->Viewport,
        viewportClient->GetScene(),
        viewportClient->EngineShowFlags)
        .SetRealtimeUpdate(viewportClient->IsRealtime()));
    FSceneView* view = viewportClient->CalcSceneView(&viewFamily);

    //We need to shift the coordinates of the stamp, because here, we're working in viewport coordinates, not texture coordinates (1 px in viewport != 1px in coord)
    iStampParams.mPosition.x = iStampParams.mPosition.x + iStampParams.mBlock->Width() / 2.f;
    iStampParams.mPosition.y = iStampParams.mPosition.y + iStampParams.mBlock->Height() / 2.f;

    const FViewportCursorLocation mouseViewportRay(view, viewportClient, iStampParams.mPosition.x, iStampParams.mPosition.y);

    FHitResult traceHitResult(1.0f);
    const FVector rayEnd(mouseViewportRay.GetOrigin() + mouseViewportRay.GetDirection() * HALF_WORLD_MAX);

    meshAdapter->LineTraceComponent(traceHitResult, mouseViewportRay.GetOrigin(), rayEnd, FCollisionQueryParams(SCENE_QUERY_STAT(Paint), true));


    FVector brushXAxis, brushYAxis;
    traceHitResult.Normal.FindBestAxisVectors(brushXAxis,brushYAxis);
    const FMatrix worldToBrushMatrix = FMatrix(brushXAxis, brushYAxis, traceHitResult.Normal, traceHitResult.Location).Inverse();


    // Convert trace to UV position
    FVector2D coord;
    if (UGameplayStatics::FindCollisionUV(traceHitResult, mEditor->GetUVIndexUsedByCurrentTexture(), coord))
    {
        //And here we shift back the coordinates, because we converted it to texture coordinates
        iStampParams.mPosition.x = coord.X * mEditor->Texture()->GetSurfaceWidth() - iStampParams.mBlock->Width() / 2.f;
        iStampParams.mPosition.y = coord.Y * mEditor->Texture()->GetSurfaceHeight() - iStampParams.mBlock->Height() / 2.f;

        mStrokeBufferTexture2D = NewUncompressedTextureFromBlockData(iStampParams.mBlock);
    }

    TRefCountPtr< FOdysseyMeshPaintBatchedElementParameters > meshPaintBatchedElementParameters(new FOdysseyMeshPaintBatchedElementParameters());
    {
        meshPaintBatchedElementParameters->ShaderParams.Stroke2D = mStrokeBufferTexture2D;
        meshPaintBatchedElementParameters->ShaderParams.WorldToBrushMatrix = worldToBrushMatrix;
        meshPaintBatchedElementParameters->ShaderParams.TextureHitPoint = FVector2D( iStampParams.mPosition.x, iStampParams.mPosition.y );
        meshPaintBatchedElementParameters->ShaderParams.StampQuality = GetStampQuality();
    }

    const ERHIFeatureLevel::Type featureLevel = mEditor->Component()->GetWorld()->FeatureLevel;

    FTextureRenderTargetResource* strokeRenderTargetResource = mStrokeBufferRenderTarget2D->GameThread_GetRenderTargetResource();
    FCanvas strokePaintCanvas(strokeRenderTargetResource, nullptr, 0, 0, 0, featureLevel);
    FHitProxyId strokePaintHitProxyId = strokePaintCanvas.GetHitProxyId();
    FBatchedElements* strokePaintBatchedElements = strokePaintCanvas.GetBatchedElements(FCanvas::ET_Triangle, meshPaintBatchedElementParameters, nullptr, SE_BLEND_Opaque);

    //Todo: make ellipseIntersectTriangles ?
    TArray<uint32> triangles;
    float brushSize = FMath::Max(iStampParams.mBlock->Width(), iStampParams.mBlock->Height()); //ToCheck after optimization for non scaled objects FMath::Min3(mEditor->Actor()->GetActorScale().X, mEditor->Actor()->GetActorScale().Y, mEditor->Actor()->GetActorScale().Z);
    brushSize *= brushSize;
    brushSize /= GetStampQuality();
    triangles = meshAdapter->SphereIntersectTriangles(brushSize, meshAdapter->GetComponentToWorldMatrix().InverseTransformPosition(traceHitResult.Location), mouseViewportRay.GetOrigin(), false);

    const TArray<uint32> vertexIndices = meshAdapter->GetMeshIndices();
    uint32 triIndices = vertexIndices.Num() / 3;
    TArray<FTexturePaintTriangleInfo> triangleInfo;
    TArray<FTexturePaintMeshSectionInfo> sectionInfo;
    for (int i = 0; i < triangles.Num(); i++)
    {
        const int32 indices[3] = { int32(vertexIndices[triangles[i] * 3]), int32(vertexIndices[triangles[i] * 3 + 1]), int32(vertexIndices[triangles[i] * 3 + 2]) };
        GatherTextureTriangles(meshAdapter.Get(), triangles[i], indices, &triangleInfo, &sectionInfo, mEditor->GetUVIndexUsedByCurrentTexture());
    }

    // Process the influenced triangles - storing off a large list is much slower than processing in a single loop
    for (int32 CurIndex = 0; CurIndex < triangleInfo.Num(); ++CurIndex)
    {
        FTexturePaintTriangleInfo& CurTriangle = triangleInfo[CurIndex];
        FTexturePaintTriangleInfo CurTriangleCopy = triangleInfo[CurIndex];

        for (int32 TriVertexNum = 0; TriVertexNum < 3; ++TriVertexNum)
        {
            CurTriangle.TrianglePoints[TriVertexNum].X = CurTriangleCopy.TriUVs[TriVertexNum].X * mStrokeBufferRenderTarget2D->GetSurfaceWidth();
            CurTriangle.TrianglePoints[TriVertexNum].Y = CurTriangleCopy.TriUVs[TriVertexNum].Y * mStrokeBufferRenderTarget2D->GetSurfaceHeight();
        }

        // Vertex positions
        FVector4 Vert0(CurTriangle.TrianglePoints[0].X, CurTriangle.TrianglePoints[0].Y, 0, 1);
        FVector4 Vert1(CurTriangle.TrianglePoints[1].X, CurTriangle.TrianglePoints[1].Y, 0, 1);
        FVector4 Vert2(CurTriangle.TrianglePoints[2].X, CurTriangle.TrianglePoints[2].Y, 0, 1);

        // Vertex color
        FLinearColor Col0(CurTriangle.TriVertices[0].X, CurTriangle.TriVertices[0].Y, CurTriangle.TriVertices[0].Z);
        FLinearColor Col1(CurTriangle.TriVertices[1].X, CurTriangle.TriVertices[1].Y, CurTriangle.TriVertices[1].Z);
        FLinearColor Col2(CurTriangle.TriVertices[2].X, CurTriangle.TriVertices[2].Y, CurTriangle.TriVertices[2].Z);

        // Brush Paint triangle
        {
            int32 V0 = strokePaintBatchedElements->AddVertex(Vert0, CurTriangle.TriUVs[0], Col0, strokePaintHitProxyId);
            int32 V1 = strokePaintBatchedElements->AddVertex(Vert1, CurTriangle.TriUVs[1], Col1, strokePaintHitProxyId);
            int32 V2 = strokePaintBatchedElements->AddVertex(Vert2, CurTriangle.TriUVs[2], Col2, strokePaintHitProxyId);

            strokePaintBatchedElements->AddTriangle(V0, V1, V2, meshPaintBatchedElementParameters, SE_BLEND_Opaque);
        }
    }

    // Tell the rendering thread to draw any remaining batched elements
    {
        strokePaintCanvas.Flush_GameThread(true);
    }

    {
        ENQUEUE_RENDER_COMMAND(UpdateOdysseyMeshPaintRTCommandSeams)(
            [this, strokeRenderTargetResource](FRHICommandListImmediate& RHICmdList)
            {
                // Copy (resolve) the rendered image from the frame buffer to its render target texture
                RHICmdList.CopyToResolveTarget(
                    strokeRenderTargetResource->GetRenderTargetTexture(),		// Source texture
                    strokeRenderTargetResource->TextureRHI,
                    FResolveParams());									// Resolve parameters
            });
    }

    //Seams painting ---------------------------------------------------------------
    const float MinU = 0.0f;
    const float MinV = 0.0f;
    const float MaxU = 1.0f;
    const float MaxV = 1.0f;
    const float MinX = 0.0f;
    const float MinY = 0.0f;
    const float MaxX = mStrokeBufferRenderTarget2D->GetSurfaceWidth();
    const float MaxY = mStrokeBufferRenderTarget2D->GetSurfaceHeight();

    // Create a canvas for the render target.

    TRefCountPtr< FOdysseySeamsPaintBatchedElementParameters > seamsPaintBatchedElementParameters(new FOdysseySeamsPaintBatchedElementParameters());
    {
        seamsPaintBatchedElementParameters->ShaderParams.Stroke2D = mStrokeBufferRenderTarget2D;
        seamsPaintBatchedElementParameters->ShaderParams.SeamMaskRenderTarget = mSeamRenderTarget2D;
        seamsPaintBatchedElementParameters->ShaderParams.WidthPixelOffset = (float)(1.0f / mStrokeBufferRenderTarget2D->GetSurfaceWidth());
        seamsPaintBatchedElementParameters->ShaderParams.HeightPixelOffset = (float)(1.0f / mStrokeBufferRenderTarget2D->GetSurfaceHeight());
    }

    // Draw a quad to copy the texture over to the render target
    TArray< FCanvasUVTri >	TriangleList;
    FCanvasUVTri SingleTri;
    SingleTri.V0_Pos = FVector2D(MinX, MinY);
    SingleTri.V0_UV = FVector2D(MinU, MinV);
    SingleTri.V0_Color = FLinearColor::White;

    SingleTri.V1_Pos = FVector2D(MaxX, MinY);
    SingleTri.V1_UV = FVector2D(MaxU, MinV);
    SingleTri.V1_Color = FLinearColor::White;

    SingleTri.V2_Pos = FVector2D(MaxX, MaxY);
    SingleTri.V2_UV = FVector2D(MaxU, MaxV);
    SingleTri.V2_Color = FLinearColor::White;
    TriangleList.Add(SingleTri);

    SingleTri.V0_Pos = FVector2D(MaxX, MaxY);
    SingleTri.V0_UV = FVector2D(MaxU, MaxV);
    SingleTri.V0_Color = FLinearColor::White;

    SingleTri.V1_Pos = FVector2D(MinX, MaxY);
    SingleTri.V1_UV = FVector2D(MinU, MaxV);
    SingleTri.V1_Color = FLinearColor::White;

    SingleTri.V2_Pos = FVector2D(MinX, MinY);
    SingleTri.V2_UV = FVector2D(MinU, MinV);
    SingleTri.V2_Color = FLinearColor::White;
    TriangleList.Add(SingleTri);

    FCanvasTriangleItem TriItemList(TriangleList, nullptr);
    TriItemList.BatchedElementParameters = seamsPaintBatchedElementParameters;
    TriItemList.BlendMode = SE_BLEND_Opaque;
    strokePaintCanvas.DrawItem(TriItemList);
    {
        strokePaintCanvas.Flush_GameThread(true);
    }


    TArray<::ULIS::FRectI> rects;
    rects = GetMinimalRectanglesForTriangleSet(triangleInfo, mStrokeBufferRenderTarget2D->GetSurfaceWidth(), mStrokeBufferRenderTarget2D->GetSurfaceHeight());

    mPixelFence.BeginFence();
    mPixelFence.Wait();


    for (int i = 0; i < rects.Num(); i++)
    {
        ENQUEUE_RENDER_COMMAND(UpdateOdysseyMeshPaintRTCommand)(
            [this, strokeRenderTargetResource, rects, i](FRHICommandListImmediate& RHICmdList)
            {
                // Copy (resolve) the rendered image from the frame buffer to its render target texture*/
                /*RHICmdList.CopyToResolveTarget(
                    brushRenderTargetResource->GetRenderTargetTexture(),		// Source texture
                    brushRenderTargetResource->TextureRHI,
                    FResolveParams());									// Resolve parameters

                /*TArray<FRHIGPUTextureReadback> readbufferArray;

                for (int i = 0; i < rects.Num(); i++)
                {
                    mColorDataPtr.Add(nullptr);
                    readbufferArray.Add(FRHIGPUTextureReadback(TEXT("ReadMeshPaintTextureIliad" + i)));

                    //for (int y = rects[i].y; y < rects[i].y + rects[i].h; y++)
                    //{
                    //    readbufferArray[i].EnqueueCopy(RHICmdList, brushRenderTargetResource->GetRenderTargetTexture(), FResolveRect(rects[i].x, rects[i].y + y, rects[i].x + rects[i].w, 1));
                    //}

                    readbufferArray[i].EnqueueCopy(RHICmdList, brushRenderTargetResource->GetRenderTargetTexture(), FResolveRect(rects[i].x, rects[i].y, rects[i].x + rects[i].w, rects[i].y + rects[i].h));

                }

                RHICmdList.BlockUntilGPUIdle();

                for (int i = 0; i < rects.Num(); i++)
                {
                    mColorDataPtr[i] = (FColor*)readbufferArray[i].Lock(4 * rects[i].w * rects[i].h);
                    readbufferArray[i].Unlock();
                }*/

                if( mColorData.Num() < rects.Num() )
                    mColorData.Add(TArray<FLinearColor>());

                RHICmdList.ReadSurfaceData(strokeRenderTargetResource->GetRenderTargetTexture(), FIntRect(rects[i].x, rects[i].y, rects[i].x + rects[i].w, rects[i].y + rects[i].h), mColorData[i], FReadSurfaceDataFlags());
            });
    }

    mPixelFence.BeginFence();
    mPixelFence.Wait();

    //Resets the color of all pixels in mStrokeBufferRenderTarget2D for next stamp. Is there a better way to do it ?
    mStrokeBufferRenderTarget2D->UpdateResource();

    TArray<::ULIS::FEvent> eventBlend;
    ::ULIS::eFormat target_format = ::ULIS::eFormat::Format_RGBAF;
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(target_format);


    for (int i = 0; i < rects.Num(); i++)
    {
        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> blockToStamp = MakeShared<::ULIS::FBlock>(static_cast<uint8*>(static_cast<void*>(mColorData[i].GetData())), rects[i].w, rects[i].h, ::ULIS::eFormat::Format_RGBAF);

        ULIS::FVec2F position = rects[i].Position();
        rects[i].x = 0;
        rects[i].y = 0;

        //Convert the destination if needed and Blend the folderBlock
        TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(drawingTool->GetBrushInstance()->GetBlock(), blockToStamp->Format(), rects[i], position, { iStampParams.mEvent },
            [this, &blockToStamp, &ctx, iStampParams](TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
            {
                ::ULIS::FEvent eventBlend = FULISEventBuilder().RetainBlock(blockToStamp).Build();

                if (iStampParams.mAntiAliasing)
                {
                    ctx.BlendAA(
                        *blockToStamp
                        , *ioDest
                        , iRect
                        , iPos
                        , ::ULIS::eBlendMode(iStampParams.mBlendingMode)
                        , ::ULIS::eAlphaMode(iStampParams.mAlphaMode)
                        , FMath::Clamp(iStampParams.mFlow, 0.f, 1.f)
                        , ::ULIS::FSchedulePolicy::AsyncCacheEfficient
                        , iWaitList.Num()
                        , iWaitList.GetData()
                        , &eventBlend
                    );
                }
                else
                {
                    ctx.Blend(
                        *blockToStamp,
                        *ioDest,
                        iRect,
                        iPos,
                        ::ULIS::eBlendMode(iStampParams.mBlendingMode),
                        ::ULIS::eAlphaMode(iStampParams.mAlphaMode),
                        FMath::Clamp(iStampParams.mFlow, 0.f, 1.f),
                        ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                        iWaitList.Num(),
                        iWaitList.GetData(),
                        &eventBlend
                    );
                }

                return { eventBlend };
            }
        );
        eventBlend.Append(eventConvertAndExecute);

        ctx.Flush();

        ::ULIS::FRectI invalidRectI(FMath::FloorToInt(position.x), FMath::FloorToInt(position.y), FMath::CeilToInt(rects[i].w + 2.f), FMath::CeilToInt(rects[i].h + 2.f));
        invalidRectI = invalidRectI & drawingTool->GetBrushInstance()->GetBlock()->Rect();
        if (invalidRectI.w > 0 && invalidRectI.h > 0)
            drawingTool->GetBrushInstance()->GetInvalidRects()->Add(invalidRectI);
    }

    ::ULIS::FEvent eventStampInternal;
    ctx.Dummy_OP(eventBlend.Num(), eventBlend.GetData(), &eventStampInternal);

    return eventStampInternal;
}

void FOdysseyViewportDrawingEditorMeshBasedAdapter::BindStampBrushInstance(UOdysseyBrushAssetBase* iBindBrush)
{
    IOdysseyViewportDrawingEditorAdapter::BindStampBrushInstance( iBindBrush );
    if (iBindBrush)
    {
        iBindBrush->AddContext( new FOdysseyViewportEditorBrushContext() );
    }
}

#undef LOCTEXT_NAMESPACE
