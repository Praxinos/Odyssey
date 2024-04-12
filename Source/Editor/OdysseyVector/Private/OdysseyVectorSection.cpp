#include "OdysseyVectorSection.h"
#include "OdysseyVector.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorObject.h"

FOdysseyVectorSection::~FOdysseyVectorSection()
{
}

FOdysseyVectorSection::FOdysseyVectorSection( FOdysseyVectorObject* iOwner // paintgroup
                                            , FOdysseyVectorSegment* iSegment
                                            , FOdysseyVectorVertex* iVertex0
                                            , FOdysseyVectorVertex* iVertex1
                                            , double iSectionT0
                                            , double iSectionT1
                                            , std::vector<FOdysseyVectorSection*>& oShortSectionArray
 )
{
    Init( iOwner, iSegment, iVertex0, iVertex1, iSectionT0, iSectionT1, oShortSectionArray );
}

double
FOdysseyVectorSection::GetLength()
{
    return mLength;
}

bool
FOdysseyVectorSection::IsValid()
{
    if( ( mLength == 0.0f )
     && ( ( mVertex[0]->GetSectionCount() > 2 )
       || ( mVertex[1]->GetSectionCount() > 2 ) ) )
    {
        return false;
    }

    return true;
}

FOdysseyVectorObject*
FOdysseyVectorSection::GetOwner()
{
    return mOwner;
}

