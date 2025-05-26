// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyVectorIntersection.h"

FOdysseyVectorIntersection::~FOdysseyVectorIntersection()
{
}

FOdysseyVectorIntersection::FOdysseyVectorIntersection()
{
}

FOdysseyVectorIntersection::FOdysseyVectorIntersection( FOdysseyVectorVertex* iVertex
                                                      , double iSegmentT )
    : mVertex( iVertex )
    , mSegmentT( iSegmentT )
{
}

FOdysseyVectorVertex*
FOdysseyVectorIntersection::GetVertex()
{
    return mVertex;
}

double
FOdysseyVectorIntersection::GetSegmentT()
{
    return mSegmentT;
}
