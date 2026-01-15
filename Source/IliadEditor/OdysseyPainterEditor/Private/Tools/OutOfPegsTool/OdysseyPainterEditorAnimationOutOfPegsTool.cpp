// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/OutOfPegsTool/OdysseyPainterEditorAnimationOutOfPegsTool.h"

#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyHUDElement.h"
#include "OdysseyHUDPolygon.h"
#include "OdysseyHUDHandle.h"
#include "OdysseyHUDCircle.h"
#include "OdysseyHUDLine.h"
#include "OdysseyPainterEditor.h"
#include "DetailLayoutBuilder.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyLayerCell.h"

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
            return tool->GetCell() && tool->GetCell()->GetOutOfPegs().Pan != FVector2D(0, 0);
        }
    );

    FIsResetToDefaultVisible rotationIsResetToDefaultVisible = FIsResetToDefaultVisible::CreateLambda(
        [tool](TSharedPtr<IPropertyHandle> iHandle)
        {
            return tool->GetCell() && tool->GetCell()->GetOutOfPegs().Rotation != 0.f;
        }
    );

    FIsResetToDefaultVisible zoomIsResetToDefaultVisible = FIsResetToDefaultVisible::CreateLambda(
        [tool](TSharedPtr<IPropertyHandle> iHandle)
        {
            return tool->GetCell() && tool->GetCell()->GetOutOfPegs().Zoom != 100.f;
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
    Icon = *FOdysseyStyle::GetBrush("Animation.Lighttable.OutOfPegs.Button.On");
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
    UOdysseyPainterEditorTool::OnMouseDown(iPointInTexture, iKey);

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
        FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Pan), mCell->GetOutOfPegs().Pan, EPropertyChangeType::ValueSet);
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

    bool isInteractive = iEvent.ChangeType & EPropertyChangeType::Interactive;
    if ( memberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Pan) )
    {
        FOdysseyLayerCellOutOfPegs outOfPegs = mCell->GetOutOfPegs();
        outOfPegs.Pan = Pan;
        if (isInteractive)
        {
            mCell->SetOutOfPegsInteractive(outOfPegs);
        }
        else
        {
            mCell->SetOutOfPegs(outOfPegs);
        }
    }
    if ( propertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Rotation) )
    {
        FOdysseyLayerCellOutOfPegs outOfPegs = mCell->GetOutOfPegs();
        outOfPegs.Rotation = FMath::UnwindDegrees(Rotation);
        if (isInteractive)
        {
            mCell->SetOutOfPegsInteractive(outOfPegs);
        }
        else
        {
            mCell->SetOutOfPegs(outOfPegs);
        }
    }
    if ( propertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Zoom) )
    {
        FOdysseyLayerCellOutOfPegs outOfPegs = mCell->GetOutOfPegs();
        outOfPegs.Zoom = Zoom;
        if (isInteractive)
        {
            mCell->SetOutOfPegsInteractive(outOfPegs);
        }
        else
        {
            mCell->SetOutOfPegs(outOfPegs);
        }
    }
}

