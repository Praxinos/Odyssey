#include "OdysseyVectorSection.h"

FOdysseyVectorSection::~FOdysseyVectorSection()
{
}

FOdysseyVectorSection::FOdysseyVectorSection()
{
    //Init( nullptr, nullptr, nullptr );
}

FOdysseyVectorSection::FOdysseyVectorSection( FOdysseyVectorSegment* iSegment
                                            , BLMatrix2D* iConversionMatrix
                                            , FOdysseyVectorVertex* iVertex0
                                            , FOdysseyVectorVertex* iVertex1 )
    : FOdysseyVectorSection()
{
    Init( iSegment, iConversionMatrix, iVertex0, iVertex1 );
}

void
FOdysseyVectorSection::Init( FOdysseyVectorSegment* iSegment
                           , BLMatrix2D* iConversionMatrix
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
    if( iSegment->HasBaseClass( FOdysseyVectorSegmentCubic::StaticClass() ) )
    {
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(iSegment);
        ::ULIS::FVec2D* segmentBezier = cubicSegment->GetBezier();
        BLPoint convertedPoint[4];

        if( fabs( t0 - t1 ) < 1.0f )
        {
            FOdysseyVector::BezierExtract( segmentBezier[0], segmentBezier[1], segmentBezier[2], segmentBezier[3]
                                         , t0, t1
                                         , mBezier[0], mBezier[1], mBezier[2], mBezier[3] );
        }
        else
        {
            mBezier[0] = segmentBezier[0];
            mBezier[1] = segmentBezier[1];
            mBezier[2] = segmentBezier[2];
            mBezier[3] = segmentBezier[3];
        }

        if( iConversionMatrix )
        {
            // convert to desired space (the paintgroup, normally)
            convertedPoint[0] = iConversionMatrix->mapPoint( mBezier[0].x, mBezier[0].y );
            convertedPoint[1] = iConversionMatrix->mapPoint( mBezier[1].x, mBezier[1].y );
            convertedPoint[2] = iConversionMatrix->mapPoint( mBezier[2].x, mBezier[2].y );
            convertedPoint[3] = iConversionMatrix->mapPoint( mBezier[3].x, mBezier[3].y );

            // copy values directly (we don't use ::ULIS::FVec2D constructor, to save some speed).
            mBezier[0].x = convertedPoint[0].x;
            mBezier[0].y = convertedPoint[0].y;

            mBezier[1].x = convertedPoint[1].x;
            mBezier[1].y = convertedPoint[1].y;

            mBezier[2].x = convertedPoint[2].x;
            mBezier[2].y = convertedPoint[2].y;

            mBezier[3].x = convertedPoint[3].x;
            mBezier[3].y = convertedPoint[3].y;
        }
    }
}

::ULIS::FVec2D&
FOdysseyVectorSection::GetVertexCoords( FOdysseyVectorVertex* iVertex )
{
    return ( iVertex == mVertex[0] ) ? mBezier[0] : mBezier[3];
}

::ULIS::FVec2D
FOdysseyVectorSection::GetPointAt( double t )
{
    ::ULIS::FVec2D pointAt = ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( mBezier[0]
                                                                                , mBezier[1]
                                                                                , mBezier[2]
                                                                                , mBezier[3]
                                                                                , t );

    return pointAt;
}

::ULIS::FVec2D
FOdysseyVectorSection::GetTangentAt( double t, bool iNormalize )
{
    ::ULIS::FVec2D tangent;

    // Special case when control point are located at endpoint (tangentequals 0 then).

    if( ( t > 0.0f ) && ( t < 1.0f ) )
    {
        tangent = ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( mBezier[0]
                                                                       , mBezier[1]
                                                                       , mBezier[2]
                                                                       , mBezier[3]
                                                                       , t );
    }

    if( t == 0.0f )
    {
        tangent = mBezier[1] - mBezier[0];

        if( tangent.Distance() == 0.0f ) 
        {
            tangent = mBezier[2] - mBezier[0];
        }
    }

    if( t == 1.0f )
    {
        tangent =  mBezier[3] - mBezier[2];

        if( tangent.Distance() == 0.0f )
        {
            tangent = mBezier[3] - mBezier[1];
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

uint32
FOdysseyVectorSection::GetCycleCount()
{
    return mCycleCount;
}

::ULIS::FVec2D
FOdysseyVectorSection::GetVectorFromVertex( FOdysseyVectorVertex* iVertex, bool iStraight, bool iNormalize )
{
    ::ULIS::FVec2D tangent = { 0.0f, 0.0f };

    if( /*( mSegment == nullptr ) || */ iStraight == true )
    {
        tangent =  ( iVertex == mVertex[0] ) ? mBezier[3] - mBezier[0]
                                             : mBezier[0] - mBezier[3];
    }
    else
    {
        if( mSegment->HasBaseClass( FOdysseyVectorSegmentCubic::StaticClass() ) )
        {
             tangent = ( iVertex == mVertex[0] ) ?  GetTangentAt( 0.0f, false )
                                                 : -GetTangentAt( 1.0f, false );
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

bool
FOdysseyVectorSection::IsErased()
{
    return ( mFlags & ERASED ) ? true : false;
}

void
FOdysseyVectorSection::SetErased( bool iErased )
{
    if( iErased )
    {
        mFlags |= ERASED;
    }
    else
    {
        mFlags &= (~ERASED);
    }
}

void
FOdysseyVectorSection::AddCycle( FOdysseyVectorCycle* iCycle )
{
    mCycle[mCycleCount++] = iCycle;
}

FOdysseyVectorCycle*
FOdysseyVectorSection::GetOtherCycle( FOdysseyVectorCycle* iCycle )
{
    return ( mCycle[0] == iCycle ) ? mCycle[1] : mCycle[0];
}

bool
FOdysseyVectorSection::HasCycle( FOdysseyVectorCycle* iCycle )
{
    return ( ( mCycle[0] == iCycle ) || ( mCycle[1] == iCycle ) );
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
