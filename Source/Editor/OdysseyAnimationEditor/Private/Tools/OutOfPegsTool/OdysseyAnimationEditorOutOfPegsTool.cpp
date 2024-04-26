// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/OutOfPegsTool/OdysseyAnimationEditorOutOfPegsTool.h"

#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyHUDElement.h"
#include "OdysseyHUDPolygon.h"
#include "OdysseyHUDHandle.h"
#include "OdysseyHUDCircle.h"
#include "OdysseyHUDLine.h"

#include <ULIS>

UOdysseyAnimationEditorOutOfPegsTool::~UOdysseyAnimationEditorOutOfPegsTool()
{

}

UOdysseyAnimationEditorOutOfPegsTool::UOdysseyAnimationEditorOutOfPegsTool()
{

}

void
UOdysseyAnimationEditorOutOfPegsTool::Load()
{
    UOdysseyPainterEditorTool::Load();

    RebuildHUD();
}

bool
UOdysseyAnimationEditorOutOfPegsTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    FOdysseyPoint point = iPointInTexture;
    point.x = FMath::RoundToInt( point.x );
    point.y = FMath::RoundToInt( point.y );

    if (mHUD->OnKeyDown(point, iKey)) //Handling HUD events if needed
    {
        FVector2D center = GetCenter();
        if (mTransformTopLeftHandleHUD->IsCaptured())
        {
            mZoomReference = mCell->OutOfPegsZoom();
            mZoomCenter = center;
            mZoomDistanceReference = FVector2D::Distance( center, mTransformTopLeftHandleHUD->GetPosition() );
        }

        if (mTransformTopRightHandleHUD->IsCaptured())
        {
            mZoomReference = mCell->OutOfPegsZoom();
            mZoomCenter = center;
            mZoomDistanceReference = FVector2D::Distance( center, mTransformTopRightHandleHUD->GetPosition() );
        }

        if (mTransformBottomRightHandleHUD->IsCaptured())
        {
            mZoomReference = mCell->OutOfPegsZoom();
            mZoomCenter = center;
            mZoomDistanceReference = FVector2D::Distance( center, mTransformBottomRightHandleHUD->GetPosition() );
        }

        if (mTransformBottomLeftHandleHUD->IsCaptured())
        {
            mZoomReference = mCell->OutOfPegsZoom();
            mZoomCenter = center;
            mZoomDistanceReference = FVector2D::Distance( center, mTransformBottomLeftHandleHUD->GetPosition() );
        }

        if (mTransformRotationHandleHUD->IsCaptured())
        {
            mRotationCenter = center;
        }
        return true;
    }
    else
    {
        mIsPanning = true;
        mPanPointReference = FVector2D(point.x, point.y);
        mPanReference = Pan;
    }

    return true;
}

bool
UOdysseyAnimationEditorOutOfPegsTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    FOdysseyPoint point = iPointInTexture;
    point.x = FMath::RoundToInt(point.x);
    point.y = FMath::RoundToInt(point.y);

    if (mIsPanning)
    {
        FOdysseyObjectEditorUtils::SetPropertyValue(this, "Pan", mCell->OutOfPegsPan(), EPropertyChangeType::ValueSet);
        mIsPanning = false;
    }

    if (mTransformTopLeftHandleHUD->IsCaptured())
        FOdysseyObjectEditorUtils::SetPropertyValue(this, "Zoom", mCell->OutOfPegsZoom() * 100.f, EPropertyChangeType::ValueSet);

    if (mTransformTopRightHandleHUD->IsCaptured())
        FOdysseyObjectEditorUtils::SetPropertyValue(this, "Zoom", mCell->OutOfPegsZoom() * 100.f, EPropertyChangeType::ValueSet);

    if (mTransformBottomRightHandleHUD->IsCaptured())
        FOdysseyObjectEditorUtils::SetPropertyValue(this, "Zoom", mCell->OutOfPegsZoom() * 100.f, EPropertyChangeType::ValueSet);

    if (mTransformBottomLeftHandleHUD->IsCaptured())
        FOdysseyObjectEditorUtils::SetPropertyValue(this, "Zoom", mCell->OutOfPegsZoom() * 100.f, EPropertyChangeType::ValueSet);

    if (mTransformRotationHandleHUD->IsCaptured())
        FOdysseyObjectEditorUtils::SetPropertyValue(this, "Rotation", mCell->OutOfPegsRotation(), EPropertyChangeType::ValueSet);

    if( mHUD->OnKeyUp(point, iKey) )
        return true;

    return true;
}

