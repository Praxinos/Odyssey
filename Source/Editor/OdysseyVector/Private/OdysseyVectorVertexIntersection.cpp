#include "OdysseyVectorVertexIntersection.h"
#include "OdysseyVectorIntersection.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVector.h"

FOdysseyVectorVertexIntersection::~FOdysseyVectorVertexIntersection()
{
}

FOdysseyVectorVertexIntersection::FOdysseyVectorVertexIntersection( FOdysseyVectorGroupPaint* iOwner
                                                                  , double iX
                                                                  , double iY )
    : FOdysseyVectorVertex ( iX, iY, 0.0f )
    , mIntersection { nullptr, nullptr }
{
    SetOwner( iOwner );
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
