// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDRectangle.h"

FOdysseyHUDRectangle::~FOdysseyHUDRectangle()
{

}

FOdysseyHUDRectangle::FOdysseyHUDRectangle(FName iName, FVector2D iTopLeftPoint, FVector2D iBottomRightPoint, FTransform2D iTransform /*= FTransform2D() */) :
    FOdysseyHUDElement(iName, iTransform)
{
    mTopLeftPoint = mPreviousTopLeftPoint = iTopLeftPoint;
    mBottomRightPoint = mPreviousBottomRightPoint = iBottomRightPoint;
}

void FOdysseyHUDRectangle::Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    if( !ioBlock )
        return;

    if ( mIsInvalid || mPreviousTopLeftPoint != mTopLeftPoint || mPreviousBottomRightPoint != mBottomRightPoint || mPreviousTransform != iTransform)
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

void FOdysseyHUDRectangle::Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    if (!ioBlock)
        return;
 
    //Erase the children of this HUDElement
    FOdysseyHUDElement::Erase(ioBlock, iTransform);

    FVector2D transformedTopLeftPoint = mPreviousTransform.TransformPoint(mPreviousTopLeftPoint);
    FVector2D transformedBottomRightPoint = mPreviousTransform.TransformPoint(mPreviousBottomRightPoint);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawRectangle(*(ioBlock), ::ULIS::FVec2I(transformedTopLeftPoint.X, transformedTopLeftPoint.Y), ::ULIS::FVec2I(transformedBottomRightPoint.X, transformedBottomRightPoint.Y), ::ULIS::FColor::RGBA8(0, 255, 0, 0));
    ctx.Finish();
}
