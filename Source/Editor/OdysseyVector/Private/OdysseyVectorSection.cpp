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
FOdysseyVectorSection::GetVectorFromVertex( FOdysseyVectorVertex* iVertex, bool iNormalize )
{
    ::ULIS::FVec2D tangent = { 0.0f, 0.0f };

    if( mSegment )
    {
        if( mSegment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
        {

            double T0 = mVertex[0]->GetT( mSegment );
            double T1 = mVertex[1]->GetT( mSegment );
            double deltaT = fabs( T1 - T0 ) * 0.1f;
            double sampleT = ( iVertex == mVertex[0] ) ? T0 + deltaT : T1 - deltaT;
            ::ULIS::FVec2D sample = mSegment->GetPointAt( sampleT );

            tangent.x = sample.x - iVertex->GetCoords().x;
            tangent.y = sample.y - iVertex->GetCoords().y;
    /*
    return mSegment->GetTangentAt( iVertex->GetT( *mSegment ) );
    */
        }
    }
    else
    {
        tangent =  ( iVertex == mVertex[0] ) ? mVertex[1]->GetCoords() - mVertex[0]->GetCoords()
                                             : mVertex[0]->GetCoords() - mVertex[1]->GetCoords();
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
