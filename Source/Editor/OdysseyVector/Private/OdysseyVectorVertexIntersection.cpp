#include "OdysseyVectorVertexIntersection.h"
#include "OdysseyVectorIntersection.h"
#include "OdysseyVectorPath.h"

FOdysseyVectorVertexIntersection::~FOdysseyVectorVertexIntersection()
{
}

FOdysseyVectorVertexIntersection::FOdysseyVectorVertexIntersection( FOdysseyVectorPath* iPath
                                                                  , bool iSelfIntersects
                                                                  , double iX
                                                                  , double iY
                                                                  , double iT )
    : FOdysseyVectorVertex ( iPath, iX, iY, 0.0f )
{
    mT = iT;
    mSelfIntersects = iSelfIntersects;
}

bool 
FOdysseyVectorVertexIntersection::SelfIntersects()
{
    return mSelfIntersects;
}

uint32
FOdysseyVectorVertexIntersection::GetSectionCount()
{
    return GetSectionList().size() + GetPartner()->GetSectionList().size();
}

void
FOdysseyVectorVertexIntersection::SetIntersection( FOdysseyVectorIntersection* iIntersection )
{
    mIntersection = iIntersection;
}

FOdysseyVectorIntersection*
FOdysseyVectorVertexIntersection::GetIntersection()
{
    return mIntersection;
}

double
FOdysseyVectorVertexIntersection::GetT( FOdysseyVectorSegment* iSegment )
{
    return mT;
}

FOdysseyVectorVertexIntersection*
FOdysseyVectorVertexIntersection::GetPartner()
{
    return mIntersection->GetOtherVertex( this );
}

void
FOdysseyVectorVertexIntersection::BuildExplorationPairs( std::vector<FExplorationPair>& iExplorationPairsArray )
{
    for( std::list<FOdysseyVectorSection*>::iterator it = mSectionList.begin(); it != mSectionList.end(); ++it )
    {
        FOdysseyVectorSection* returnSection = static_cast<FOdysseyVectorSection*>(*it);
        FOdysseyVectorVertexIntersection* partnerVertex = GetPartner();
        FOdysseyVectorSection* departSection = GetCycleNextSection( returnSection, 1.0f );

        iExplorationPairsArray.push_back( FExplorationPair( returnSection, partnerVertex, departSection ) );
    }
}

FOdysseyVectorSection*
FOdysseyVectorVertexIntersection::GetCycleNextSection( FOdysseyVectorSection* iLastSection, double iOrientation )
{
    ::ULIS::FVec2D lastSectionVector = -iLastSection->GetVectorFromVertex( this, false, false );
    FOdysseyVectorVertexIntersection* partnerVertex = GetPartner();
    std::list<FOdysseyVectorSection*>& sectionList = partnerVertex->GetSectionList();

    for( std::list<FOdysseyVectorSection*>::iterator it = sectionList.begin(); it != sectionList.end(); ++it )
    {
        FOdysseyVectorSection* section = static_cast<FOdysseyVectorSection*>(*it);
        ::ULIS::FVec2D sectionVector = section->GetVectorFromVertex( partnerVertex, false, false );

//UE_LOG(LogTemp, Warning, TEXT("cross:%f"), FOdysseyVector::Cross2D( lastSectionVector, sectionVector ) );
        if( ( FOdysseyVector::Cross2D( lastSectionVector, sectionVector ) * iOrientation > 0.0f ) )
        {
            return section;
        }
    }

    return /*GetOtherSection( iLastSection )*/nullptr;
}
