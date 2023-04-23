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
FOdysseyVectorVertexIntersection::GetCycleNextSection( ::ULIS::FVec2D& iLastSectionVector, double iOrientation )
{
    //::ULIS::FVec2D lastSectionVector = -iLastSection->GetVectorFromVertex( this, false, false );

    for( std::list<FOdysseyVectorSection*>::iterator it = mSectionList.begin(); it != mSectionList.end(); ++it )
    {
        FOdysseyVectorSection* section = static_cast<FOdysseyVectorSection*>(*it);
        ::ULIS::FVec2D sectionVector = section->GetVectorFromVertex( this, false, false );

//UE_LOG(LogTemp, Warning, TEXT("cross:%f"), FOdysseyVector::Cross2D( lastSectionVector, sectionVector ) );
        if( ( FOdysseyVector::Cross2D( iLastSectionVector, sectionVector ) * iOrientation > 0.0f ) )
        {
            return section;
        }
    }

    return /*GetOtherSection( iLastSection )*/nullptr;
}
