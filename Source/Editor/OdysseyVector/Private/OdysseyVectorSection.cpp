#include "OdysseyVectorSection.h"

FOdysseyVectorSection::~FOdysseyVectorSection()
{
}

FOdysseyVectorSection::FOdysseyVectorSection( FOdysseyVectorSegment* iSegment
                                            , FOdysseyVectorVertex* iVertex0
                                            , FOdysseyVectorVertex* iVertex1 )
    : mSegment ( iSegment )
    , mVertex { iVertex0, iVertex1 }
    , mFlags( 0 )
    , mCycleCount( 0 )
{
}

::ULIS::FVec2D
FOdysseyVectorSection::GetVectorFromVertex( FOdysseyVectorVertex* iVertex, bool iStraight, bool iNormalize )
{
    ::ULIS::FVec2D tangent = { 0.0f, 0.0f };

    if( ( mSegment == nullptr ) || ( iStraight == true ) )
    {
        tangent =  ( iVertex == mVertex[0] ) ? mVertex[1]->GetCoords() - mVertex[0]->GetCoords()
                                             : mVertex[0]->GetCoords() - mVertex[1]->GetCoords();
    }
    else
    {
        if( mSegment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
        {
             tangent = ( iVertex == mVertex[0] ) ?  mSegment->GetTangentAt( iVertex->GetT( mSegment ) )
                                                 : -mSegment->GetTangentAt( iVertex->GetT( mSegment ) );

    /*
    return mSegment->GetTangentAt( iVertex->GetT( *mSegment ) );
    */
        }
    }

    if( iNormalize )
    {
        if( tangent.DistanceSquared() )
        {
            tangent.Normalize();
        }
    }

    return tangent;
}

void
FOdysseyVectorSection::IncrementCycleCount()
{
    mCycleCount++;
}

void 
FOdysseyVectorSection::UnBlock( FOdysseyVectorVertex* iVertex )
{
    uint32 blocked = ( iVertex == mVertex[0] ) ? FOdysseyVectorSection::BLOCKVERTEX0
                                               : FOdysseyVectorSection::BLOCKVERTEX1;

    mFlags &= (~blocked);
}

void 
FOdysseyVectorSection::Block( FOdysseyVectorVertex* iVertex )
{
    uint32 blocked = ( iVertex == mVertex[0] ) ? FOdysseyVectorSection::BLOCKVERTEX0
                                               : FOdysseyVectorSection::BLOCKVERTEX1;

    mFlags |= blocked;
}

bool 
FOdysseyVectorSection::IsBlocked( FOdysseyVectorVertex* iVertex )
{
    uint32 blocked = ( iVertex == mVertex[0] ) ? FOdysseyVectorSection::BLOCKVERTEX0
                                               : FOdysseyVectorSection::BLOCKVERTEX1;

    return ( mFlags & blocked ) ? true : false;
}

bool
FOdysseyVectorSection::IsLinked()
{
    return ( mFlags & UNLINKED ) ? false : true;
}

void
FOdysseyVectorSection::Link()
{
    mVertex[0]->AddSection( this );
    mVertex[1]->AddSection( this );

    mFlags &= (~UNLINKED);
}

void FOdysseyVectorSection::Unlink()
{
    mVertex[0]->RemoveSection( this );
    mVertex[1]->RemoveSection( this );

    mFlags |= UNLINKED;
}

FOdysseyVectorVertex*
FOdysseyVectorSection::GetOtherVertex( FOdysseyVectorVertex* iVertex )
{
    return ( iVertex == mVertex[0] ) ? mVertex[1] : mVertex[0];
}

FOdysseyVectorSegment*
FOdysseyVectorSection::GetSegment()
{
    return mSegment;
}

FOdysseyVectorVertex*
FOdysseyVectorSection::GetVertex( int iNum )
{
    return mVertex[iNum];
}
