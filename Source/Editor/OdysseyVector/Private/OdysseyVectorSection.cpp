#include "OdysseyVectorSection.h"

FOdysseyVectorSection::~FOdysseyVectorSection()
{
}

FOdysseyVectorSection::FOdysseyVectorSection()
{
    Init( nullptr, nullptr, nullptr );
}

FOdysseyVectorSection::FOdysseyVectorSection( FOdysseyVectorSegment* iSegment
                                            , FOdysseyVectorVertex* iVertex0
                                            , FOdysseyVectorVertex* iVertex1 )
    : FOdysseyVectorSection()
{
    Init( iSegment, iVertex0, iVertex1 );
}

void
FOdysseyVectorSection::Init( FOdysseyVectorSegment* iSegment
                           , FOdysseyVectorVertex* iVertex0
                           , FOdysseyVectorVertex* iVertex1 )
{
    mSegment = iSegment;
    mVertex[0] = iVertex0;
    mVertex[1] = iVertex1;
    mCycle[0] = nullptr;
    mCycle[1] = nullptr;
    mCycleCount = 0;
    mFlags = 0;
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
    return ( mFlags & LINKED ) ? true : false;
}

void
FOdysseyVectorSection::AddCycle( FOdysseyVectorCycle* iCycle )
{
    // check is mandatory. See GroupPaint_figure1.png for explanations
    if( mCycleCount < 2 )
    {
        mCycle[mCycleCount++] = iCycle;
    }
}

FOdysseyVectorCycle*
FOdysseyVectorSection::GetOtherCycle( FOdysseyVectorCycle* iCycle )
{
    return ( mCycle[0] == iCycle ) ? mCycle[1] : mCycle[0];
}

void
FOdysseyVectorSection::Link()
{
    mVertex[0]->AddSection( this );
    mVertex[1]->AddSection( this );

    mFlags |= LINKED;
}

void FOdysseyVectorSection::Unlink()
{
    mVertex[0]->RemoveSection( this );
    mVertex[1]->RemoveSection( this );

    mFlags &= (~LINKED);
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
