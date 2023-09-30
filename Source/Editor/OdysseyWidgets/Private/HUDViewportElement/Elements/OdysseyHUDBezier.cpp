// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDBezier.h"


FOdysseyHUDBezier::~FOdysseyHUDBezier()
{

}

FOdysseyHUDBezier::FOdysseyHUDBezier(FName iName, FVector2D iStartPoint, FVector2D iEndPoint, FVector2D iControlPoint, FTransform2D iTransform /*= FTransform2D()*/) :
    FOdysseyHUDElement(iName, iTransform)
{
    mStartPoint = mPreviousStartPoint = iStartPoint;
    mEndPoint = mPreviousEndPoint = iEndPoint;
    mControlPoint = mPreviousControlPoint = iControlPoint;
}

void FOdysseyHUDBezier::Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    if( !ioBlock )
        return;

    if ( mIsInvalid
        || mPreviousStartPoint != mStartPoint
        || mPreviousEndPoint != mEndPoint
        || mPreviousControlPoint != mControlPoint
        || mPreviousTransform != iTransform
    )
    {
        Erase(ioBlock, iTransform);
        //Draw the children of this HUDElement
        FOdysseyHUDElement::Draw(ioBlock, iTransform);
    }
    else
    {
        //Draw the children of this HUDElement
        FOdysseyHUDElement::Draw(ioBlock, iTransform);
        return;
    }

    FVector2D transformedStartPoint = iTransform.TransformPoint(mStartPoint);
    FVector2D transformedEndPoint = iTransform.TransformPoint(mEndPoint);
    FVector2D transformedControlPoint = iTransform.TransformPoint(mControlPoint);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawQuadraticBezier(
        *(ioBlock),
        ::ULIS::FVec2I(transformedStartPoint.X,transformedStartPoint.Y),
        ::ULIS::FVec2I(transformedControlPoint.X,transformedControlPoint.Y),
        ::ULIS::FVec2I(transformedEndPoint.X, transformedEndPoint.Y),
        1.f,
        ::ULIS::FColor::RGBA8(0, 255, 0, 255)
    );
    if ( transformedStartPoint != transformedControlPoint )
    {
        ctx.DrawLine(
            *(ioBlock),
            ::ULIS::FVec2I(transformedStartPoint.X,transformedStartPoint.Y),
            ::ULIS::FVec2I(transformedControlPoint.X,transformedControlPoint.Y),
            ::ULIS::FColor::RGBA8(0, 255, 0, 95)
        );
        ctx.DrawLine(
            *(ioBlock),
            ::ULIS::FVec2I(transformedControlPoint.X,transformedControlPoint.Y),
            ::ULIS::FVec2I(transformedEndPoint.X, transformedEndPoint.Y),
            ::ULIS::FColor::RGBA8(0, 255, 0, 95)
        );
    }
    ctx.Finish();

    mPreviousEndPoint = mEndPoint;
    mPreviousStartPoint = mStartPoint;
    mPreviousControlPoint = mControlPoint;
    mPreviousTransform = iTransform;

    ioBlock->Dirty();
}

void FOdysseyHUDBezier::Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    if (!ioBlock)
        return;
 
    //Erase the children of this HUDElement
    FOdysseyHUDElement::Erase(ioBlock, iTransform);

    FVector2D transformedStartPoint = mPreviousTransform.TransformPoint(mPreviousStartPoint);
    FVector2D transformedEndPoint = mPreviousTransform.TransformPoint(mPreviousEndPoint);
    FVector2D transformedControlPoint = mPreviousTransform.TransformPoint(mPreviousControlPoint);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawQuadraticBezier(
        *(ioBlock),
        ::ULIS::FVec2I(transformedStartPoint.X, transformedStartPoint.Y),
        ::ULIS::FVec2I(transformedControlPoint.X, transformedControlPoint.Y),
        ::ULIS::FVec2I(transformedEndPoint.X, transformedEndPoint.Y),
        1.f,
        ::ULIS::FColor::RGBA8(0, 255, 0, 0)
    );
    if ( transformedStartPoint != transformedControlPoint )
    {
        ctx.DrawLine(
            *(ioBlock),
            ::ULIS::FVec2I(transformedStartPoint.X, transformedStartPoint.Y),
            ::ULIS::FVec2I(transformedControlPoint.X, transformedControlPoint.Y),
            ::ULIS::FColor::RGBA8(0, 255, 0, 0)
        );
        ctx.DrawLine(
            *(ioBlock),
            ::ULIS::FVec2I(transformedControlPoint.X, transformedControlPoint.Y),
            ::ULIS::FVec2I(transformedEndPoint.X, transformedEndPoint.Y),
            ::ULIS::FColor::RGBA8(0, 255, 0, 0)
        );
    }
    ctx.Finish();
}
