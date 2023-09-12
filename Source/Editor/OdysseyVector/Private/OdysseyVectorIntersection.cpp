#include "OdysseyVectorIntersection.h"

FOdysseyVectorIntersection::~FOdysseyVectorIntersection()
{
}

FOdysseyVectorIntersection::FOdysseyVectorIntersection( bool iSelfIntersects
                                                      , FOdysseyVectorPath* iVertex0Path
                                                      , double iVertex0X
                                                      , double iVertex0Y
                                                      , double iVertex0T
                                                      , FOdysseyVectorPath* iVertex1Path
                                                      , double iVertex1X
                                                      , double iVertex1Y
                                                      , double iVertex1T )
    : mVertex { FOdysseyVectorVertexIntersection( this
                                                , iVertex0Path
                                                , iVertex0X
                                                , iVertex0Y
                                                , iVertex0T )
              , FOdysseyVectorVertexIntersection( this
                                                , iVertex1Path
                                                , iVertex1X
                                                , iVertex1Y
                                                , iVertex1T ) }
    , mSelfIntersects ( iSelfIntersects )
{
}

bool
FOdysseyVectorIntersection::HasVertex( FOdysseyVectorVertexIntersection* iVertex )
{
    return ( ( &mVertex[0] == iVertex ) || ( &mVertex[1] == iVertex ) );
}

FOdysseyVectorVertexIntersection*
FOdysseyVectorIntersection::GetOtherVertex( FOdysseyVectorVertexIntersection* iVertex )
{
    return ( iVertex == &mVertex[0] ) ? &mVertex[1] : &mVertex[0];
}

FOdysseyVectorVertexIntersection*
FOdysseyVectorIntersection::GetVertex( int iNum )
{
    return &mVertex[iNum];
}

void
FOdysseyVectorIntersection::BuildExplorationPairs( std::vector<FExplorationPair>& iExplorationPairsArray )
{
    mVertex[0].BuildExplorationPairs( iExplorationPairsArray );
    mVertex[1].BuildExplorationPairs( iExplorationPairsArray );
}

bool 
FOdysseyVectorIntersection::SelfIntersects()
{
    return mSelfIntersects;
}
