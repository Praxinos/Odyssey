#include "OdysseyVectorIntersection.h"

FOdysseyVectorIntersection::~FOdysseyVectorIntersection()
{
}

FOdysseyVectorIntersection::FOdysseyVectorIntersection()
{
}

FOdysseyVectorIntersection::FOdysseyVectorIntersection( FOdysseyVectorVertexIntersection* iIntersectionVertex
                                                      , double iSegmentT )
    : mIntersectionVertex( iIntersectionVertex )
    , mSegmentT( iSegmentT )
{
}

FOdysseyVectorVertexIntersection*
FOdysseyVectorIntersection::GetIntersectionVertex()
{
    return mIntersectionVertex;
}

double
FOdysseyVectorIntersection::GetSegmentT()
{
    return mSegmentT;
}
