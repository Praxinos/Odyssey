// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDCircle.h"


void UOdysseyHUDCircle::Init( FName iName, FVector2D iCenterPoint, FVector2D iBorderPoint, FTransform2D iTransform )
{
    UOdysseyHUDElement::Init( iName, iTransform );
    mCenterPoint = mPreviousCenterPoint = iCenterPoint;
    mBorderPoint = mPreviousBorderPoint = iBorderPoint;
    mRadius = mPreviousRadius = (int)::ULIS::FMath::Dist( mCenterPoint.X, mCenterPoint.Y, mBorderPoint.X, mBorderPoint.Y );
}

void UOdysseyHUDCircle::Init( FName iName, FVector2D iCenterPoint, int iRadius, FTransform2D iTransform )
{
    UOdysseyHUDElement::Init( iName, iTransform );
    mCenterPoint = mPreviousCenterPoint = iCenterPoint;
    mRadius = mPreviousRadius = iRadius;
    mBorderPoint = FVector2D( mCenterPoint.X + iRadius, mCenterPoint.Y );
}

TSharedPtr<SWidget> UOdysseyHUDCircle::CreateWidget()
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

void UOdysseyHUDCircle::Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    if( !ioBlock )
        return;

    if ( mIsInvalid || mPreviousRadius != mRadius || mPreviousCenterPoint != mCenterPoint || mPreviousBorderPoint != mBorderPoint || mPreviousTransform != iTransform)
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
    int transformedRadius = (int)(iTransform.GetMatrix().GetScale().GetVector().X * mRadius);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawCircle(*(ioBlock), ::ULIS::FVec2I(transformedCenterPoint.X, transformedCenterPoint.Y), transformedRadius, ::ULIS::FColor::RGBA8(0, 255, 0, 255));
    ctx.Finish();

    mPreviousRadius = mRadius;
    mPreviousBorderPoint = mBorderPoint;
    mPreviousCenterPoint = mCenterPoint;
    mPreviousTransform = iTransform;

    ioBlock->Dirty();
}

void UOdysseyHUDCircle::Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    if (!ioBlock)
        return;
 
    //Erase the children of this HUDElement
    UOdysseyHUDElement::Erase(ioBlock, iTransform);

    FVector2D previousTransformedCenterPoint = mPreviousTransform.TransformPoint(mPreviousCenterPoint);
    FVector2D previousTransformedBorderPoint = mPreviousTransform.TransformPoint(mPreviousBorderPoint);
    int transformedPreviousRadius = (int)(iTransform.GetMatrix().GetScale().GetVector().X * mPreviousRadius);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawCircle(*(ioBlock), ::ULIS::FVec2I(previousTransformedCenterPoint.X, previousTransformedCenterPoint.Y), transformedPreviousRadius, ::ULIS::FColor::RGBA8(0, 255, 0, 0));
    ctx.Finish();
}