void
FOdysseyVectorSection::Init( FOdysseyVectorObject* iOwner // usually the paintgroup
                           , FOdysseyVectorSegment* iSegment
                           , FOdysseyVectorVertex* iVertex0
                           , FOdysseyVectorVertex* iVertex1
                           , double iT0
                           , double iT1
                           , std::vector<FOdysseyVectorSection*>& oShortSectionArray )
{
    BLMatrix2D& ownerInverseWorldMatrix = iOwner->GetInverseWorldMatrix();
    mLength = fabs ( iT1 - iT0 ) * iSegment->GetLength();
    mSegment = iSegment;
    mVertex[0] = iVertex0;
    mVertex[1] = iVertex1;
    mCycle[0] = nullptr;
    mCycle[1] = nullptr;
    mCycleCount = 0;
    mFlags = 0;
    mOwner = iOwner;

    Link();

    // Get "sub-bezier" from t values. Will help us building the adjacent cycle and draw the section.
    // We indeed have to draw the section or else you can expect a small 1-pixel gap between cycles,
    // especially where strokes are transparent.
    if( iSegment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
    {
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(iSegment);
        ::ULIS::FVec2D* segmentBezier = cubicSegment->GetBezier();
        FOdysseyVectorObject* segmentOwner = iSegment->GetOwner();
        BLMatrix2D& segmentOwnerWorldMatrix = segmentOwner->GetWorldMatrix();
        BLPoint worldSegmentBezier[4] = { segmentOwnerWorldMatrix.mapPoint( segmentBezier[0].x, segmentBezier[0].y )
                                        , segmentOwnerWorldMatrix.mapPoint( segmentBezier[1].x, segmentBezier[1].y )
                                        , segmentOwnerWorldMatrix.mapPoint( segmentBezier[2].x, segmentBezier[2].y )
                                        , segmentOwnerWorldMatrix.mapPoint( segmentBezier[3].x, segmentBezier[3].y ) };

        BLPoint convertedPoint[4];

        if( fabs( iT0 - iT1 ) < 1.0f )
        {
            ::ULIS::FVec2D vertex0Worldcoords = iVertex0->GetWorldCoords();
            ::ULIS::FVec2D vertex1Worldcoords = iVertex1->GetWorldCoords();
            ::ULIS::FVec2D subBezier[4];

            FOdysseyVector::BezierExtract( ::ULIS::FVec2D( worldSegmentBezier[0].x, worldSegmentBezier[0].y )
                                         , ::ULIS::FVec2D( worldSegmentBezier[1].x, worldSegmentBezier[1].y )
                                         , ::ULIS::FVec2D( worldSegmentBezier[2].x, worldSegmentBezier[2].y )
                                         , ::ULIS::FVec2D( worldSegmentBezier[3].x, worldSegmentBezier[3].y )
                                         , iT0
                                         , iT1
                                         , subBezier[0]
                                         , subBezier[1]
                                         , subBezier[2]
                                         , subBezier[3] );
            // here we take the vertex coords and not the one we could retrieve from the 
            // subBezier because it might be inconsistent due to the value at T found from
            // performing linear intersection and not from a bezier-bezier intersection.
            // for this reason T might no be reliable to find the endpoints of our bezier.
            // we only use it for the handles. 
            mBezier[0] = vertex0Worldcoords;
            mBezier[3] = vertex1Worldcoords;

            mBezier[1] = mBezier[0] + ( subBezier[1] - subBezier[0] );
            mBezier[2] = mBezier[3] + ( subBezier[2] - subBezier[3] );
        }
        else
        {
            mBezier[0].x = worldSegmentBezier[0].x;
            mBezier[0].y = worldSegmentBezier[0].y;

            mBezier[1].x = worldSegmentBezier[1].x;
            mBezier[1].y = worldSegmentBezier[1].y;

            mBezier[2].x = worldSegmentBezier[2].x;
            mBezier[2].y = worldSegmentBezier[2].y;

            mBezier[3].x = worldSegmentBezier[3].x;
            mBezier[3].y = worldSegmentBezier[3].y;
        }

        // convert to desired space (the paintgroup, normally)
        convertedPoint[0] = ownerInverseWorldMatrix.mapPoint( mBezier[0].x, mBezier[0].y );
        convertedPoint[1] = ownerInverseWorldMatrix.mapPoint( mBezier[1].x, mBezier[1].y );
        convertedPoint[2] = ownerInverseWorldMatrix.mapPoint( mBezier[2].x, mBezier[2].y );
        convertedPoint[3] = ownerInverseWorldMatrix.mapPoint( mBezier[3].x, mBezier[3].y );

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

    if( iSegment->GetClass() == FOdysseyVectorSegmentCubicGap::StaticClass() )
    {
        FOdysseyVectorSegmentCubicGap* gapCubicSegment = static_cast<FOdysseyVectorSegmentCubicGap*>(iSegment);
        ::ULIS::FVec2D* segmentBezier = gapCubicSegment->GetBezier();

        // Gap segments already are in parent coordinates. Just copy the whole thing
        memcpy( mBezier, segmentBezier, sizeof( mBezier ) );
    }

    // check bezier validity. It can happen at very very small values
    // of T that the bezier has the same values at all controllers. We get rid of those
    // sections in FOdysseyVectorGroupPaint::SimplifyGraph()
    if( ( mBezier[0] == mBezier[1] )
     && ( mBezier[0] == mBezier[2] )
     && ( mBezier[0] == mBezier[3] ) )
    {
        mLength = 0.0f;

        oShortSectionArray.push_back( this );
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

        if( tangent.DistanceSquared() == 0.0f ) 
        {
            tangent = mBezier[2] - mBezier[0];
        }
    }

    if( t == 1.0f )
    {
        tangent =  mBezier[3] - mBezier[2];

        if( tangent.DistanceSquared() == 0.0f )
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
FOdysseyVectorSection::GetVectorFromVertex( uint32 iVertexIndex
                                          , bool iStraight
                                          , bool iNormalize )
{
    ::ULIS::FVec2D tangent = { 0.0f, 0.0f };

    if( /*( mSegment == nullptr ) || */ iStraight == true )
    {
        tangent =  ( iVertexIndex == 0 ) ? mBezier[3] - mBezier[0]
                                         : mBezier[0] - mBezier[3];
    }
    else
    {
        if( mSegment->HasBaseClass( FOdysseyVectorSegmentCubic::StaticClass() ) )
        {
             tangent = ( iVertexIndex == 0 ) ?  GetTangentAt( 0.0f, false )
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
FOdysseyVectorSection::UnBlock( uint32 iVertexIndex )
{
    uint32 blocked = ( iVertexIndex == 0 ) ? FOdysseyVectorSection::BLOCKVERTEX0
                                           : FOdysseyVectorSection::BLOCKVERTEX1;

    mFlags &= (~blocked);
}

void 
FOdysseyVectorSection::Block( uint32 iVertexIndex )
{
    uint32 blocked = ( iVertexIndex == 0 ) ? FOdysseyVectorSection::BLOCKVERTEX0
                                           : FOdysseyVectorSection::BLOCKVERTEX1;

    mFlags |= blocked;
}

bool 
FOdysseyVectorSection::IsBlocked( uint32 iVertexIndex )
{
    uint32 blocked = ( iVertexIndex == 0 ) ? FOdysseyVectorSection::BLOCKVERTEX0
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
    if( mVertex[0] != mVertex[1] )
    {
        mVertex[0]->AddSection( this );
        mVertex[1]->AddSection( this );
    }
    else
    {
        mVertex[0]->AddSection( this );
    }

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

// static
void
FOdysseyVectorSection::ListToArray( const std::list<FOdysseyVectorSection*>& iSectionList
                                  , std::vector<FOdysseyVectorSection*>& oSectionArray )
{
    oSectionArray.reserve( iSectionList.size() );

    for( FOdysseyVectorSection* section : iSectionList )
    {
        oSectionArray.push_back( section );
    }
}
