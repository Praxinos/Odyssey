// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Tools/OutOfPegsTool/OdysseyPainterEditorAnimationOutOfPegsTool.h"

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
#include "DetailLayoutBuilder.h"

#include <ULIS>

TSharedRef<IDetailCustomization>
FOdysseyPainterEditorAnimationOutOfPegsToolDetails::MakeInstance()
{
    return MakeShared<FOdysseyPainterEditorAnimationOutOfPegsToolDetails>();
}

void
FOdysseyPainterEditorAnimationOutOfPegsToolDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
    TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
    DetailLayout.GetObjectsBeingCustomized(ObjectsBeingCustomized);

    UOdysseyPainterEditorAnimationOutOfPegsTool* tool = Cast<UOdysseyPainterEditorAnimationOutOfPegsTool>(ObjectsBeingCustomized[0].Get());
    if (!tool)
        return;

    TSharedRef<IPropertyHandle> panHandle = DetailLayout.GetProperty( GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Pan));
    TSharedRef<IPropertyHandle> rotationHandle = DetailLayout.GetProperty( GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Rotation));
    TSharedRef<IPropertyHandle> zoomHandle = DetailLayout.GetProperty( GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Zoom));

    IDetailPropertyRow* panRow = DetailLayout.EditDefaultProperty(panHandle);
    IDetailPropertyRow* rotationRow = DetailLayout.EditDefaultProperty(rotationHandle);
    IDetailPropertyRow* zoomRow = DetailLayout.EditDefaultProperty(zoomHandle);

    FIsResetToDefaultVisible panIsResetToDefaultVisible = FIsResetToDefaultVisible::CreateLambda(
        [tool](TSharedPtr<IPropertyHandle> iHandle)
        {
            return tool->GetCell() && tool->GetCell()->OutOfPegs.Pan != FVector2D(0, 0);
        }
    );

    FIsResetToDefaultVisible rotationIsResetToDefaultVisible = FIsResetToDefaultVisible::CreateLambda(
        [tool](TSharedPtr<IPropertyHandle> iHandle)
        {
            return tool->GetCell() && tool->GetCell()->OutOfPegs.Rotation != 0.f;
        }
    );

    FIsResetToDefaultVisible zoomIsResetToDefaultVisible = FIsResetToDefaultVisible::CreateLambda(
        [tool](TSharedPtr<IPropertyHandle> iHandle)
        {
            return tool->GetCell() && tool->GetCell()->OutOfPegs.Zoom != 100.f;
        }
    );

    FResetToDefaultHandler onResetToDefaultClicked = FResetToDefaultHandler::CreateLambda(
        [](TSharedPtr<IPropertyHandle> iHandle)
        {
            iHandle->ResetToDefault();
        }
    );

    FResetToDefaultOverride panResetToDefault = FResetToDefaultOverride::Create(panIsResetToDefaultVisible, onResetToDefaultClicked);
    FResetToDefaultOverride rotationResetToDefault = FResetToDefaultOverride::Create(rotationIsResetToDefaultVisible, onResetToDefaultClicked);
    FResetToDefaultOverride zoomResetToDefault = FResetToDefaultOverride::Create(zoomIsResetToDefaultVisible, onResetToDefaultClicked);

    panRow->OverrideResetToDefault(panResetToDefault);
    rotationRow->OverrideResetToDefault(rotationResetToDefault);
    zoomRow->OverrideResetToDefault(zoomResetToDefault);
}

UOdysseyPainterEditorAnimationOutOfPegsTool::~UOdysseyPainterEditorAnimationOutOfPegsTool()
{

}

UOdysseyPainterEditorAnimationOutOfPegsTool::UOdysseyPainterEditorAnimationOutOfPegsTool()
{
    mIsTemporaryTool = true;
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::Load()
{
    UOdysseyPainterEditorTool::Load();

    RebuildHUD();
}

bool
UOdysseyPainterEditorAnimationOutOfPegsTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
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
UOdysseyPainterEditorAnimationOutOfPegsTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    FOdysseyPoint point = iPointInTexture;
    point.x = FMath::RoundToInt(point.x);
    point.y = FMath::RoundToInt(point.y);

    if (mIsPanning)
    {
        FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Pan), mCell->OutOfPegs.Pan, EPropertyChangeType::ValueSet);
        mIsPanning = false;
    }
    return true;
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    FOdysseyPoint point = iPointInTexture;
    point.x = FMath::RoundToInt(point.x);
    point.y = FMath::RoundToInt(point.y);

    if (mIsPanning)
    {
        FVector2D pan = mPanReference + FVector2D(point.x, point.y) - mPanPointReference;
        FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Pan), pan, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
    }
}

