#include "OdysseyVectorSection.h"

FOdysseyVectorSection::~FOdysseyVectorSection()
{
}

FOdysseyVectorSection::FOdysseyVectorSection( FOdysseyVectorSegment& iSegment
                                            , FOdysseyVectorPoint* iPoint0
                                            , FOdysseyVectorPoint* iPoint1 )
    : mSegment ( iSegment )
    , mPoint { iPoint0, iPoint1 }
{

}

FOdysseyVectorSegment&
FOdysseyVectorSection::GetSegment()
{
    return mSegment;
}

FOdysseyVectorPoint*
FOdysseyVectorSection::GetPoint( int iNum )
{
    return mPoint[iNum];
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
