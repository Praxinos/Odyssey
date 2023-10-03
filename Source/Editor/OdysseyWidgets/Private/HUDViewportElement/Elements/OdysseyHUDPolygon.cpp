// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDPolygon.h"

FOdysseyHUDPolygon::~FOdysseyHUDPolygon()
{

}

FOdysseyHUDPolygon::FOdysseyHUDPolygon(FName iName, TArray<FVector2D> iPoints, FTransform2D iTransform /*= FTransform2D() */) :
    FOdysseyHUDElement(iName, iTransform)
{
    mPoints = mPreviousPoints = iPoints;
}

void FOdysseyHUDPolygon::Draw( ::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/ )
{
    if( !ioBlock )
        return;

    if ( mIsInvalid || mPreviousPoints != mPoints || mPreviousTransform != iTransform)
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


    std::vector<::ULIS::FVec2I> vectors;
    for ( int i = 0; i < mPoints.Num(); i++ )
    {
        FVector2D transformedPoint = iTransform.TransformPoint(mPoints[i]);
        
        ::ULIS::FVec2I vec = ::ULIS::FVec2I( transformedPoint.X, transformedPoint.Y );
        vectors.push_back(vec);
    }
    UE_LOG(LogTemp, Display, TEXT("%d"), mPoints.Num());

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);

    if( vectors.size() == 2 )
    {
        ctx.DrawLine(*(ioBlock), vectors[0], vectors[1], ::ULIS::FColor::RGBA8(0, 255, 0, 255));
    }
    else
    {
        ctx.DrawPolygon(*(ioBlock), vectors, ::ULIS::FColor::RGBA8(0, 255, 0, 255));
    }
    ctx.Finish();


    mPreviousPoints = mPoints;
    mPreviousTransform = iTransform;

    ioBlock->Dirty();
}

void FOdysseyHUDPolygon::Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    if (!ioBlock)
        return;
 
    //Erase the children of this HUDElement
    FOdysseyHUDElement::Erase(ioBlock, iTransform);

    std::vector<::ULIS::FVec2I> vectors;
    for ( int i = 0; i < mPreviousPoints.Num(); i++ )
    {
        FVector2D transformedPoint = mPreviousTransform.TransformPoint(mPreviousPoints[i]);
        
        ::ULIS::FVec2I vec = ::ULIS::FVec2I( transformedPoint.X, transformedPoint.Y );
        vectors.push_back(vec);
    }

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    if (vectors.size() == 2)
    {
        ctx.DrawLine(*(ioBlock), vectors[0], vectors[1], ::ULIS::FColor::RGBA8(0, 0, 0, 0));
    }
    else
    {
        ctx.DrawPolygon(*(ioBlock), vectors, ::ULIS::FColor::RGBA8(0, 0, 0, 0));
    }

    ctx.Finish();
}

TArray<FVector2D>& FOdysseyHUDPolygon::GetPoints()
{
    return mPoints;
}
