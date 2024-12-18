// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
