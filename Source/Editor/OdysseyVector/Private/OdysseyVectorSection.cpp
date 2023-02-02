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
FOdysseyVectorSection::SetMarched( bool iMarched )
{
    if( iMarched == true )
    {
        mFlags |= MARCHED;
    }
    else
    {
        mFlags &= (~MARCHED);
    }
}

bool
FOdysseyVectorSection::IsMarched()
{
    return ( mFlags & MARCHED ) ? true : false;
}

void 
FOdysseyVectorSection::Block()
{
    mFlags |= BLOCKED;
}

void
FOdysseyVectorSection::UnBlock()
{
    mFlags &= (~BLOCKED);
}

bool 
FOdysseyVectorSection::IsBlocked()
{
    return ( mFlags & BLOCKED ) ? true : false;
}

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
