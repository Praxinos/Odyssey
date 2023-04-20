#include "OdysseyVectorVertexIntersection.h"
#include "OdysseyVectorPath.h"

FOdysseyVectorVertexIntersection::~FOdysseyVectorVertexIntersection()
{
}

FOdysseyVectorVertexIntersection::FOdysseyVectorVertexIntersection( double iX, double iY, bool iSelfIntersect )
    : FOdysseyVectorVertex ()
{
    mCoords.x = iX;
    mCoords.y = iY;
    mSelfIntersect = iSelfIntersect;
}

void
FOdysseyVectorVertexIntersection::BuildExplorationPairs()
{
    mExplorationPairs.reserve( mSectionList.size() );

    for( std::list<FOdysseyVectorSection*>::iterator it = mSectionList.begin(); it != mSectionList.end(); ++it )
    {
        FOdysseyVectorSection* returnSection = static_cast<FOdysseyVectorSection*>(*it);
        FOdysseyVectorSection* departSection = GetCycleNextSection( returnSection, 1.0f );

        mExplorationPairs.push_back( FExplorationPair( returnSection, departSection ) );
    }
}

std::vector<FExplorationPair>&
FOdysseyVectorVertexIntersection::GetExplorationPairs()
{
    return mExplorationPairs;
}

FOdysseyVectorSection*
FOdysseyVectorVertexIntersection::GetCycleNextSection( FOdysseyVectorSection* iLastSection, double iOrientation )
{
    ::ULIS::FVec2D lastSectionVector = -iLastSection->GetVectorFromVertex( this, false, false );

    for( std::list<FOdysseyVectorSection*>::iterator it = mSectionList.begin(); it != mSectionList.end(); ++it )
    {
        FOdysseyVectorSection* section = static_cast<FOdysseyVectorSection*>(*it);

        if(  ( mSelfIntersect == true  ) // a segment loops on itself.
        || ( ( mSelfIntersect == false ) && ( section->GetSegment() != iLastSection->GetSegment() ) ) )
        {
            ::ULIS::FVec2D sectionVector = section->GetVectorFromVertex( this, false, false );
UE_LOG(LogTemp, Warning, TEXT("cross:%f"), FOdysseyVector::Cross2D( lastSectionVector, sectionVector ) );
            if( ( FOdysseyVector::Cross2D( lastSectionVector, sectionVector ) * iOrientation > 0.0f ) )
            {
                return section;
            }
        }
    }
if( mSelfIntersect == true ) 
UE_LOG(LogTemp, Warning, TEXT("DaFook") );
    return /*GetOtherSection( iLastSection )*/nullptr;
}

double
FOdysseyVectorVertexIntersection::GetT( FOdysseyVectorSection* iSection )
{
    if ( auto search = mTMap.find(iSection); search != mTMap.end())
        return search->second.t;
    else
        return 0.0f;
}

void
FOdysseyVectorVertexIntersection::MapSection( FOdysseyVectorSection* iSection, double t )
{
    FIntersection intersect;

    //intersect.position = intersectAt;
    intersect.t        = t;

    mTMap.insert( std::make_pair( iSection, intersect ) );
}
