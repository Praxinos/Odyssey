#include "OdysseyVectorHandleBucket.h"

FOdysseyVectorHandleBucket::~FOdysseyVectorHandleBucket()
{
}

FOdysseyVectorHandleBucket::FOdysseyVectorHandleBucket( FOdysseyVectorBucket* iParentBucket )
    : FOdysseyVectorHandle()
    , mParentBucket ( iParentBucket )
{
}

FOdysseyVectorBucket*
FOdysseyVectorHandleBucket::GetParent()
{
    return mParentBucket;
}

void 
FOdysseyVectorHandleBucket::SetX( double iX )
{
    FOdysseyVectorPoint::SetX( iX );

    mParentBucket->Invalidate();
}

void 
FOdysseyVectorHandleBucket::SetY( double iY )
{
    FOdysseyVectorPoint::SetY( iY );

    mParentBucket->Invalidate();
}

void 
FOdysseyVectorHandleBucket::Set( double iX, double iY )
{
    FOdysseyVectorPoint::Set( iX, iY );

    mParentBucket->Invalidate();
}
