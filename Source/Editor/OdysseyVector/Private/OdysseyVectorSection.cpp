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
