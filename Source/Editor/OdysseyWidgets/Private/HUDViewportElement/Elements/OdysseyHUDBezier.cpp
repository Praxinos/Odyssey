// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyHUDBezier.h"


void UOdysseyHUDBezier::Init( FName iName, FVector2D iStartPoint, FVector2D iEndPoint, FVector2D iVertexPoint, FTransform2D iTransform )
{
    UOdysseyHUDElement::Init( iName, iTransform );
    mStartPoint = mPreviousStartPoint = iStartPoint;
    mEndPoint = mPreviousEndPoint = iEndPoint;
    mVertexPoint = mPreviousVertexPoint = iVertexPoint;
}

TSharedPtr<SWidget> UOdysseyHUDBezier::CreateWidget()
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

void UOdysseyHUDBezier::Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    if( !ioBlock )
        return;

    if ( mIsInvalid
        || mPreviousStartPoint != mStartPoint
        || mPreviousEndPoint != mEndPoint
        || mPreviousVertexPoint != mVertexPoint
        || mPreviousTransform != iTransform
    )
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
    FVector2D transformedEndPoint = iTransform.TransformPoint(mEndPoint);
    FVector2D transformedVertexPoint = iTransform.TransformPoint(mVertexPoint);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawQuadraticBezier(
        *(ioBlock),
        ::ULIS::FVec2I(transformedStartPoint.X,transformedStartPoint.Y),
        ::ULIS::FVec2I(transformedVertexPoint.X,transformedVertexPoint.Y),
        ::ULIS::FVec2I(transformedEndPoint.X, transformedEndPoint.Y),
        1.f,
        ::ULIS::FColor::RGBA8(0, 255, 0, 255)
    );
    if ( transformedStartPoint != transformedVertexPoint )
    {
        ctx.DrawLine(
            *(ioBlock),
            ::ULIS::FVec2I(transformedStartPoint.X,transformedStartPoint.Y),
            ::ULIS::FVec2I(transformedVertexPoint.X,transformedVertexPoint.Y),
            ::ULIS::FColor::RGBA8(0, 255, 0, 95)
        );
        ctx.DrawLine(
            *(ioBlock),
            ::ULIS::FVec2I(transformedVertexPoint.X,transformedVertexPoint.Y),
            ::ULIS::FVec2I(transformedEndPoint.X, transformedEndPoint.Y),
            ::ULIS::FColor::RGBA8(0, 255, 0, 95)
        );
    }
    ctx.Finish();

    mPreviousEndPoint = mEndPoint;
    mPreviousStartPoint = mStartPoint;
    mPreviousVertexPoint = mVertexPoint;
    mPreviousTransform = iTransform;

    ioBlock->Dirty();
}

void UOdysseyHUDBezier::Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    if (!ioBlock)
        return;
 
    //Erase the children of this HUDElement
    UOdysseyHUDElement::Erase(ioBlock, iTransform);

    FVector2D transformedStartPoint = mPreviousTransform.TransformPoint(mPreviousStartPoint);
    FVector2D transformedEndPoint = mPreviousTransform.TransformPoint(mPreviousEndPoint);
    FVector2D transformedVertexPoint = mPreviousTransform.TransformPoint(mPreviousVertexPoint);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawQuadraticBezier(
        *(ioBlock),
        ::ULIS::FVec2I(transformedStartPoint.X, transformedStartPoint.Y),
        ::ULIS::FVec2I(transformedVertexPoint.X, transformedVertexPoint.Y),
        ::ULIS::FVec2I(transformedEndPoint.X, transformedEndPoint.Y),
        1.f,
        ::ULIS::FColor::RGBA8(0, 255, 0, 0)
    );
    if ( transformedStartPoint != transformedVertexPoint )
    {
        ctx.DrawLine(
            *(ioBlock),
            ::ULIS::FVec2I(transformedStartPoint.X, transformedStartPoint.Y),
            ::ULIS::FVec2I(transformedVertexPoint.X, transformedVertexPoint.Y),
            ::ULIS::FColor::RGBA8(0, 255, 0, 0)
        );
        ctx.DrawLine(
            *(ioBlock),
            ::ULIS::FVec2I(transformedVertexPoint.X, transformedVertexPoint.Y),
            ::ULIS::FVec2I(transformedEndPoint.X, transformedEndPoint.Y),
            ::ULIS::FColor::RGBA8(0, 255, 0, 0)
        );
    }
    ctx.Finish();
}
