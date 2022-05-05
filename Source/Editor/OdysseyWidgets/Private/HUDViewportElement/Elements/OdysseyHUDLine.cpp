// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDLine.h"


void UOdysseyHUDLine::Init( FName iName, FVector2D iStartPoint, FVector2D iFinishPoint, FTransform2D iTransform )
{
    UOdysseyHUDElement::Init( iName, iTransform );
    mStartPoint = mPreviousStartPoint = iStartPoint;
    mFinishPoint = mPreviousFinishPoint = iFinishPoint;
}

TSharedPtr<SWidget> UOdysseyHUDLine::CreateWidget()
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

void UOdysseyHUDLine::Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    if( !ioBlock )
        return;

    if ( mIsInvalid || mPreviousStartPoint != mStartPoint || mPreviousFinishPoint != mFinishPoint || mPreviousTransform != iTransform)
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

    FVector2D transformedStartPoint = iTransform.TransformPoint(mStartPoint);
    FVector2D transformedFinishPoint = iTransform.TransformPoint(mFinishPoint);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawLine(*(ioBlock), ::ULIS::FVec2I(transformedStartPoint.X, transformedStartPoint.Y), ::ULIS::FVec2I(transformedFinishPoint.X, transformedFinishPoint.Y), ::ULIS::FColor::RGBA8(0, 255, 0, 255));
    ctx.Finish();

    mPreviousFinishPoint = mFinishPoint;
    mPreviousStartPoint = mStartPoint;
    mPreviousTransform = iTransform;

    ioBlock->Dirty();
}

void UOdysseyHUDLine::Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    if (!ioBlock)
        return;
 
    //Erase the children of this HUDElement
    UOdysseyHUDElement::Erase(ioBlock, iTransform);

    FVector2D transformedStartPoint = mPreviousTransform.TransformPoint(mPreviousStartPoint);
    FVector2D transformedFinishPoint = mPreviousTransform.TransformPoint(mPreviousFinishPoint);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawLine(*(ioBlock), ::ULIS::FVec2I(transformedStartPoint.X, transformedStartPoint.Y), ::ULIS::FVec2I(transformedFinishPoint.X, transformedFinishPoint.Y), ::ULIS::FColor::RGBA8(0, 255, 0, 0));
    ctx.Finish();
}