UOdysseyLayerCell*
UOdysseyPainterEditorAnimationOutOfPegsTool::GetCell() const
{
    return mCell;
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::SetCell(UOdysseyLayerCell* iCell)
{
    if (iCell == mCell)
        return;

    if (mCell)
        mCell->OnOutOfPegsChanged().RemoveAll(this);

    if (mLayer)
        mLayer->OnLighttableChanged().RemoveAll(this);

    mCell = iCell;

    if (mCell)
    {
        mLayer = mCell->GetLayer();
        mLayer->OnLighttableChanged().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnLighttableChanged);

        Pan = mCell->GetOutOfPegs().Pan;
        Rotation = mCell->GetOutOfPegs().Rotation;
        Zoom = mCell->GetOutOfPegs().Zoom;

        mCell->OnOutOfPegsChanged().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnCellOutOfPegsChanged);
    }

    RebuildHUD();
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnCellOutOfPegsChanged(bool iIsInteractive)
{
    Pan = mCell->GetOutOfPegs().Pan;
    Rotation = mCell->GetOutOfPegs().Rotation;
    Zoom = mCell->GetOutOfPegs().Zoom;

    RefreshHUD();
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnLighttableChanged()
{
    if (!mLayer->GetLighttable().bIsActivated)
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
    UOdysseyAnimation* animation = Cast<UOdysseyAnimationLayer>(mLayer)->GetAnimation();
    if (!animation)
        return FVector2D();

    float width = animation->GetWidth();
    float height = animation->GetHeight();

    FMatrix oopTransform = mCell->OutOfPegsTransform();
    FVector center = oopTransform.TransformPosition(FVector(width / 2.f, height / 2.f, 1.f));
    return FVector2D(center.X, center.Y);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::RefreshHUD()
{
    UOdysseyAnimation* animation = Cast<UOdysseyAnimationLayer>(mLayer)->GetAnimation();
    if (!animation)
        return;

    float width = animation->GetWidth();
    float height = animation->GetHeight();

    FMatrix oopTransform = mCell->OutOfPegsTransform();

    //Transform HUD
    if (mTransformHUD)
    {
        TArray<FVector2D>& points = mTransformHUD->GetPoints();

        FVector topLeft = oopTransform.TransformPosition(FVector(0.f, 0.f, 1.f));
        FVector topRight = oopTransform.TransformPosition(FVector(width, 0.f, 1.f));
        FVector bottomRight = oopTransform.TransformPosition(FVector(width, height, 1.f));
        FVector bottomLeft = oopTransform.TransformPosition(FVector(0.f, height, 1.f));

        points[0] = FVector2D(topLeft.X, topLeft.Y); //top left
        points[1] = FVector2D(topRight.X, topRight.Y); //top right
        points[2] = FVector2D(bottomRight.X, bottomRight.Y); //bottom right
        points[3] = FVector2D(bottomLeft.X, bottomLeft.Y); //bottom left

        FVector rotationLineStartPoint = oopTransform.TransformPosition(FVector(width / 2.f, height / 2.f, 1.f));
        FVector rotationLineEndPoint = oopTransform.TransformPosition(FVector(width, height / 2.f, 1.f));

        mTransformRotationLineHUD->SetStartPoint(FVector2D(rotationLineStartPoint.X, rotationLineStartPoint.Y));
        mTransformRotationLineHUD->SetEndPoint(FVector2D(rotationLineEndPoint.X, rotationLineEndPoint.Y));

        mTransformTopLeftHandleHUD->SetPosition(points[0]);
        mTransformTopRightHandleHUD->SetPosition(points[1]);
        mTransformBottomRightHandleHUD->SetPosition(points[2]);
        mTransformBottomLeftHandleHUD->SetPosition(points[3]);
        mTransformRotationHandleHUD->SetPosition(FVector2D(rotationLineEndPoint.X, rotationLineEndPoint.Y));
    }

}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopLeftHandleDragBegin()
{
    mZoomReference = mCell->GetOutOfPegs().Zoom;
    mZoomCenter = GetCenter();
    mZoomDistanceReference = FVector2D::Distance( mZoomCenter, mTransformTopLeftHandleHUD->GetPosition() );
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopRightHandleDragBegin()
{
    mZoomReference = mCell->GetOutOfPegs().Zoom;
    mZoomCenter = GetCenter();
    mZoomDistanceReference = FVector2D::Distance( mZoomCenter, mTransformTopRightHandleHUD->GetPosition() );
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomRightHandleDragBegin()
{
    mZoomReference = mCell->GetOutOfPegs().Zoom;
    mZoomCenter = GetCenter();
    mZoomDistanceReference = FVector2D::Distance( mZoomCenter, mTransformBottomRightHandleHUD->GetPosition() );
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomLeftHandleDragBegin()
{
    mZoomReference = mCell->GetOutOfPegs().Zoom;
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
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Zoom), mCell->GetOutOfPegs().Zoom, EPropertyChangeType::ValueSet);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopRightHandleDragEnd()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Zoom), mCell->GetOutOfPegs().Zoom, EPropertyChangeType::ValueSet);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomRightHandleDragEnd()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Zoom), mCell->GetOutOfPegs().Zoom, EPropertyChangeType::ValueSet);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomLeftHandleDragEnd()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Zoom), mCell->GetOutOfPegs().Zoom, EPropertyChangeType::ValueSet);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnRotationHandleDragEnd()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Rotation), mCell->GetOutOfPegs().Rotation, EPropertyChangeType::ValueSet);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::Reset()
{
    mCell->SetOutOfPegs(FOdysseyLayerCellOutOfPegs());
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::ResetAll()
{
    const TArray<UOdysseyLayerCell*> cells = mLayer->GetCells();
    for (UOdysseyLayerCell* cell : cells)
    {
        cell->SetOutOfPegs(FOdysseyLayerCellOutOfPegs());
    }
}
