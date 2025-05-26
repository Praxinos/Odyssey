// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyVectorVertexIntersection.h"
#include "OdysseyVectorIntersection.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVector.h"

// for X-Junction Gaps
FOdysseyVectorVertexIntersection::XRecord::XRecord( double iX
                                                  , double iY
                                                  , FOdysseyVectorSegment* iSegment0
                                                  , double iSegment0T
                                                  , FOdysseyVectorSegment* iSegment1
                                                  , double iSegment1T )
        : segment0 ( iSegment0 )
        , segment1 ( iSegment1 )
        , segment0T( iSegment0T )
        , segment1T( iSegment1T )
        , x( iX )
        , y( iY )
{
    iSegment0->AddIntersectionSlot();
    iSegment1->AddIntersectionSlot();
}

// for T-Junction Gaps
FOdysseyVectorVertexIntersection::TRecord::TRecord( double iX
                                                  , double iY
                                                  , FOdysseyVectorSegment* iSegment
                                                  , double iSegmentT
                                                  , FOdysseyVectorVertex* iVertex )
        : segment( iSegment )
        , segmentT( iSegmentT )
        , x( iX )
        , y( iY )
        , vertex( iVertex )
{
    iSegment->AddIntersectionSlot();
}

FOdysseyVectorVertexIntersection::~FOdysseyVectorVertexIntersection()
{
}

FOdysseyVectorVertexIntersection::FOdysseyVectorVertexIntersection( FOdysseyVectorObject* iOwner
                                                                  , double iX
                                                                  , double iY
                                                                  , FOdysseyVectorSegment* iSegment
                                                                  , double iSegmentT
                                                                  , FOdysseyVectorVertex* iVertex )
    : FOdysseyVectorVertex ( iX, iY, 0.0f )
    , mIntersection { FOdysseyVectorIntersection( this, iSegmentT  )
                    , FOdysseyVectorIntersection( /* empty ctor */ ) }
    , mSegment { iSegment, nullptr }
{
    SetOwner( iOwner );

    // attach to segments. T-Junction, one segment only.
    Attach();

    if( iVertex )
    {
        iVertex->SetNearestVertex( this, 0.0f );
    }
}

FOdysseyVectorVertexIntersection::FOdysseyVectorVertexIntersection( FOdysseyVectorObject* iOwner
                                                                  , double iX
                                                                  , double iY
                                                                  , FOdysseyVectorSegment* iSegment0
                                                                  , double iSegment0T
                                                                  , FOdysseyVectorSegment* iSegment1
                                                                  , double iSegment1T )
    : FOdysseyVectorVertex ( iX, iY, 0.0f )
    , mIntersection { FOdysseyVectorIntersection( this, iSegment0T )
                    , FOdysseyVectorIntersection( this, iSegment1T ) }
    , mSegment { iSegment0,  iSegment1 }
{
    SetOwner( iOwner );

    // attach to segments
    Attach();
}

bool
FOdysseyVectorVertexIntersection::SelfIntersects()
{
    return mSegment[0] == mSegment[1];
}

void
FOdysseyVectorVertexIntersection::Attach()
{
    mSegment[0]->AddIntersection( &mIntersection[0] );

    if( mSegment[1] )
    {
        mSegment[1]->AddIntersection( &mIntersection[1] );
    }
}

void
FOdysseyVectorVertexIntersection::Detach()
{
    mSegment[0]->RemoveIntersection( &mIntersection[0] );

    if( mSegment[1] )
    {
        mSegment[1]->RemoveIntersection( &mIntersection[1] );
    }
}

double
FOdysseyVectorVertexIntersection::GetT( FOdysseyVectorSegment* iSegment )
{
    return ( mSegment[0] == iSegment ) ? mIntersection[0].GetSegmentT()
                                       : mIntersection[1].GetSegmentT();
}