EMouseCursor::Type
UOdysseyPainterEditorAnimationOutOfPegsTool::GetMouseCursor() const
{
    return UOdysseyPainterEditorTool::GetMouseCursor();
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::PostEditChangeProperty( FPropertyChangedEvent& iEvent)
{
    Super::PostEditChangeProperty(iEvent);

    FName propertyName = iEvent.GetPropertyName();
    FName memberPropertyName = iEvent.MemberProperty->GetFName();

    bool isInteractive = EPropertyChangeType::ValueSet | (iEvent.ChangeType & EPropertyChangeType::Interactive);
    if ( memberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Pan) )
    {
        FOdysseyAnimationCellOutOfPegs outOfPegs = mCell->OutOfPegs;
        outOfPegs.Pan = Pan;
        FOdysseyObjectEditorUtils::SetPropertyValue(mCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, OutOfPegs), outOfPegs, iEvent.ChangeType);
    }
    if ( propertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Rotation) )
    {
        FOdysseyAnimationCellOutOfPegs outOfPegs = mCell->OutOfPegs;
        outOfPegs.Rotation = FMath::UnwindDegrees(Rotation);
        FOdysseyObjectEditorUtils::SetPropertyValue(mCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, OutOfPegs), outOfPegs, iEvent.ChangeType);
    }
    if ( propertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Zoom) )
    {
        FOdysseyAnimationCellOutOfPegs outOfPegs = mCell->OutOfPegs;
        outOfPegs.Zoom = Zoom;
        FOdysseyObjectEditorUtils::SetPropertyValue(mCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, OutOfPegs), outOfPegs, iEvent.ChangeType);
    }
}

