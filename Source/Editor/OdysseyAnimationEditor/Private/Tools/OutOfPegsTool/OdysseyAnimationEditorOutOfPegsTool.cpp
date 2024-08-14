// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/OutOfPegsTool/OdysseyAnimationEditorOutOfPegsTool.h"

#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyHUDElement.h"
#include "OdysseyHUDPolygon.h"
#include "OdysseyHUDHandle.h"
#include "OdysseyHUDCircle.h"
#include "OdysseyHUDLine.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyAnimation.h"

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
    mIsPanning = true;
    mPanPointReference = FVector2D(point.x, point.y);
    mPanReference = Pan;

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
        FOdysseyObjectEditorUtils::SetPropertyValue(this, "Pan", mCell->OutOfPegs.Pan, EPropertyChangeType::ValueSet);
        mIsPanning = false;
    }
    return true;
}

void
UOdysseyAnimationEditorOutOfPegsTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
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
    
    bool isInteractive = EPropertyChangeType::ValueSet | (iEvent.ChangeType & EPropertyChangeType::Interactive);
    if ( memberPropertyName == TEXT("Pan") )
    {
		FOdysseyAnimationCellOutOfPegs outOfPegs = mCell->OutOfPegs;
		outOfPegs.Pan = Pan;
		FOdysseyObjectEditorUtils::SetPropertyValue(mCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, OutOfPegs), outOfPegs, iEvent.ChangeType);
    }
    if ( propertyName == TEXT("Rotation") )
    {
		FOdysseyAnimationCellOutOfPegs outOfPegs = mCell->OutOfPegs;
		outOfPegs.Rotation = Rotation;
		FOdysseyObjectEditorUtils::SetPropertyValue(mCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, OutOfPegs), outOfPegs, iEvent.ChangeType);
    }
    if ( propertyName == TEXT("Zoom") )
    {
		FOdysseyAnimationCellOutOfPegs outOfPegs = mCell->OutOfPegs;
		outOfPegs.Zoom = Zoom;
		FOdysseyObjectEditorUtils::SetPropertyValue(mCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, OutOfPegs), outOfPegs, iEvent.ChangeType);
    }
}

UOdysseyAnimationCell*
UOdysseyAnimationEditorOutOfPegsTool::GetCell() const
{
    return mCell;
}

void
UOdysseyAnimationEditorOutOfPegsTool::SetCell(UOdysseyAnimationCell* iCell)
{
    if (iCell == mCell)
        return;

    if (mCell)
        mCell->OnOutOfPegsChanged().RemoveAll(this);
    
    if (mLayer)
        mLayer->OnLightTableChanged().RemoveAll(this);

    mCell = iCell;

    if (mCell)
    {
        mLayer = mCell->GetLayer();
        mLayer->OnLightTableChanged().AddUObject(this, &UOdysseyAnimationEditorOutOfPegsTool::OnLightTableChanged);

        Pan = mCell->OutOfPegs.Pan;
        Rotation = mCell->OutOfPegs.Rotation;
        Zoom = mCell->OutOfPegs.Zoom;

        mCell->OnOutOfPegsChanged().AddUObject(this, &UOdysseyAnimationEditorOutOfPegsTool::OnCellOutOfPegsChanged);
    }
    
    RebuildHUD();
}

void
UOdysseyAnimationEditorOutOfPegsTool::OnCellOutOfPegsChanged(bool iIsInteractive)
{
    Pan = mCell->OutOfPegs.Pan;
    Rotation = mCell->OutOfPegs.Rotation;
    Zoom = mCell->OutOfPegs.Zoom;

    RefreshHUD();
}

void
UOdysseyAnimationEditorOutOfPegsTool::OnLightTableChanged()
{
    if (!mLayer->Lighttable.bIsActivated)
    {
        GetEditor()->InactivateTemporaryTool();
    }
}

