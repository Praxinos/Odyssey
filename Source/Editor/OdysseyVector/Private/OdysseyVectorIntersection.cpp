#include "OdysseyVectorIntersection.h"

FOdysseyVectorIntersection::~FOdysseyVectorIntersection()
{
}

FOdysseyVectorIntersection::FOdysseyVectorIntersection( double iSegmentT )
    : mSegmentT( iSegmentT )
{
}

void
FOdysseyVectorIntersection::SetIntersectionVertexID( uint32 iIntersectionVertexID )
{
    mIntersectionVertexID = iIntersectionVertexID;
}

uint32
FOdysseyVectorIntersection::GetIntersectionVertexID()
{
    return mIntersectionVertexID;
}

double
FOdysseyVectorIntersection::GetSegmentT()
{
    return mSegmentT;
}
