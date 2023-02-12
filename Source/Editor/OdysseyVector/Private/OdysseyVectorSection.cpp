#include "OdysseyVectorSection.h"

FOdysseyVectorSection::~FOdysseyVectorSection()
{
}

FOdysseyVectorSection::FOdysseyVectorSection( UOdysseyVectorSegment* iSegment
                                            , UOdysseyVectorVertex* iVertex0
                                            , UOdysseyVectorVertex* iVertex1 )
    : mSegment ( iSegment )
    , mVertex { iVertex0, iVertex1 }
    , mFlags( 0 )
    , mCycleCount( 0 )
{

}

void
FOdysseyVectorSection::SetInCycle( bool iInCycle )
{
    if( iInCycle == true )
    {
        mFlags |= INCYCLE;
    }
    else
    {
        mFlags &= (~INCYCLE);
    }
}

::ULIS::FVec2D
FOdysseyVectorSection::GetVectorFromVertex( UOdysseyVectorVertex* iVertex )
{
    ::ULIS::FVec2D tangent = { 0.0f, 0.0f };

    if( mSegment->GetClass() == UOdysseyVectorSegmentCubic::StaticClass() )
    {

        double T0 = mVertex[0]->GetT( *mSegment );
        double T1 = mVertex[1]->GetT( *mSegment );
        double deltaT = fabs( T1 - T0 ) * 0.1f;
        double sampleT = ( iVertex == mVertex[0] ) ? T0 + deltaT : T1 - deltaT;
        ::ULIS::FVec2D sample = mSegment->GetPointAt( sampleT );

        tangent.x = sample.x - iVertex->GetPosition(*mSegment).x;
        tangent.y = sample.y - iVertex->GetPosition(*mSegment).y;
/*
return mSegment->GetTangentAt( iVertex->GetT( *mSegment ) );
*/
    }

    return tangent;
}

bool
FOdysseyVectorSection::IsInCycle()
{
    return ( mFlags & INCYCLE ) ? true : false;
}

void
FOdysseyVectorSection::SetVisited( bool iVisited )
{
    if( iVisited == true )
    {
        mFlags |= VISITED;
    }
    else
    {
        mFlags &= (~VISITED);
    }
}

bool
FOdysseyVectorSection::IsVisited()
{
    return ( mFlags & VISITED ) ? true : false;
}

void
FOdysseyVectorSection::IncrementCycleCount()
{
    mCycleCount++;
}

void 
FOdysseyVectorSection::Block( UOdysseyVectorVertex* iVertex )
{
    uint32 blocked = ( iVertex == mVertex[0] ) ? FOdysseyVectorSection::BLOCKVERTEX0
                                               : FOdysseyVectorSection::BLOCKVERTEX1;

    mFlags |= blocked;
}

void
FOdysseyVectorSection::BlockAll()
{
    mFlags |=  ( FOdysseyVectorSection::BLOCKVERTEX0 |  FOdysseyVectorSection::BLOCKVERTEX1 );
}

bool 
FOdysseyVectorSection::IsBlocked( UOdysseyVectorVertex* iVertex )
{
    uint32 blocked = ( iVertex == mVertex[0] ) ? FOdysseyVectorSection::BLOCKVERTEX0
                                               : FOdysseyVectorSection::BLOCKVERTEX1;

    return ( mFlags & blocked ) ? true : false;
}

UOdysseyVectorVertex*
FOdysseyVectorSection::GetOtherVertex( UOdysseyVectorVertex* iVertex )
{
    return ( iVertex == mVertex[0] ) ? mVertex[1] : mVertex[0];
}

/*
bool
FOdysseyVectorSection::IsBlocked()
{
    return ( mCycleCount == 2 ) ? true : false;
}
*/

UOdysseyVectorSegment*
FOdysseyVectorSection::GetSegment()
{
    return mSegment;
}

UOdysseyVectorVertex*
FOdysseyVectorSection::GetVertex( int iNum )
{
    return mVertex[iNum];
}

void
FOdysseyVectorSection::AddLoop( UOdysseyVectorLoop* iLoop )
{
    mLoopList.push_back( iLoop );
}

void
FOdysseyVectorSection::RemoveLoop( UOdysseyVectorLoop* iLoop )
{
    mLoopList.remove( iLoop );
}

std::list<UOdysseyVectorLoop*>&
FOdysseyVectorSection::GetLoopList()
{
    return mLoopList;
}