void
UOdysseyAnimationEditorOutOfPegsTool::RebuildHUD()
{
    mHUD->EmptyElements();
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

    mTransformHUD = MakeShared<FOdysseyHUDPolygon>();

    TArray<FVector2D>& points = mTransformHUD->GetPoints();
    points.Add(FVector2D(0, 0));
    points.Add(FVector2D(0, 0));
    points.Add(FVector2D(0, 0));
    points.Add(FVector2D(0, 0));
    
    mTransformTopLeftHandleHUD = MakeShared<FOdysseyHUDHandle>(FVector2D(0, 0));
    mTransformTopRightHandleHUD = MakeShared<FOdysseyHUDHandle>(FVector2D(0, 0));
    mTransformBottomRightHandleHUD = MakeShared<FOdysseyHUDHandle>(FVector2D(0, 0));
    mTransformBottomLeftHandleHUD = MakeShared<FOdysseyHUDHandle>(FVector2D(0, 0));
    mTransformRotationHandleHUD = MakeShared<FOdysseyHUDHandle>(FVector2D(0, 0));
    mTransformRotationLineHUD = MakeShared<FOdysseyHUDLine>(FVector2D(0, 0), FVector2D(0, 0));

    mTransformTopLeftHandleHUD->OnDragBegin().AddUObject(this, &UOdysseyAnimationEditorOutOfPegsTool::OnTopLeftHandleDragBegin);
    mTransformTopRightHandleHUD->OnDragBegin().AddUObject(this, &UOdysseyAnimationEditorOutOfPegsTool::OnTopRightHandleDragBegin);
    mTransformBottomRightHandleHUD->OnDragBegin().AddUObject(this, &UOdysseyAnimationEditorOutOfPegsTool::OnBottomRightHandleDragBegin);
    mTransformBottomLeftHandleHUD->OnDragBegin().AddUObject(this, &UOdysseyAnimationEditorOutOfPegsTool::OnBottomLeftHandleDragBegin);
    mTransformRotationHandleHUD->OnDragBegin().AddUObject(this, &UOdysseyAnimationEditorOutOfPegsTool::OnRotationHandleDragBegin);

    mTransformTopLeftHandleHUD->OnDragged().AddUObject(this, &UOdysseyAnimationEditorOutOfPegsTool::OnTopLeftHandleDragged);
    mTransformTopRightHandleHUD->OnDragged().AddUObject(this, &UOdysseyAnimationEditorOutOfPegsTool::OnTopRightHandleDragged);
    mTransformBottomRightHandleHUD->OnDragged().AddUObject(this, &UOdysseyAnimationEditorOutOfPegsTool::OnBottomRightHandleDragged);
    mTransformBottomLeftHandleHUD->OnDragged().AddUObject(this, &UOdysseyAnimationEditorOutOfPegsTool::OnBottomLeftHandleDragged);
    mTransformRotationHandleHUD->OnDragged().AddUObject(this, &UOdysseyAnimationEditorOutOfPegsTool::OnRotationHandleDragged);

    mTransformTopLeftHandleHUD->OnDragEnd().AddUObject(this, &UOdysseyAnimationEditorOutOfPegsTool::OnTopLeftHandleDragEnd);
    mTransformTopRightHandleHUD->OnDragEnd().AddUObject(this, &UOdysseyAnimationEditorOutOfPegsTool::OnTopRightHandleDragEnd);
    mTransformBottomRightHandleHUD->OnDragEnd().AddUObject(this, &UOdysseyAnimationEditorOutOfPegsTool::OnBottomRightHandleDragEnd);
    mTransformBottomLeftHandleHUD->OnDragEnd().AddUObject(this, &UOdysseyAnimationEditorOutOfPegsTool::OnBottomLeftHandleDragEnd);
    mTransformRotationHandleHUD->OnDragEnd().AddUObject(this, &UOdysseyAnimationEditorOutOfPegsTool::OnRotationHandleDragEnd);
    
    mHUD->AddElement(mTransformHUD);
    mHUD->AddElement(mTransformTopLeftHandleHUD);
    mHUD->AddElement(mTransformTopRightHandleHUD);
    mHUD->AddElement(mTransformBottomRightHandleHUD);
    mHUD->AddElement(mTransformBottomLeftHandleHUD);
    mHUD->AddElement(mTransformRotationHandleHUD);
    mHUD->AddElement(mTransformRotationLineHUD);

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

        ::ULIS::FVec2F rotationLineStartPoint = oopTransform * ::ULIS::FVec3F(width / 2.f, height / 2.f, 1.f);
        ::ULIS::FVec2F rotationLineEndPoint = oopTransform * ::ULIS::FVec3F(width, height / 2.f, 1.f);

        mTransformRotationLineHUD->SetStartPoint(FVector2D(rotationLineStartPoint.x, rotationLineStartPoint.y));
        mTransformRotationLineHUD->SetEndPoint(FVector2D(rotationLineEndPoint.x, rotationLineEndPoint.y));

        mTransformTopLeftHandleHUD->SetPosition(points[0]);
        mTransformTopRightHandleHUD->SetPosition(points[1]);
        mTransformBottomRightHandleHUD->SetPosition(points[2]);
        mTransformBottomLeftHandleHUD->SetPosition(points[3]);
        mTransformRotationHandleHUD->SetPosition(FVector2D(rotationLineEndPoint.x, rotationLineEndPoint.y));
    }
        
}

