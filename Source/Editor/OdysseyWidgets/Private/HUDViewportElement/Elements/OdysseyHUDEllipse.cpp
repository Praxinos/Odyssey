// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDEllipse.h"


void UOdysseyHUDEllipse::Init( FName iName, FVector2D iCenterPoint, FVector2D iBorderPoint, FTransform2D iTransform )
{
    UOdysseyHUDElement::Init( iName, iTransform );
    mCenterPoint = mPreviousCenterPoint = iCenterPoint;
    mBorderPoint = mPreviousBorderPoint = iBorderPoint;
    mEllipseAaxis = mPreviousEllipseAaxis = (int)(mCenterPoint.X - mBorderPoint.X);
    mEllipseBaxis = mPreviousEllipseBaxis = (int)(mCenterPoint.Y - mBorderPoint.Y);
}

void UOdysseyHUDEllipse::Init( FName iName, FVector2D iCenterPoint, int iEllipseAaxis, int iEllipseBaxis, FTransform2D iTransform )
{
    UOdysseyHUDElement::Init( iName, iTransform );
    mCenterPoint = mPreviousCenterPoint = iCenterPoint;
    mEllipseAaxis = mPreviousEllipseAaxis = iEllipseAaxis;
    mEllipseBaxis = mPreviousEllipseBaxis = iEllipseBaxis;
    mBorderPoint = mPreviousBorderPoint = FVector2D( iCenterPoint.X + iEllipseAaxis, iCenterPoint.Y + iEllipseBaxis);
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

    if ( mIsInvalid || mPreviousCenterPoint != mCenterPoint || mPreviousBorderPoint != mBorderPoint || mEllipseAaxis != mPreviousEllipseAaxis || mEllipseBaxis != mPreviousEllipseBaxis || mPreviousTransform != iTransform)
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
    FVector2D transformedBorderPoint = iTransform.TransformPoint(mBorderPoint);
    int transformedEllipseAaxis = FMath::Abs( transformedCenterPoint.X - transformedBorderPoint.X );
    int transformedEllipseBaxis = FMath::Abs( transformedCenterPoint.Y - transformedBorderPoint.Y );

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawEllipse(*(ioBlock), ::ULIS::FVec2I(transformedCenterPoint.X, transformedCenterPoint.Y), transformedEllipseAaxis, transformedEllipseBaxis, ::ULIS::FColor::RGBA8(0, 255, 0, 255));
    ctx.Finish();

    mPreviousEllipseAaxis = mEllipseAaxis;
    mPreviousEllipseBaxis = mEllipseBaxis;
    mPreviousCenterPoint = mCenterPoint;
    mPreviousBorderPoint = mBorderPoint;
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
    FVector2D previousTransformedBorderPoint = mPreviousTransform.TransformPoint(mPreviousBorderPoint);
    int previousTransformedEllipseAaxis = FMath::Abs( previousTransformedCenterPoint.X - previousTransformedBorderPoint.X);
    int previousTransformedEllipseBaxis = FMath::Abs( previousTransformedCenterPoint.Y - previousTransformedBorderPoint.Y);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawEllipse(*(ioBlock), ::ULIS::FVec2I( previousTransformedCenterPoint.X, previousTransformedCenterPoint.Y ), previousTransformedEllipseAaxis, previousTransformedEllipseBaxis, ::ULIS::FColor::RGBA8(0, 255, 0, 0));
    ctx.Finish();
}
