// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyViewportDrawingEditorTextureBasedAdapter.h"
#include "MeshPaintHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "OdysseyViewportDrawingEditorExtension.h"
#include "OdysseyPainterEditor.h"
#include "TextureResource.h"
#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"

FOdysseyViewportDrawingEditorTextureBasedAdapter::~FOdysseyViewportDrawingEditorTextureBasedAdapter()
{
}

FOdysseyViewportDrawingEditorTextureBasedAdapter::FOdysseyViewportDrawingEditorTextureBasedAdapter(FOdysseyViewportDrawingEditorExtension* iExtension) :
    IOdysseyViewportDrawingEditorAdapter(iExtension),
    mLastPoint( FVector2D (-1,-1))
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
    IOdysseyViewportDrawingEditorAdapter::FinishPainting();
    mStopDrawing = false;
    mLastPoint = FVector2D( -1, -1 );
}

void FOdysseyViewportDrawingEditorTextureBasedAdapter::RenderInteractorWidget(const FSceneView* iView, FViewport* iViewport, FPrimitiveDrawInterface* iPDI)
{
}

::ULIS::FEvent FOdysseyViewportDrawingEditorTextureBasedAdapter::StampOverride(UOdysseyBrushAssetBase::FStampParams iStampParams)
{
    UOdysseyPainterEditorRasterDrawingTool* drawingTool = nullptr;
    if (mExtension->GetEditor()->GetCurrentTool()->IsA(UOdysseyPainterEditorRasterDrawingTool::StaticClass()))
        drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mExtension->GetEditor()->GetCurrentTool());

    if (mStopDrawing == true || !drawingTool)
        return iStampParams.mEvent;

    if (mLastPoint == FVector2D(-1, -1))
    {
        mLastPoint = FVector2D( mCurrentStrokeRay.mPoint.x, mCurrentStrokeRay.mPoint.y);
    }

    float thresholdX = mTexture->GetResource()->GetSizeX() / 5.f; //20% of texture Size
    float thresholdY = mTexture->GetResource()->GetSizeY() / 5.f; //20% of texture Size

    if ((::FMath::Abs( float(mLastPoint.X) - mCurrentStrokeRay.mPoint.x ) > thresholdX || ::FMath::Abs(float(mLastPoint.Y) - mCurrentStrokeRay.mPoint.y) > thresholdY))
    {
        mStopDrawing = true;
        return iStampParams.mEvent;
    }

    mLastPoint = FVector2D(mCurrentStrokeRay.mPoint.x, mCurrentStrokeRay.mPoint.y);

    return drawingTool->GetBrushInstance()->StampInternal(iStampParams);
}