void
UOdysseyAnimationEditorOutOfPegsTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    mHUD->MouseMove(iPointInTexture);
}

void
UOdysseyAnimationEditorOutOfPegsTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    FOdysseyPoint point = iPointInTexture;
    point.x = FMath::RoundToInt(point.x);
    point.y = FMath::RoundToInt(point.y);

    if (mIsPanning)
    {
        FVector2D pan = mPanReference + FVector2D(point.x, point.y) - mPanPointReference;
        FOdysseyObjectEditorUtils::SetPropertyValue(this, "Pan", pan, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
    }

    mHUD->CapturedMouseMove(point);

    if (mTransformTopLeftHandleHUD->IsCaptured())
    {
        float distance = FVector2D::Distance( mZoomCenter, mTransformTopLeftHandleHUD->GetPosition() );
        float zoomRatio = distance / mZoomDistanceReference;
        float zoom = mZoomReference * zoomRatio;
        FOdysseyObjectEditorUtils::SetPropertyValue(this, "Zoom", zoom * 100.f, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
    }

    if (mTransformTopRightHandleHUD->IsCaptured())
    {
        float distance = FVector2D::Distance( mZoomCenter, mTransformTopRightHandleHUD->GetPosition() );
        float zoomRatio = distance / mZoomDistanceReference;
        float zoom = mZoomReference * zoomRatio;
        FOdysseyObjectEditorUtils::SetPropertyValue(this, "Zoom", zoom * 100.f, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
    }

    if (mTransformBottomRightHandleHUD->IsCaptured())
    {
        float distance = FVector2D::Distance( mZoomCenter, mTransformBottomRightHandleHUD->GetPosition() );
        float zoomRatio = distance / mZoomDistanceReference;
        float zoom = mZoomReference * zoomRatio;
        FOdysseyObjectEditorUtils::SetPropertyValue(this, "Zoom", zoom * 100.f, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
    }

    if (mTransformBottomLeftHandleHUD->IsCaptured())
    {
        float distance = FVector2D::Distance( mZoomCenter, mTransformBottomLeftHandleHUD->GetPosition() );
        float zoomRatio = distance / mZoomDistanceReference;
        float zoom = mZoomReference * zoomRatio;
        FOdysseyObjectEditorUtils::SetPropertyValue(this, "Zoom", zoom * 100.f, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
    }

    if (mTransformRotationHandleHUD->IsCaptured())
    {
        FVector2D vector = mTransformRotationHandleHUD->GetPosition() - mRotationCenter;
        float rotation = FMath::RadiansToDegrees(FMath::Atan2(vector.Y, vector.X));
        FOdysseyObjectEditorUtils::SetPropertyValue(this, "Rotation", rotation, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
    }
}

EMouseCursor::Type
UOdysseyAnimationEditorOutOfPegsTool::GetMouseCursor() const
{
    return UOdysseyPainterEditorTool::GetMouseCursor();
}

TSharedRef<SWidget>
UOdysseyAnimationEditorOutOfPegsTool::CreateTopTabWidget()
{
    return SNullWidget::NullWidget;
}

void
UOdysseyAnimationEditorOutOfPegsTool::PostEditChangeProperty( FPropertyChangedEvent& iEvent)
{
    Super::PostEditChangeProperty(iEvent);
    
    FName propertyName = iEvent.GetPropertyName();
    FName memberPropertyName = iEvent.MemberProperty->GetFName();
    
    if (   memberPropertyName == TEXT("Pan")
        || propertyName == TEXT("Rotation")
        || propertyName == TEXT("Zoom") )
    {
        mCell->SetOutOfPegs(Pan, Rotation, Zoom / 100.f, EPropertyChangeType::ValueSet | (iEvent.ChangeType & EPropertyChangeType::Interactive));
    }
}

TSharedPtr<FOdysseyAnimationCell>
UOdysseyAnimationEditorOutOfPegsTool::GetCell() const
{
    return mCell;
}

void
UOdysseyAnimationEditorOutOfPegsTool::SetCell(TSharedPtr<FOdysseyAnimationCell> iCell)
{
    if (iCell == mCell)
        return;

    if (mCell)
        mCell->OnOutOfPegsChanged().RemoveAll(this);
    
    if (mLayer)
        mLayer->OnLightTableIsActivatedChanged().RemoveAll(this);

    mCell = iCell;

    if (mCell)
    {
        mLayer = mCell->GetLayer();
        mLayer->OnLightTableIsActivatedChanged().AddUObject(this, &UOdysseyAnimationEditorOutOfPegsTool::OnLightTableIsActivatedChanged);

        FOdysseyObjectEditorUtils::SetPropertyValue(this, "Pan", mCell->OutOfPegsPan(), EPropertyChangeType::ValueSet);
        FOdysseyObjectEditorUtils::SetPropertyValue(this, "Rotation", mCell->OutOfPegsRotation(), EPropertyChangeType::ValueSet);
        FOdysseyObjectEditorUtils::SetPropertyValue(this, "Zoom", mCell->OutOfPegsZoom() * 100.f, EPropertyChangeType::ValueSet);
        mCell->OnOutOfPegsChanged().AddUObject(this, &UOdysseyAnimationEditorOutOfPegsTool::OnCellOutOfPegsChanged);
    }
    
    RebuildHUD();
}

void
UOdysseyAnimationEditorOutOfPegsTool::OnCellOutOfPegsChanged(bool iIsInteractive)
{
    Pan = mCell->OutOfPegsPan();
    Rotation = mCell->OutOfPegsRotation();
    Zoom = mCell->OutOfPegsZoom() * 100.f;

    RefreshHUD();
}

void
UOdysseyAnimationEditorOutOfPegsTool::OnLightTableIsActivatedChanged()
{
    if (!mLayer->GetIsLightTableActivated())
    {
        GetEditor()->InactivateTemporaryTool();
    }
}

void
UOdysseyAnimationEditorOutOfPegsTool::RebuildHUD()
{
    mHUD->EmptyHUDElements();
    mTransformHUD = nullptr;
    mTransformTopLeftHandleHUD = nullptr;
    mTransformTopRightHandleHUD = nullptr;
    mTransformBottomRightHandleHUD = nullptr;
    mTransformBottomLeftHandleHUD = nullptr;
    mTransformRotationHandleHUD = nullptr;
    /* mLeftPegHUD = nullptr;
    mRightPegHUD = nullptr;
    mCenterPegHUD = nullptr; */

    if (!mCell)
        return;

    TArray<FVector2D> defaultPoints;
    defaultPoints.Add(FVector2D(0, 0));
    defaultPoints.Add(FVector2D(0, 0));
    defaultPoints.Add(FVector2D(0, 0));
    defaultPoints.Add(FVector2D(0, 0));

    mTransformHUD = new FOdysseyHUDPolygon(FName("Transform"), defaultPoints);

    TArray<FVector2D>& points = mTransformHUD->GetPoints();
    
    mTransformTopLeftHandleHUD = new FOdysseyHUDHandle("TransformTopLeftHandle", mTransformHUD, &points[0]);
    mTransformTopRightHandleHUD = new FOdysseyHUDHandle("TransformTopRightHandle", mTransformHUD, &points[1]);
    mTransformBottomRightHandleHUD = new FOdysseyHUDHandle("TransformBottomRightHandle", mTransformHUD, &points[2]);
    mTransformBottomLeftHandleHUD = new FOdysseyHUDHandle("TransformBottomLeftHandle", mTransformHUD, &points[3]);
    mTransformRotationHandleHUD = new FOdysseyHUDHandle("TransformRotationHandle", mTransformHUD, &mTransformRotationPoint);
    mTransformRotationLineHUD = new FOdysseyHUDLine("TransformRotationLine", FVector2D(0, 0), FVector2D(0, 0));
    /* mLeftPegHUD = new FOdysseyHUDPolygon(FName("LeftPeg"), defaultPoints);
    mRightPegHUD = new FOdysseyHUDPolygon(FName("RightPeg"), defaultPoints);
    mCenterPegHUD = new FOdysseyHUDCircle(FName("CenterPeg"), FVector2D(0, 0), 0.f); */
    
    mHUD->AddElement(mTransformHUD);
    mHUD->AddElement(mTransformTopLeftHandleHUD);
    mHUD->AddElement(mTransformTopRightHandleHUD);
    mHUD->AddElement(mTransformBottomRightHandleHUD);
    mHUD->AddElement(mTransformBottomLeftHandleHUD);
    mHUD->AddElement(mTransformRotationHandleHUD);
    mHUD->AddElement(mTransformRotationLineHUD);
    /* mHUD->AddElement(mLeftPegHUD);
    mHUD->AddElement(mRightPegHUD);
    mHUD->AddElement(mCenterPegHUD); */

    RefreshHUD();
}

FVector2D
UOdysseyAnimationEditorOutOfPegsTool::GetCenter() const
{
    UOdysseyAnimationLayer* layer = mCell->GetLayer();
    if (!layer)
        return FVector2D();

    UOdysseyAnimation* animation = layer->GetAnimation();
    if (!animation)
        return FVector2D();

    float width = animation->Width();
    float height = animation->Height();

    ::ULIS::FMat3F oopTransform = mCell->OutOfPegsTransform();
    ::ULIS::FVec2F center = oopTransform * ::ULIS::FVec3F(width / 2.f, height / 2.f, 1.f);
    return FVector2D(center.x, center.y);
}

void
UOdysseyAnimationEditorOutOfPegsTool::RefreshHUD()
{
    UOdysseyAnimationLayer* layer = mCell->GetLayer();
    if (!layer)
        return;

    UOdysseyAnimation* animation = layer->GetAnimation();
    if (!animation)
        return;

    float width = animation->Width();
    float height = animation->Height();

    ::ULIS::FMat3F oopTransform = mCell->OutOfPegsTransform();

    //Transform HUD
    if (mTransformHUD)
    {
        TArray<FVector2D>& points = mTransformHUD->GetPoints();

        ::ULIS::FVec2F topLeft = oopTransform * ::ULIS::FVec3F(0.f, 0.f, 1.f);
        ::ULIS::FVec2F topRight = oopTransform * ::ULIS::FVec3F(width, 0.f, 1.f);
        ::ULIS::FVec2F bottomRight = oopTransform * ::ULIS::FVec3F(width, height, 1.f);
        ::ULIS::FVec2F bottomLeft = oopTransform * ::ULIS::FVec3F(0.f, height, 1.f);

        points[0] = FVector2D(topLeft.x, topLeft.y); //top left
        points[1] = FVector2D(topRight.x, topRight.y); //top right
        points[2] = FVector2D(bottomRight.x, bottomRight.y); //bottom right
        points[3] = FVector2D(bottomLeft.x, bottomLeft.y); //bottom left   

        ::ULIS::FVec2F rotationPoint = oopTransform * ::ULIS::FVec3F(width, height / 2.f, 1.f);
        ::ULIS::FVec2F rotationLineStartPoint = oopTransform * ::ULIS::FVec3F(width / 2.f, height / 2.f, 1.f);
        ::ULIS::FVec2F rotationLineFinishPoint = oopTransform * ::ULIS::FVec3F(width, height / 2.f, 1.f);

        mTransformRotationPoint = FVector2D(rotationPoint.x, rotationPoint.y);
        mTransformRotationLineHUD->mStartPoint = FVector2D(rotationLineStartPoint.x, rotationLineStartPoint.y);
        mTransformRotationLineHUD->mFinishPoint = FVector2D(rotationLineFinishPoint.x, rotationLineFinishPoint.y);
    }
        
}