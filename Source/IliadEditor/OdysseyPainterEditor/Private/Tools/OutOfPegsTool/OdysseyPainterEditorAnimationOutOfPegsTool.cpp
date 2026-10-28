// IDDN.FR.001.060015.015.S.X.2019.000.00000
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
    TSharedRef<IPropertyHandle> scaleHandle = DetailLayout.GetProperty( GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Scale));

    IDetailPropertyRow* panRow = DetailLayout.EditDefaultProperty(panHandle);
    IDetailPropertyRow* rotationRow = DetailLayout.EditDefaultProperty(rotationHandle);
    IDetailPropertyRow* scaleRow = DetailLayout.EditDefaultProperty(scaleHandle);

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

    FIsResetToDefaultVisible scaleIsResetToDefaultVisible = FIsResetToDefaultVisible::CreateLambda(
        [tool](TSharedPtr<IPropertyHandle> iHandle)
        {
            return tool->GetCell() && tool->GetCell()->GetOutOfPegs().Scale != FVector2D( 100.f, 100.f );
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
    FResetToDefaultOverride scaleResetToDefault = FResetToDefaultOverride::Create(scaleIsResetToDefaultVisible, onResetToDefaultClicked);

    panRow->OverrideResetToDefault(panResetToDefault);
    rotationRow->OverrideResetToDefault(rotationResetToDefault);
    scaleRow->OverrideResetToDefault(scaleResetToDefault);
}

UOdysseyPainterEditorAnimationOutOfPegsTool::~UOdysseyPainterEditorAnimationOutOfPegsTool()
{

}

UOdysseyPainterEditorAnimationOutOfPegsTool::UOdysseyPainterEditorAnimationOutOfPegsTool()
{
    mIsTemporaryTool = true;
    mIconStyleSet = FName(TEXT("Animation.Lighttable.OutOfPegs.Button.On"));
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
    if ( memberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Scale) )
    {
        FOdysseyLayerCellOutOfPegs outOfPegs = mCell->GetOutOfPegs();
        outOfPegs.Scale = Scale;
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
        Scale = mCell->GetOutOfPegs().Scale;

        mCell->OnOutOfPegsChanged().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnCellOutOfPegsChanged);
    }

    RebuildHUD();
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnCellOutOfPegsChanged(bool iIsInteractive)
{
    Pan = mCell->GetOutOfPegs().Pan;
    Rotation = mCell->GetOutOfPegs().Rotation;
    Scale = mCell->GetOutOfPegs().Scale;

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
    mTransformTopHandleHUD = nullptr;
    mTransformTopRightHandleHUD = nullptr;
    mTransformRightHandleHUD = nullptr;
    mTransformBottomRightHandleHUD = nullptr;
    mTransformBottomHandleHUD = nullptr;
    mTransformBottomLeftHandleHUD = nullptr;
    mTransformLeftHandleHUD = nullptr;
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
    mTransformTopHandleHUD = MakeShared<FOdysseyHUDHandle>(FVector2D(0, 0));
    mTransformTopRightHandleHUD = MakeShared<FOdysseyHUDHandle>(FVector2D(0, 0));
    mTransformRightHandleHUD = MakeShared<FOdysseyHUDHandle>(FVector2D(0, 0));
    mTransformBottomRightHandleHUD = MakeShared<FOdysseyHUDHandle>(FVector2D(0, 0));
    mTransformBottomHandleHUD = MakeShared<FOdysseyHUDHandle>(FVector2D(0, 0));
    mTransformBottomLeftHandleHUD = MakeShared<FOdysseyHUDHandle>(FVector2D(0, 0));
    mTransformLeftHandleHUD = MakeShared<FOdysseyHUDHandle>(FVector2D(0, 0));
    mTransformRotationHandleHUD = MakeShared<FOdysseyHUDHandle>(FVector2D(0, 0));
    mTransformRotationLineHUD = MakeShared<FOdysseyHUDLine>(FVector2D(0, 0), FVector2D(0, 0));

    mTransformTopLeftHandleHUD->OnDragBegin().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopLeftHandleDragBegin);
    mTransformTopHandleHUD->OnDragBegin().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopHandleDragBegin);
    mTransformTopRightHandleHUD->OnDragBegin().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopRightHandleDragBegin);
    mTransformRightHandleHUD->OnDragBegin().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnRightHandleDragBegin);
    mTransformBottomRightHandleHUD->OnDragBegin().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomRightHandleDragBegin);
    mTransformBottomHandleHUD->OnDragBegin().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomHandleDragBegin);
    mTransformBottomLeftHandleHUD->OnDragBegin().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomLeftHandleDragBegin);
    mTransformLeftHandleHUD->OnDragBegin().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnLeftHandleDragBegin);
    mTransformRotationHandleHUD->OnDragBegin().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnRotationHandleDragBegin);

    mTransformTopLeftHandleHUD->OnDragged().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopLeftHandleDragged);
    mTransformTopHandleHUD->OnDragged().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopHandleDragged);
    mTransformTopRightHandleHUD->OnDragged().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopRightHandleDragged);
    mTransformRightHandleHUD->OnDragged().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnRightHandleDragged);
    mTransformBottomRightHandleHUD->OnDragged().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomRightHandleDragged);
    mTransformBottomHandleHUD->OnDragged().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomHandleDragged);
    mTransformBottomLeftHandleHUD->OnDragged().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomLeftHandleDragged);
    mTransformLeftHandleHUD->OnDragged().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnLeftHandleDragged);
    mTransformRotationHandleHUD->OnDragged().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnRotationHandleDragged);

    mTransformTopLeftHandleHUD->OnDragEnd().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopLeftHandleDragEnd);
    mTransformTopHandleHUD->OnDragEnd().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopHandleDragEnd);
    mTransformTopRightHandleHUD->OnDragEnd().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopRightHandleDragEnd);
    mTransformRightHandleHUD->OnDragEnd().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnRightHandleDragEnd);
    mTransformBottomRightHandleHUD->OnDragEnd().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomRightHandleDragEnd);
    mTransformBottomHandleHUD->OnDragEnd().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomHandleDragEnd);
    mTransformBottomLeftHandleHUD->OnDragEnd().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomLeftHandleDragEnd);
    mTransformLeftHandleHUD->OnDragEnd().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnLeftHandleDragEnd);
    mTransformRotationHandleHUD->OnDragEnd().AddUObject(this, &UOdysseyPainterEditorAnimationOutOfPegsTool::OnRotationHandleDragEnd);

    mHUD->AddElement(mTransformHUD);
    mHUD->AddElement(mTransformTopLeftHandleHUD);
    mHUD->AddElement(mTransformTopHandleHUD);
    mHUD->AddElement(mTransformTopRightHandleHUD);
    mHUD->AddElement(mTransformRightHandleHUD);
    mHUD->AddElement(mTransformBottomRightHandleHUD);
    mHUD->AddElement(mTransformBottomHandleHUD);
    mHUD->AddElement(mTransformBottomLeftHandleHUD);
    mHUD->AddElement(mTransformLeftHandleHUD);
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
        FVector top = oopTransform.TransformPosition(FVector( width / 2.f, 0.f, 1.f));
        FVector topRight = oopTransform.TransformPosition(FVector(width, 0.f, 1.f));
        FVector Right = oopTransform.TransformPosition(FVector(width, height / 2.f, 1.f));
        FVector bottomRight = oopTransform.TransformPosition(FVector(width, height, 1.f));
        FVector bottom = oopTransform.TransformPosition(FVector( width / 2.f, height, 1.f));
        FVector bottomLeft = oopTransform.TransformPosition(FVector(0.f, height, 1.f));
        FVector Left = oopTransform.TransformPosition(FVector(0.f, height / 2.f, 1.f));

        points[0] = FVector2D(topLeft.X, topLeft.Y); //top left
        points[1] = FVector2D(topRight.X, topRight.Y); //top right
        points[2] = FVector2D(bottomRight.X, bottomRight.Y); //bottom right
        points[3] = FVector2D(bottomLeft.X, bottomLeft.Y); //bottom left

        FVector rotationLineStartPoint = oopTransform.TransformPosition(FVector(width / 2.f, height / 2.f, 1.f));
        FVector rotationLineEndPoint = oopTransform.TransformPosition(FVector(width + 200 /* offset */, height / 2.f, 1.f));

        mTransformRotationLineHUD->SetStartPoint(FVector2D(rotationLineStartPoint.X, rotationLineStartPoint.Y));
        mTransformRotationLineHUD->SetEndPoint(FVector2D(rotationLineEndPoint.X, rotationLineEndPoint.Y));

        mTransformTopLeftHandleHUD->SetPosition(points[0]);
        mTransformTopHandleHUD->SetPosition(FVector2D(top.X, top.Y));
        mTransformTopRightHandleHUD->SetPosition(points[1]);
        mTransformRightHandleHUD->SetPosition( FVector2D( Right.X, Right.Y ) );
        mTransformBottomRightHandleHUD->SetPosition(points[2]);
        mTransformBottomHandleHUD->SetPosition( FVector2D( bottom.X, bottom.Y ) );
        mTransformBottomLeftHandleHUD->SetPosition(points[3]);
        mTransformLeftHandleHUD->SetPosition( FVector2D( Left.X, Left.Y ) );
        mTransformRotationHandleHUD->SetPosition(FVector2D(rotationLineEndPoint.X, rotationLineEndPoint.Y));
    }

}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopLeftHandleDragBegin()
{
    mScaleReference = mCell->GetOutOfPegs().Scale;
    mScaleCenter = GetCenter();
    mScaleDistanceReference = FVector2D::Distance( mScaleCenter, mTransformTopLeftHandleHUD->GetPosition() );
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopHandleDragBegin()
{
    mScaleReference = mCell->GetOutOfPegs().Scale;
    mScaleCenter = GetCenter();
    mScaleDistanceReference = FVector2D::Distance( mScaleCenter, mTransformTopHandleHUD->GetPosition() );
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopRightHandleDragBegin()
{
    mScaleReference = mCell->GetOutOfPegs().Scale;
    mScaleCenter = GetCenter();
    mScaleDistanceReference = FVector2D::Distance( mScaleCenter, mTransformTopRightHandleHUD->GetPosition() );
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnRightHandleDragBegin()
{
    mScaleReference = mCell->GetOutOfPegs().Scale;
    mScaleCenter = GetCenter();
    mScaleDistanceReference = FVector2D::Distance( mScaleCenter, mTransformRightHandleHUD->GetPosition() );
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomRightHandleDragBegin()
{
    mScaleReference = mCell->GetOutOfPegs().Scale;
    mScaleCenter = GetCenter();
    mScaleDistanceReference = FVector2D::Distance( mScaleCenter, mTransformBottomRightHandleHUD->GetPosition() );
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomHandleDragBegin()
{
    mScaleReference = mCell->GetOutOfPegs().Scale;
    mScaleCenter = GetCenter();
    mScaleDistanceReference = FVector2D::Distance( mScaleCenter, mTransformBottomHandleHUD->GetPosition() );
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomLeftHandleDragBegin()
{
    mScaleReference = mCell->GetOutOfPegs().Scale;
    mScaleCenter = GetCenter();
    mScaleDistanceReference = FVector2D::Distance( mScaleCenter, mTransformBottomLeftHandleHUD->GetPosition() );
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnLeftHandleDragBegin()
{
    mScaleReference = mCell->GetOutOfPegs().Scale;
    mScaleCenter = GetCenter();
    mScaleDistanceReference = FVector2D::Distance( mScaleCenter, mTransformLeftHandleHUD->GetPosition() );
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnRotationHandleDragBegin()
{
    mRotationCenter = GetCenter();
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopLeftHandleDragged()
{
    float distance = FVector2D::Distance( mScaleCenter, mTransformTopLeftHandleHUD->GetPosition() );
    float scaleRatio = distance / mScaleDistanceReference;
    FVector2D scale = mScaleReference * scaleRatio;
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Scale), scale, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopHandleDragged()
{
    float distance = FVector2D::Distance( mScaleCenter, mTransformTopHandleHUD->GetPosition() );
    float scaleRatio = distance / mScaleDistanceReference;
    FVector2D scale( mScaleReference.X, mScaleReference.Y * scaleRatio );
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Scale), scale, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopRightHandleDragged()
{
    float distance = FVector2D::Distance( mScaleCenter, mTransformTopRightHandleHUD->GetPosition() );
    float scaleRatio = distance / mScaleDistanceReference;
    FVector2D scale = mScaleReference * scaleRatio;
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Scale), scale, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnRightHandleDragged()
{
    float distance = FVector2D::Distance( mScaleCenter, mTransformRightHandleHUD->GetPosition() );
    float scaleRatio = distance / mScaleDistanceReference;
    FVector2D scale( mScaleReference.X * scaleRatio, mScaleReference.Y );
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Scale), scale, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomRightHandleDragged()
{
    float distance = FVector2D::Distance( mScaleCenter, mTransformBottomRightHandleHUD->GetPosition() );
    float scaleRatio = distance / mScaleDistanceReference;
    FVector2D scale = mScaleReference * scaleRatio;
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Scale), scale, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomHandleDragged()
{
    float distance = FVector2D::Distance( mScaleCenter, mTransformBottomHandleHUD->GetPosition() );
    float scaleRatio = distance / mScaleDistanceReference;
    FVector2D scale( mScaleReference.X, mScaleReference.Y * scaleRatio );
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Scale), scale, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomLeftHandleDragged()
{
    float distance = FVector2D::Distance( mScaleCenter, mTransformBottomLeftHandleHUD->GetPosition() );
    float scaleRatio = distance / mScaleDistanceReference;
    FVector2D scale = mScaleReference * scaleRatio;
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Scale), scale, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnLeftHandleDragged()
{
    float distance = FVector2D::Distance( mScaleCenter, mTransformLeftHandleHUD->GetPosition() );
    float scaleRatio = distance / mScaleDistanceReference;
    FVector2D scale( mScaleReference.X * scaleRatio, mScaleReference.Y );
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Scale), scale, EPropertyChangeType::ValueSet | EPropertyChangeType::Interactive);
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
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Scale), mCell->GetOutOfPegs().Scale, EPropertyChangeType::ValueSet);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopHandleDragEnd()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Scale), mCell->GetOutOfPegs().Scale, EPropertyChangeType::ValueSet);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnTopRightHandleDragEnd()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Scale), mCell->GetOutOfPegs().Scale, EPropertyChangeType::ValueSet);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnRightHandleDragEnd()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Scale), mCell->GetOutOfPegs().Scale, EPropertyChangeType::ValueSet);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomRightHandleDragEnd()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Scale), mCell->GetOutOfPegs().Scale, EPropertyChangeType::ValueSet);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomHandleDragEnd()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Scale), mCell->GetOutOfPegs().Scale, EPropertyChangeType::ValueSet);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnBottomLeftHandleDragEnd()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Scale), mCell->GetOutOfPegs().Scale, EPropertyChangeType::ValueSet);
}

void
UOdysseyPainterEditorAnimationOutOfPegsTool::OnLeftHandleDragEnd()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorAnimationOutOfPegsTool, Scale), mCell->GetOutOfPegs().Scale, EPropertyChangeType::ValueSet);
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