void
UOdysseyAnimationEditorOutOfPegsTool::OnTopLeftHandleDragBegin()
{
    mZoomReference = mCell->OutOfPegs.Zoom;
    mZoomCenter = GetCenter();
    mZoomDistanceReference = FVector2D::Distance( mZoomCenter, mTransformTopLeftHandleHUD->GetPosition() );
}

void
UOdysseyAnimationEditorOutOfPegsTool::OnTopRightHandleDragBegin()
{
    mZoomReference = mCell->OutOfPegs.Zoom;
    mZoomCenter = GetCenter();
    mZoomDistanceReference = FVector2D::Distance( mZoomCenter, mTransformTopRightHandleHUD->GetPosition() );
}

void
UOdysseyAnimationEditorOutOfPegsTool::OnBottomRightHandleDragBegin()
{
    mZoomReference = mCell->OutOfPegs.Zoom;
    mZoomCenter = GetCenter();
    mZoomDistanceReference = FVector2D::Distance( mZoomCenter, mTransformBottomRightHandleHUD->GetPosition() );
}

void
UOdysseyAnimationEditorOutOfPegsTool::OnBottomLeftHandleDragBegin()
{
    mZoomReference = mCell->OutOfPegs.Zoom;
    mZoomCenter = GetCenter();
    mZoomDistanceReference = FVector2D::Distance( mZoomCenter, mTransformBottomLeftHandleHUD->GetPosition() );
}

void
UOdysseyAnimationEditorOutOfPegsTool::OnRotationHandleDragBegin()
{
    mRotationCenter = GetCenter();
}

void
UOdysseyAnimationEditorOutOfPegsTool::OnTopLeftHandleDragged()
{
    float distance = FVector2D::Distance( mZoomCenter, mTransformTopLeftHandleHUD->GetPosition() );
    float zoomRatio = distance / mZoomDistanceReference;
    float zoom = mZoomReference * zoomRatio;
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "Zoom", zoom, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
}

void
UOdysseyAnimationEditorOutOfPegsTool::OnTopRightHandleDragged()
{
    float distance = FVector2D::Distance( mZoomCenter, mTransformTopRightHandleHUD->GetPosition() );
    float zoomRatio = distance / mZoomDistanceReference;
    float zoom = mZoomReference * zoomRatio;
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "Zoom", zoom, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
}

void
UOdysseyAnimationEditorOutOfPegsTool::OnBottomRightHandleDragged()
{
    float distance = FVector2D::Distance( mZoomCenter, mTransformBottomRightHandleHUD->GetPosition() );
    float zoomRatio = distance / mZoomDistanceReference;
    float zoom = mZoomReference * zoomRatio;
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "Zoom", zoom, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
}

void
UOdysseyAnimationEditorOutOfPegsTool::OnBottomLeftHandleDragged()
{
    float distance = FVector2D::Distance( mZoomCenter, mTransformBottomLeftHandleHUD->GetPosition() );
    float zoomRatio = distance / mZoomDistanceReference;
    float zoom = mZoomReference * zoomRatio;
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "Zoom", zoom, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
}

void
UOdysseyAnimationEditorOutOfPegsTool::OnRotationHandleDragged()
{
    FVector2D vector = mTransformRotationHandleHUD->GetPosition() - mRotationCenter;
    float rotation = FMath::RadiansToDegrees(FMath::Atan2(vector.Y, vector.X));
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "Rotation", rotation, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
}

void
UOdysseyAnimationEditorOutOfPegsTool::OnTopLeftHandleDragEnd()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "Zoom", mCell->OutOfPegs.Zoom, EPropertyChangeType::ValueSet);
}

void
UOdysseyAnimationEditorOutOfPegsTool::OnTopRightHandleDragEnd()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "Zoom", mCell->OutOfPegs.Zoom, EPropertyChangeType::ValueSet);
}

void
UOdysseyAnimationEditorOutOfPegsTool::OnBottomRightHandleDragEnd()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "Zoom", mCell->OutOfPegs.Zoom, EPropertyChangeType::ValueSet);
}

void
UOdysseyAnimationEditorOutOfPegsTool::OnBottomLeftHandleDragEnd()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "Zoom", mCell->OutOfPegs.Zoom, EPropertyChangeType::ValueSet);
}

void
UOdysseyAnimationEditorOutOfPegsTool::OnRotationHandleDragEnd()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "Rotation", mCell->OutOfPegs.Rotation, EPropertyChangeType::ValueSet);
}
