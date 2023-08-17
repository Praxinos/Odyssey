// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022
// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyViewportDrawingEditorTextureBasedAdapter.h"
#include "MeshPaintHelpers.h"

#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorTextureBasedAdapter"

FOdysseyViewportDrawingEditorTextureBasedAdapter::~FOdysseyViewportDrawingEditorTextureBasedAdapter()
{
}

FOdysseyViewportDrawingEditorTextureBasedAdapter::FOdysseyViewportDrawingEditorTextureBasedAdapter(FOdysseyViewportDrawingEditorExtension* iExtension) :
    IOdysseyViewportDrawingEditorAdapter(iExtension)
{
}

void
FOdysseyViewportDrawingEditorTextureBasedAdapter::Initialize()
{
    IOdysseyViewportDrawingEditorAdapter::Initialize();
    mExtension->GetEditor()->GetRasterDrawingTool()->SetBaseSize(0);
}

void FOdysseyViewportDrawingEditorTextureBasedAdapter::FinishPainting()
{
    mStopDrawing = false;
    IOdysseyViewportDrawingEditorAdapter::FinishPainting();
}

void FOdysseyViewportDrawingEditorTextureBasedAdapter::RenderInteractorWidget(const FSceneView* iView, FViewport* iViewport, FPrimitiveDrawInterface* iPDI)
{
}

::ULIS::FEvent FOdysseyViewportDrawingEditorTextureBasedAdapter::StampOverride(UOdysseyBrushAssetBase::FStampParams iStampParams)
{
    UOdysseyPainterEditorRasterDrawingTool* drawingTool = nullptr;
    if (mExtension->GetEditor()->GetSelectedTool()->IsA(UOdysseyPainterEditorRasterDrawingTool::StaticClass()))
        drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mExtension->GetEditor()->GetSelectedTool());

    const TSharedPtr<IMeshPaintGeometryAdapter>* meshAdapterPtr = mEditor->ComponentToAdapterMap().Find(mEditor->Component());

    TSharedPtr<IMeshPaintGeometryAdapter> meshAdapter = *meshAdapterPtr;

    FHitResult lastTraceHitResult(1.0f);
    const FVector lastRayEnd(mLastStrokeRay.mRayOrigin + mLastStrokeRay.mRayDirection * HALF_WORLD_MAX);

    FHitResult currentTraceHitResult(1.0f);
    const FVector currentRayEnd(mCurrentStrokeRay.mRayOrigin + mCurrentStrokeRay.mRayDirection * HALF_WORLD_MAX);

    meshAdapter->LineTraceComponent(lastTraceHitResult, mLastStrokeRay.mRayOrigin, lastRayEnd, FCollisionQueryParams(SCENE_QUERY_STAT(Paint), true));
    meshAdapter->LineTraceComponent(currentTraceHitResult, mCurrentStrokeRay.mRayOrigin, currentRayEnd, FCollisionQueryParams(SCENE_QUERY_STAT(Paint), true));

    // Convert trace to UV position
    FVector2D lastCoord;
    FVector2D currentCoord;
    UGameplayStatics::FindCollisionUV(lastTraceHitResult, mEditor->GetUVIndexUsedByCurrentTexture(), lastCoord);
    UGameplayStatics::FindCollisionUV(currentTraceHitResult, mEditor->GetUVIndexUsedByCurrentTexture(), currentCoord);

    if (::ULIS::FMath::Dist(lastCoord.X, lastCoord.Y, currentCoord.X, currentCoord.Y) > 0.1f)
    {
        mStopDrawing = true;
        return iStampParams.mEvent;
    }

    if(drawingTool && mStopDrawing == false)
        return drawingTool->GetBrushInstance()->StampInternal(iStampParams);
    else
        return iStampParams.mEvent;

}

#undef LOCTEXT_NAMESPACE
