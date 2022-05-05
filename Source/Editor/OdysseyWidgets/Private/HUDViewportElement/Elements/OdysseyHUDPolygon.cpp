// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDPolygon.h"

void UOdysseyHUDPolygon::Init( FName iName, TArray<FVector2D> iPoints, FTransform2D iTransform )
{
    UOdysseyHUDElement::Init( iName, iTransform );
    mPoints = mPreviousPoints = iPoints;
}

TSharedPtr<SWidget> UOdysseyHUDPolygon::CreateWidget()
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

void UOdysseyHUDPolygon::Draw( ::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/ )
{
    if( !ioBlock )
        return;

    if ( mIsInvalid || mPreviousPoints != mPoints || mPreviousTransform != iTransform)
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


    std::vector<::ULIS::FVec2I> vectors;
    for ( int i = 0; i < mPoints.Num(); i++ )
    {
        FVector2D transformedPoint = iTransform.TransformPoint(mPoints[i]);
        
        ::ULIS::FVec2I vec = ::ULIS::FVec2I( transformedPoint.X, transformedPoint.Y );
        vectors.push_back(vec);
    }

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawPolygon(*(ioBlock), vectors, ::ULIS::FColor::RGBA8(0, 255, 0, 255));
    ctx.Finish();

    mPreviousPoints = mPoints;
    mPreviousTransform = iTransform;

    ioBlock->Dirty();
}

void UOdysseyHUDPolygon::Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    if (!ioBlock)
        return;
 
    //Erase the children of this HUDElement
    UOdysseyHUDElement::Erase(ioBlock, iTransform);

    std::vector<::ULIS::FVec2I> vectors;
    for ( int i = 0; i < mPoints.Num(); i++ )
    {
        FVector2D transformedPoint = iTransform.TransformPoint(mPoints[i]);
        
        ::ULIS::FVec2I vec = ::ULIS::FVec2I( transformedPoint.X, transformedPoint.Y );
        vectors.push_back(vec);
    }

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawPolygon(*(ioBlock), vectors, ::ULIS::FColor::RGBA8(0, 0, 0, 0));
    ctx.Finish();
}
