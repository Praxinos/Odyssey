#include "OdysseyVectorIntersection.h"

FOdysseyVectorIntersection::~FOdysseyVectorIntersection()
{
    delete mVertex[0];
    delete mVertex[1];
}

FOdysseyVectorIntersection::FOdysseyVectorIntersection( FOdysseyVectorVertexIntersection* iVertex0
                                                      , FOdysseyVectorVertexIntersection* iVertex1 )
    : mVertex { iVertex0, iVertex1 }
{
    iVertex0->SetIntersection( this );
    iVertex1->SetIntersection( this );
}

bool
FOdysseyVectorIntersection::HasVertex( FOdysseyVectorVertexIntersection* iVertex )
{
    return ( ( mVertex[0] == iVertex ) || ( mVertex[1] == iVertex ) );
}

FOdysseyVectorVertexIntersection*
FOdysseyVectorIntersection::GetOtherVertex( FOdysseyVectorVertexIntersection* iVertex )
{
    return ( iVertex == mVertex[0] ) ? mVertex[1] : mVertex[0];
}

FOdysseyVectorVertexIntersection*
FOdysseyVectorIntersection::GetVertex( int iNum )
{
    return mVertex[iNum];
}

void
FOdysseyVectorIntersection::BuildExplorationPairs( std::vector<FExplorationPair>& iExplorationPairsArray )
{
    mVertex[0]->BuildExplorationPairs( iExplorationPairsArray );
    mVertex[1]->BuildExplorationPairs( iExplorationPairsArray );
}
