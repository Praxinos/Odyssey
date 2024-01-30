#include "OdysseyVectorVertexIntersection.h"
#include "OdysseyVectorIntersection.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVector.h"

FOdysseyVectorVertexIntersection::~FOdysseyVectorVertexIntersection()
{
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

    iSegment0->AddIntersection( &mIntersection[0] );
    iSegment1->AddIntersection( &mIntersection[1] );

    // get ready for partnerization
    SetID( 0xFFFFFFFF );
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
    // T-Junction, one segment only.
    iSegment->AddIntersection( &mIntersection[0] );

    iVertex->SetNearestVertex( this, 0.0f );

    // get ready for partnerization
    SetID( 0xFFFFFFFF );
}

double
FOdysseyVectorVertexIntersection::GetT( FOdysseyVectorSegment* iSegment )
{
    return ( mSegment[0] == iSegment ) ? mIntersection[0].GetSegmentT()
                                       : mIntersection[1].GetSegmentT();
}
