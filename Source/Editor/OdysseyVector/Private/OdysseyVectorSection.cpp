#include "OdysseyVectorSection.h"

FOdysseyVectorSection::~FOdysseyVectorSection()
{
}

FOdysseyVectorSection::FOdysseyVectorSection()
{
    //Init( nullptr, nullptr, nullptr );
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

    double t0 = iVertex0->GetT( iSegment );
    double t1 = iVertex1->GetT( iSegment );

    mSegment = iSegment;
    mVertex[0] = iVertex0;
    mVertex[1] = iVertex1;
    mCycle[0] = nullptr;
    mCycle[1] = nullptr;
    mCycleCount = 0;
    mFlags = 0;

    // Get "sub-bezier" from t values. Will help us building the adjacent cycle and draw the section.
    // We indeed have to draw the section or else you can expect a small 1-pixel gap between cycles,
    // especially where strokes are transparent.
    if( iSegment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
    {
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(iSegment);
        ::ULIS::FVec2D& point0 = cubicSegment->GetVertex(0)->GetCoords();
        ::ULIS::FVec2D& point1 = cubicSegment->GetVertex(1)->GetCoords();
        ::ULIS::FVec2D& ctrlPoint0 = cubicSegment->GetHandle(0)->GetCoords();
        ::ULIS::FVec2D& ctrlPoint1 = cubicSegment->GetHandle(1)->GetCoords();

        if( fabs( t0 - t1 ) < 1.0f )
        {
            FOdysseyVector::BezierExtract( point0, ctrlPoint0, ctrlPoint1, point1
                                         , t0, t1
                                         , mBezier[0], mBezier[1], mBezier[2], mBezier[3] );
        }
        else
        {
            mBezier[0] = point0;
            mBezier[1] = ctrlPoint0;
            mBezier[2] = ctrlPoint1;
            mBezier[3] = point1;
        }
    }
}

uint32
FOdysseyVectorSection::GetFlags()
{
    return mFlags;
}

::ULIS::FVec2D*
FOdysseyVectorSection::GetBezier()
{
    return mBezier;
}

FOdysseyVectorCycle*
FOdysseyVectorSection::GetCycle( uint32 iCycleID )
{
    return mCycle[iCycleID];
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
