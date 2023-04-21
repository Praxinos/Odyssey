#include "OdysseyVectorVertexIntersection.h"
#include "OdysseyVectorIntersection.h"
#include "OdysseyVectorPath.h"

FOdysseyVectorVertexIntersection::~FOdysseyVectorVertexIntersection()
{
}

FOdysseyVectorVertexIntersection::FOdysseyVectorVertexIntersection( double iX
                                                                  , double iY
                                                                  , double iT )
    : FOdysseyVectorVertex ()
{
    mCoords.x = iX;
    mCoords.y = iY;
    mT = iT;
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

FOdysseyVectorSection*
FOdysseyVectorVertexIntersection::GetCycleNextSection( FOdysseyVectorSection* iLastSection, double iOrientation )
{
    ::ULIS::FVec2D lastSectionVector = -iLastSection->GetVectorFromVertex( this, false, false );
    FOdysseyVectorVertex* buddyVertex = GetPartner();
    std::list<FOdysseyVectorSection*>& sectionList = buddyVertex->GetSectionList();

    for( std::list<FOdysseyVectorSection*>::iterator it = sectionList.begin(); it != sectionList.end(); ++it )
    {
        FOdysseyVectorSection* section = static_cast<FOdysseyVectorSection*>(*it);
        ::ULIS::FVec2D sectionVector = section->GetVectorFromVertex( buddyVertex, false, false );

//UE_LOG(LogTemp, Warning, TEXT("cross:%f"), FOdysseyVector::Cross2D( lastSectionVector, sectionVector ) );
        if( ( FOdysseyVector::Cross2D( lastSectionVector, sectionVector ) * iOrientation > 0.0f ) )
        {
            return section;
        }
    }

    return /*GetOtherSection( iLastSection )*/nullptr;
}
