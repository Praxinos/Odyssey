#include "OdysseyVectorIntersection.h"

FOdysseyVectorIntersection::~FOdysseyVectorIntersection()
{
}

FOdysseyVectorIntersection::FOdysseyVectorIntersection( FOdysseyVectorVertex* iVertex
                                                      , double iSegmentT )
    : mVertex ( iVertex )
    , mSegmentT( iSegmentT )
{
}

void
FOdysseyVectorIntersection::BuildExplorationPairs( std::vector<FExplorationPair>& iExplorationPairsArray )
{
    mVertex->BuildExplorationPairs( iExplorationPairsArray, mSegmentT );
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
