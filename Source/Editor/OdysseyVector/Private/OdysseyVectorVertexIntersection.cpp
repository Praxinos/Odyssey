#include "OdysseyVectorVertexIntersection.h"
#include "OdysseyVectorIntersection.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVector.h"

FOdysseyVectorVertexIntersection::~FOdysseyVectorVertexIntersection()
{
}

FOdysseyVectorVertexIntersection::FOdysseyVectorVertexIntersection( FOdysseyVectorIntersection* iIntersection
                                                                  , FOdysseyVectorPath* iPath
                                                                  , double iX
                                                                  , double iY
                                                                  , double iT )
    : FOdysseyVectorVertex ( iX, iY, 0.0f )
    , mIntersection( iIntersection )
    , mT( iT )
{
    SetPath( iPath );
}

uint32
FOdysseyVectorVertexIntersection::GetSectionCount()
{
    return GetSectionList().size() + GetPartner()->GetSectionList().size();
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

double
FOdysseyVectorVertexIntersection::GetT( FOdysseyVectorSection* iSection )
{
    return ( this == iSection->GetVertex(0) ) ? 0.0f : 1.0f;
}

FOdysseyVectorVertexIntersection*
FOdysseyVectorVertexIntersection::GetPartner()
{
    return mIntersection->GetOtherVertex( this );
}

void
FOdysseyVectorVertexIntersection::BuildExplorationPairs( std::vector<FExplorationPair>& iExplorationPairsArray )
{
    for( FOdysseyVectorSection* returnSection : mSectionList )
    {
        FOdysseyVectorVertexIntersection* partnerVertex = GetPartner();
        FOdysseyVectorSection* departSection = GetCycleNextSection( returnSection, 1.0f );

        iExplorationPairsArray.push_back( departSection ? FExplorationPair( returnSection, partnerVertex, departSection )
        // Note: here we also handle the case where we have a T junction, which happens when dealing
        // with gaps. In that case, we have to chose a depart section on the same segment, because
        // there might be no depart section on the other segment. That's a good place to handle that
        // because we also need to know the depart vertex, which may differ for intersection vertices
        // according to the situation. It's the partner vertex when we have a X junction but it is
        // this vertex when we have a T junction.
                                                        : FExplorationPair( returnSection, this         , GetOtherSection( returnSection, true ) ) );

    }
}

FOdysseyVectorSection*
FOdysseyVectorVertexIntersection::GetCycleNextSection( FOdysseyVectorSection* iLastSection
                                                     , double iOrientation )
{
    ::ULIS::FVec2D lastSectionVector = -iLastSection->GetVectorFromVertex( this, false, false );
    FOdysseyVectorVertexIntersection* partnerVertex = GetPartner();
    std::list<FOdysseyVectorSection*>& sectionList = partnerVertex->GetSectionList();

    for( FOdysseyVectorSection* section : sectionList )
    {
        ::ULIS::FVec2D sectionVector = section->GetVectorFromVertex( partnerVertex, false, false );

        if( ( FOdysseyVector::Cross2D( lastSectionVector, sectionVector ) * iOrientation >= 0.0f ) )
        {
            return section;
        }
    }

    // we return nullptr and not the other section on the same segment because there is no way here to
    // also return the leaving vertex, which in that case should be THIS vertex and not the partner vertex.
    // That's why we do this in the above BuildExplorationPairs() function.
    return nullptr;
}
