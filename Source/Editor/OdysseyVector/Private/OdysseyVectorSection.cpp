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
FOdysseyVectorSection::Block( UOdysseyVectorVertex* iVertex )
{
    uint32 blocked = ( iVertex == mVertex[0] ) ? FOdysseyVectorSection::BLOCKVERTEX0
                                               : FOdysseyVectorSection::BLOCKVERTEX1;

    mFlags |= blocked;
}

bool 
FOdysseyVectorSection::IsBlocked( UOdysseyVectorVertex* iVertex )
{
    uint32 blocked = ( iVertex == mVertex[0] ) ? FOdysseyVectorSection::BLOCKVERTEX0
                                               : FOdysseyVectorSection::BLOCKVERTEX1;

//UE_LOG(LogTemp, Warning, TEXT("FOdysseyVectorSection::IsBlocked edge:%d vertex:%d"), this, iVertex );

    return ( mFlags & blocked ) ? true : false;
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
