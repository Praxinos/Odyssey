#include "OdysseyVectorVertexIntersection.h"
#include "OdysseyVectorIntersection.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVector.h"

FOdysseyVectorVertexIntersection::~FOdysseyVectorVertexIntersection()
{
}

FOdysseyVectorVertexIntersection::FOdysseyVectorVertexIntersection( FOdysseyVectorObject* iOwner
                                                                  , uint32 iID
                                                                  , double iX
                                                                  , double iY
                                                                  , FOdysseyVectorIntersection* iIntersection0
                                                                  , FOdysseyVectorIntersection* iIntersection1 )
    : FOdysseyVectorVertex ( iX, iY, 0.0f )
    , mIntersection { iIntersection0, iIntersection1 }
{
    SetOwner( iOwner );

    if( mIntersection[0] )
        mIntersection[0]->SetIntersectionVertexID( iID );

    if( mIntersection[1] )
        mIntersection[1]->SetIntersectionVertexID( iID );
}

void
FOdysseyVectorVertexIntersection::SetIntersection( FOdysseyVectorIntersection* iIntersection0
                                                 , FOdysseyVectorIntersection* iIntersection1 )
{
    mIntersection[0] = iIntersection0;
    mIntersection[1] = iIntersection1;
}

FOdysseyVectorIntersection*
FOdysseyVectorVertexIntersection::GetIntersection( uint32 iIndex )
{
    return mIntersection[iIndex];
}