UOdysseyAnimationCell*
UOdysseyPainterEditorAnimationOutOfPegsTool::GetCell() const
{
    return mCell;
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::SetCell(UOdysseyAnimationCell* iCell)
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
        mLayer->OnLightTableChanged().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnLightTableChanged);

        Pan = mCell->OutOfPegs.Pan;
        Rotation = mCell->OutOfPegs.Rotation;
        Zoom = mCell->OutOfPegs.Zoom;

        mCell->OnOutOfPegsChanged().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnCellOutOfPegsChanged);
    }

    RebuildHUD();
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnCellOutOfPegsChanged(bool iIsInteractive)
{
    Pan = mCell->OutOfPegs.Pan;
    Rotation = mCell->OutOfPegs.Rotation;
    Zoom = mCell->OutOfPegs.Zoom;

    RefreshHUD();
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnLightTableChanged()
{
    if (!mLayer->Lighttable.bIsActivated)
    {
        GetEditor()->InactivateTemporaryTool();
    }
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::RebuildHUD()
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

    mTransformTopLeftHandleHUD->OnDragBegin().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopLeftHandleDragBegin);
    mTransformTopRightHandleHUD->OnDragBegin().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopRightHandleDragBegin);
    mTransformBottomRightHandleHUD->OnDragBegin().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomRightHandleDragBegin);
    mTransformBottomLeftHandleHUD->OnDragBegin().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomLeftHandleDragBegin);
    mTransformRotationHandleHUD->OnDragBegin().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnRotationHandleDragBegin);

    mTransformTopLeftHandleHUD->OnDragged().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopLeftHandleDragged);
    mTransformTopRightHandleHUD->OnDragged().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopRightHandleDragged);
    mTransformBottomRightHandleHUD->OnDragged().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomRightHandleDragged);
    mTransformBottomLeftHandleHUD->OnDragged().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomLeftHandleDragged);
    mTransformRotationHandleHUD->OnDragged().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnRotationHandleDragged);

    mTransformTopLeftHandleHUD->OnDragEnd().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopLeftHandleDragEnd);
    mTransformTopRightHandleHUD->OnDragEnd().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopRightHandleDragEnd);
    mTransformBottomRightHandleHUD->OnDragEnd().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomRightHandleDragEnd);
    mTransformBottomLeftHandleHUD->OnDragEnd().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomLeftHandleDragEnd);
    mTransformRotationHandleHUD->OnDragEnd().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnRotationHandleDragEnd);

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
UOdysseyPainterEditorAnimationOutOfPegsTool::GetCenter() const
{
    UOdysseyAnimationLayer* layer = mCell->GetLayer();
    if (!layer)
        return FVector2D();

    UOdysseyAnimation* animation = layer->GetAnimation();
    if (!animation)
        return FVector2D();

    float width = animation->GetWidth();
    float height = animation->GetHeight();

    ::ULIS::FMat3F oopTransform = mCell->OutOfPegsTransform();
    ::ULIS::FVec2F center = oopTransform * ::ULIS::FVec3F(width / 2.f, height / 2.f, 1.f);
    return FVector2D(center.x, center.y);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::RefreshHUD()
{
    UOdysseyAnimationLayer* layer = mCell->GetLayer();
    if (!layer)
        return;

    UOdysseyAnimation* animation = layer->GetAnimation();
    if (!animation)
        return;

    float width = animation->GetWidth();
    float height = animation->GetHeight();

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
UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopLeftHandleDragBegin()
{
    mZoomReference = mCell->OutOfPegs.Zoom;
    mZoomCenter = GetCenter();
    mZoomDistanceReference = FVector2D::Distance( mZoomCenter, mTransformTopLeftHandleHUD->GetPosition() );
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopRightHandleDragBegin()
{
    mZoomReference = mCell->OutOfPegs.Zoom;
    mZoomCenter = GetCenter();
    mZoomDistanceReference = FVector2D::Distance( mZoomCenter, mTransformTopRightHandleHUD->GetPosition() );
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomRightHandleDragBegin()
{
    mZoomReference = mCell->OutOfPegs.Zoom;
    mZoomCenter = GetCenter();
    mZoomDistanceReference = FVector2D::Distance( mZoomCenter, mTransformBottomRightHandleHUD->GetPosition() );
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomLeftHandleDragBegin()
{
    mZoomReference = mCell->OutOfPegs.Zoom;
    mZoomCenter = GetCenter();
    mZoomDistanceReference = FVector2D::Distance( mZoomCenter, mTransformBottomLeftHandleHUD->GetPosition() );
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnRotationHandleDragBegin()
{
    mRotationCenter = GetCenter();
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopLeftHandleDragged()
{
    float distance = FVector2D::Distance( mZoomCenter, mTransformTopLeftHandleHUD->GetPosition() );
    float zoomRatio = distance / mZoomDistanceReference;
    float zoom = mZoomReference * zoomRatio;
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Zoom), zoom, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopRightHandleDragged()
{
    float distance = FVector2D::Distance( mZoomCenter, mTransformTopRightHandleHUD->GetPosition() );
    float zoomRatio = distance / mZoomDistanceReference;
    float zoom = mZoomReference * zoomRatio;
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Zoom), zoom, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomRightHandleDragged()
{
    float distance = FVector2D::Distance( mZoomCenter, mTransformBottomRightHandleHUD->GetPosition() );
    float zoomRatio = distance / mZoomDistanceReference;
    float zoom = mZoomReference * zoomRatio;
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Zoom), zoom, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomLeftHandleDragged()
{
    float distance = FVector2D::Distance( mZoomCenter, mTransformBottomLeftHandleHUD->GetPosition() );
    float zoomRatio = distance / mZoomDistanceReference;
    float zoom = mZoomReference * zoomRatio;
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Zoom), zoom, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnRotationHandleDragged()
{
    FVector2D vector = mTransformRotationHandleHUD->GetPosition() - mRotationCenter;
    float rotation = FMath::RadiansToDegrees(FMath::Atan2(vector.Y, vector.X));
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Rotation), rotation, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopLeftHandleDragEnd()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Zoom), mCell->OutOfPegs.Zoom, EPropertyChangeType::ValueSet);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopRightHandleDragEnd()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Zoom), mCell->OutOfPegs.Zoom, EPropertyChangeType::ValueSet);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomRightHandleDragEnd()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Zoom), mCell->OutOfPegs.Zoom, EPropertyChangeType::ValueSet);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomLeftHandleDragEnd()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Zoom), mCell->OutOfPegs.Zoom, EPropertyChangeType::ValueSet);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnRotationHandleDragEnd()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Rotation), mCell->OutOfPegs.Rotation, EPropertyChangeType::ValueSet);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::Reset()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, OutOfPegs), FOdysseyAnimationCellOutOfPegs());
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::ResetAll()
{
    const TArray<UOdysseyAnimationCell*> cells = mCell->GetLayer()->GetCells();
    for (UOdysseyAnimationCell* cell : cells)
    {
        if (cell)
            FOdysseyObjectEditorUtils::SetPropertyValue(cell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, OutOfPegs), FOdysseyAnimationCellOutOfPegs());
    }
}
