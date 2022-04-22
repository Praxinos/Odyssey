// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyHUDRectangle.h"


void UOdysseyHUDRectangle::Init( FName iName, FVector2D iTopLeftPoint, FVector2D iBottomRightPoint, FTransform2D iTransform )
{
    UOdysseyHUDElement::Init( iName, iTransform );
    mTopLeftPoint = mPreviousTopLeftPoint = iTopLeftPoint;
    mBottomRightPoint = mPreviousBottomRightPoint = iBottomRightPoint;
}

TSharedPtr<SWidget> UOdysseyHUDRectangle::CreateWidget()
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

void UOdysseyHUDRectangle::Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    if( !ioBlock )
        return;

    if ( mIsInvalid || mPreviousTopLeftPoint != mTopLeftPoint || mPreviousBottomRightPoint != mBottomRightPoint || mPreviousTransform != iTransform)
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

    FVector2D transformedTopLeftPoint = iTransform.TransformPoint(mTopLeftPoint);
    FVector2D transformedBottomRightPoint = iTransform.TransformPoint(mBottomRightPoint);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawRectangle(*(ioBlock), ::ULIS::FVec2I(transformedTopLeftPoint.X, transformedTopLeftPoint.Y), ::ULIS::FVec2I(transformedBottomRightPoint.X, transformedBottomRightPoint.Y), ::ULIS::FColor::RGBA8(0, 255, 0, 255));
    ctx.Finish();

    mPreviousBottomRightPoint = mBottomRightPoint;
    mPreviousTopLeftPoint = mTopLeftPoint;
    mPreviousTransform = iTransform;

    ioBlock->Dirty();
}

void UOdysseyHUDRectangle::Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    if (!ioBlock)
        return;
 
    //Erase the children of this HUDElement
    UOdysseyHUDElement::Erase(ioBlock, iTransform);

    FVector2D transformedTopLeftPoint = mPreviousTransform.TransformPoint(mPreviousTopLeftPoint);
    FVector2D transformedBottomRightPoint = mPreviousTransform.TransformPoint(mPreviousBottomRightPoint);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawRectangle(*(ioBlock), ::ULIS::FVec2I(transformedTopLeftPoint.X, transformedTopLeftPoint.Y), ::ULIS::FVec2I(transformedBottomRightPoint.X, transformedBottomRightPoint.Y), ::ULIS::FColor::RGBA8(0, 255, 0, 0));
    ctx.Finish();
}
