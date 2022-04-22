// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyHUDEllipse.h"


void UOdysseyHUDEllipse::Init( FName iName, FVector2D iCenterPoint, FVector2D iEndPoint, int iAngle, FTransform2D iTransform )
{
    UOdysseyHUDElement::Init( iName, iTransform );
    mCenterPoint = mPreviousCenterPoint = iCenterPoint;
    mEndPoint = mPreviousEndPoint = iEndPoint;
    mEllipseAaxis = mPreviousEllipseAaxis = (int)(mCenterPoint.X - mEndPoint.X);
    mEllipseBaxis = mPreviousEllipseBaxis =(int)(mCenterPoint.Y - mEndPoint.Y);
    mAngle = mPreviousAngle = iAngle;
}

void UOdysseyHUDEllipse::Init( FName iName, FVector2D iCenterPoint, int iEllipseAaxis, int iEllipseBaxis, int iAngle, FTransform2D iTransform )
{
    UOdysseyHUDElement::Init( iName, iTransform );
    mCenterPoint = mPreviousCenterPoint = iCenterPoint;
    mEllipseAaxis = mPreviousEllipseAaxis = iEllipseAaxis;
    mEllipseBaxis = mPreviousEllipseBaxis =iEllipseBaxis;
    mEndPoint = mPreviousEndPoint = FVector2D( iCenterPoint.X + iEllipseAaxis, iCenterPoint.Y + iEllipseBaxis);
    mAngle = mPreviousAngle = iAngle;
}

TSharedPtr<SWidget> UOdysseyHUDEllipse::CreateWidget()
{
    UOdysseyHUDElement::CreateWidget();

    //TODO: custom widget instead of property view. Will be cleaner.
    FPropertyEditorModule& propertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

    FDetailsViewArgs args;

    mDetailsView = propertyModule.CreateDetailView(args);
    mDetailsView->SetObject(this);

    mElementsWidget->AddSlot()
    [
        mDetailsView->AsShared()
    ];

    return mElementsWidget;
}

void UOdysseyHUDEllipse::Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    if( !ioBlock )
        return;

    if ( mIsInvalid || mPreviousAngle != mAngle || mPreviousCenterPoint != mCenterPoint || mPreviousEndPoint != mEndPoint || mEllipseAaxis != mPreviousEllipseAaxis || mEllipseBaxis != mPreviousEllipseBaxis || mPreviousTransform != iTransform)
    {
        Erase(ioBlock, iTransform);
        //Draw the children of this HUDElement
        UOdysseyHUDElement::Draw(ioBlock, iTransform);
    }
    else
    {
        //Draw the children of this HUDElement
        UOdysseyHUDElement::Draw(ioBlock, iTransform);
        return;
    }

    FVector2D transformedCenterPoint = iTransform.TransformPoint(mCenterPoint);
    FVector2D transformedEndPoint = iTransform.TransformPoint(mEndPoint);
    int transformedAngle = (int)(iTransform.GetMatrix().GetRotationAngle() + mAngle);
    int transformedEllipseAaxis = (int)(iTransform.GetMatrix().GetScale().GetVector().X * mEllipseAaxis);
    int transformedEllipseBaxis = (int)(iTransform.GetMatrix().GetScale().GetVector().X * mEllipseBaxis);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawRotatedEllipse(*(ioBlock), ::ULIS::FVec2I(transformedCenterPoint.X, transformedCenterPoint.Y), FMath::Abs(transformedEllipseAaxis), FMath::Abs(transformedEllipseBaxis), transformedAngle, ::ULIS::FColor::RGBA8(0, 255, 0, 255));
    ctx.Finish();

    mPreviousAngle = mAngle;
    mPreviousEllipseAaxis = mEllipseAaxis;
    mPreviousEllipseBaxis = mEllipseBaxis;
    mPreviousCenterPoint = mCenterPoint;
    mPreviousEndPoint = mEndPoint;
    mPreviousTransform = iTransform;

    ioBlock->Dirty();
}

void UOdysseyHUDEllipse::Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    if (!ioBlock)
        return;
 
    //Erase the children of this HUDElement
    UOdysseyHUDElement::Erase(ioBlock, iTransform);

    FVector2D previousTransformedCenterPoint = mPreviousTransform.TransformPoint(mPreviousCenterPoint);
    FVector2D previousTransformedEndPoint = mPreviousTransform.TransformPoint(mPreviousEndPoint);
    int previousTransformedEllipseAaxis = (int)(iTransform.GetMatrix().GetScale().GetVector().X * mPreviousEllipseAaxis);
    int previousTransformedEllipseBaxis = (int)(iTransform.GetMatrix().GetScale().GetVector().X * mPreviousEllipseBaxis);
    int previousTransformedAngle = (int)(iTransform.GetMatrix().GetRotationAngle() + mPreviousAngle);


    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawRotatedEllipse(*(ioBlock), ::ULIS::FVec2I(previousTransformedCenterPoint.X, previousTransformedCenterPoint.Y), FMath::Abs(previousTransformedEllipseAaxis), FMath::Abs(previousTransformedEllipseBaxis), previousTransformedAngle, ::ULIS::FColor::RGBA8(0, 255, 0, 0));
    ctx.Finish();
}
