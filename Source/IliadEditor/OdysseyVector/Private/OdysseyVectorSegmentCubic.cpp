// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVector.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"

#include <algorithm> // for std::clamp

#define MINRECURSE 0
#define MAXRECURSE 7
#define MAXSUBLINE ( 1 << MAXRECURSE )

#ifndef M_PI
#define M_PI 3.141592f
#endif

static bool IntersectSegment( const ::ULIS::FVec2D& iLine0p0
                            , const ::ULIS::FVec2D& iLine0p1
                            , const ::ULIS::FVec2D& iLine1p0
                            , const ::ULIS::FVec2D& iLine1p1
                            , ::ULIS::FVec2D& oIntersection );

FOdysseyVectorSegmentCubic::~FOdysseyVectorSegmentCubic()
{
}

FOdysseyVectorSegmentCubic::FOdysseyVectorSegmentCubic( FOdysseyVectorObject* iOwner
                                                      , FOdysseyVectorVertex* iPoint0
                                                      , double iCtrlPoint0x
                                                      , double iCtrlPoint0y
                                                      , double iCtrlPoint1x
                                                      , double iCtrlPoint1y
                                                      , FOdysseyVectorVertex* iPoint1
                                                      , bool iNeedWidth )
    : FOdysseyVectorSegment( iOwner, iPoint0, iPoint1, iNeedWidth )
    , mCtrlPoint { FOdysseyVectorHandleSegment( this, iPoint0, 0.0f, 0.0f )
                 , FOdysseyVectorHandleSegment( this, iPoint1, 0.0f, 0.0f ) }
{
    Init ( iPoint0, iCtrlPoint0x, iCtrlPoint0y, iCtrlPoint1x, iCtrlPoint1y, iPoint1 );
}

FOdysseyVectorSegmentCubic::FOdysseyVectorSegmentCubic( FOdysseyVectorObject* iOwner
                                                      , FOdysseyVectorVertex* iPoint0
                                                      , FOdysseyVectorVertex* iPoint1
                                                      , bool iNeedWidth )
    : FOdysseyVectorSegment( iOwner, iPoint0, iPoint1, iNeedWidth )
    , mCtrlPoint { FOdysseyVectorHandleSegment( this, iPoint0, 0.0f, 0.0f )
                 , FOdysseyVectorHandleSegment( this, iPoint1, 0.0f, 0.0f ) }
{
    Init( iPoint0, iPoint1 );
}

::ULIS::FVec2D
FOdysseyVectorSegmentCubic::GetHandleVector( FOdysseyVectorVertex* iVertex, bool iNormalize )
{
    return GetHandleVector( ( mPoint[0] == iVertex ) ? 0 : 1, iNormalize );
}

::ULIS::FVec2D
FOdysseyVectorSegmentCubic::GetHandleVector( uint32 iHandleID, bool iNormalize )
{
    ::ULIS::FVec2D vec = GetHandle(iHandleID)->GetCoords() - GetVertex(iHandleID)->GetCoords();

    if( iNormalize )
    {
        if( vec.DistanceSquared() )
        {
            vec.Normalize();
        }
    }

    return vec;
}

void
FOdysseyVectorSegmentCubic::Init( FOdysseyVectorVertex* iPoint0
                                , double iCtrlPoint0x
                                , double iCtrlPoint0y
                                , double iCtrlPoint1x
                                , double iCtrlPoint1y
                                , FOdysseyVectorVertex* iPoint1 )
{
    bool lockStatus[2] = { iPoint0->IsLocked(), iPoint1->IsLocked() };

    mPoint[0] = iPoint0;
    mPoint[1] = iPoint1;

    // bypass lock to be able to set up handle position at file load
    iPoint0->SetLocked( false );
    iPoint1->SetLocked( false );

    mCtrlPoint[0].Set( iCtrlPoint0x, iCtrlPoint0y );
    mCtrlPoint[1].Set( iCtrlPoint1x, iCtrlPoint1y );

    // restore lock status
    iPoint0->SetLocked( lockStatus[0] );
    iPoint1->SetLocked( lockStatus[1] );

    mBBox.x = ::ULIS::FMath::Min4( iPoint0->GetX(), iCtrlPoint0x, iCtrlPoint1x, iPoint1->GetX() );
    mBBox.y = ::ULIS::FMath::Min4( iPoint0->GetY(), iCtrlPoint0y, iCtrlPoint1y, iPoint1->GetY() );
    mBBox.w = ::ULIS::FMath::Max4( iPoint0->GetX(), iCtrlPoint0x, iCtrlPoint1x, iPoint1->GetX() ) - mBBox.x;
    mBBox.h = ::ULIS::FMath::Max4( iPoint0->GetY(), iCtrlPoint0y, iCtrlPoint1y, iPoint1->GetY() ) - mBBox.y;

    Invalidate();
}

void
FOdysseyVectorSegmentCubic::Init( FOdysseyVectorVertex* iPoint0
                                , FOdysseyVectorVertex* iPoint1 )
{
    Init( iPoint0, iPoint0->GetX(), iPoint0->GetY(), iPoint1->GetX(), iPoint1->GetY(), iPoint1 );
}

void
FOdysseyVectorSegmentCubic::Smooth( double iLimitAngleInRadians )
{
    FOdysseyVectorVertex* vertex0 = static_cast<FOdysseyVectorVertex*>(GetVertex(0));
    FOdysseyVectorVertex* vertex1 = static_cast<FOdysseyVectorVertex*>(GetVertex(1));
    FOdysseyVectorHandleSegment* handle0 = static_cast<FOdysseyVectorHandleSegment*>(GetHandle(0));
    FOdysseyVectorHandleSegment* handle1 = static_cast<FOdysseyVectorHandleSegment*>(GetHandle(1));
    FOdysseyVectorSegmentCubic* neighbour0 = static_cast<FOdysseyVectorSegmentCubic*>(GetVertex(0)->GetOtherSegment( static_cast<FOdysseyVectorSegment*>(this) ));
    FOdysseyVectorSegmentCubic* neighbour1 = static_cast<FOdysseyVectorSegmentCubic*>(GetVertex(1)->GetOtherSegment( static_cast<FOdysseyVectorSegment*>(this) ));
    ::ULIS::FVec2D v0v1 = vertex1->GetCoords() - vertex0->GetCoords();
    double length = this->GetStraightDistance();

    if( neighbour0 )
    {
        uint32_t neighbour0VertexIndex = ( neighbour0->GetVertex(0) == vertex0 ) ? 0 : 1;
        ::ULIS::FVec2D& neighbour0Point = neighbour0->GetVertex(neighbour0VertexIndex)->GetCoords();
        ::ULIS::FVec2D& neighbour0Handle = neighbour0->GetHandle(neighbour0VertexIndex)->GetCoords();
        ::ULIS::FVec2D neighbour0Vec = neighbour0Handle - neighbour0Point;
        ::ULIS::FVec2D handleVec = handle0->GetCoords() - vertex0->GetCoords();

        if( neighbour0Vec.DistanceSquared() && handleVec.DistanceSquared() )
        {
            double angle;

            neighbour0Vec.Normalize();
            handleVec.Normalize();

            angle = acos( ULIS::FMath::Clamp<double>( -neighbour0Vec.DotProduct( handleVec ), -1.0f, 1.0f ) );

            if( fabs(angle) < iLimitAngleInRadians )
            {
                double distance = handleVec.Distance();

                handle0->Set( vertex0->GetX() - ( neighbour0Vec.x * distance )
                            , vertex0->GetY() - ( neighbour0Vec.y * distance ) );
            }
        }
    }

    if( neighbour1 )
    {
        uint32_t neighbour1VertexIndex = ( neighbour1->GetVertex(0) == vertex0 ) ? 0 : 1;
        ::ULIS::FVec2D& neighbour1Point = neighbour1->GetVertex(neighbour1VertexIndex)->GetCoords();
        ::ULIS::FVec2D& neighbour1Handle = neighbour1->GetHandle(neighbour1VertexIndex)->GetCoords();
        ::ULIS::FVec2D neighbour1Vec = neighbour1Handle - neighbour1Point;
        ::ULIS::FVec2D handleVec = handle1->GetCoords() - vertex1->GetCoords();

        if( neighbour1Vec.DistanceSquared() )
        {
            double angle;

            neighbour1Vec.Normalize();
            handleVec.Normalize();

            angle = acos( ULIS::FMath::Clamp<double>( -neighbour1Vec.DotProduct( handleVec ), -1.0f, 1.0f ) );

            if( fabs(angle) < iLimitAngleInRadians )
            {
                double distance = handleVec.Distance();

                handle1->Set( vertex1->GetX() - ( neighbour1Vec.x * distance )
                            , vertex1->GetY() - ( neighbour1Vec.y * distance ) );
            }
        }
    }
}

bool
FOdysseyVectorSegmentCubic::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorSegment::HasBaseClass( iBaseClassID );
}

::ULIS::FVec2D*
FOdysseyVectorSegmentCubic::GetBezier()
{
    return mBezier;
}

::ULIS::FVec2D
FOdysseyVectorSegmentCubic::GetPointAt( double t )
{
    ::ULIS::FVec2D& point0 = GetVertex(0)->GetCoords();
    ::ULIS::FVec2D& point1 = GetVertex(1)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = GetHandle(0)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = GetHandle(1)->GetCoords();
    ::ULIS::FVec2D pointAt = ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( point0
                                                                                , ctrlPoint0
                                                                                , ctrlPoint1
                                                                                , point1
                                                                                , t );

    return pointAt;
}

::ULIS::FVec2D
FOdysseyVectorSegmentCubic::GetVectorFromVertex( FOdysseyVectorVertex* iVertex, bool iNormalize )
{
    ::ULIS::FVec2D tangent = ( iVertex == mPoint[0] ) ?  GetTangentAt( 0.0f, iNormalize )
                                                      : -GetTangentAt( 1.0f, iNormalize );


    return tangent;
}

::ULIS::FVec2D
FOdysseyVectorSegmentCubic::GetTangentAt( double t, bool iNormalize )
{
    ::ULIS::FVec2D& point0 = GetVertex(0)->GetCoords();
    ::ULIS::FVec2D& point1 = GetVertex(1)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = GetHandle(0)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = GetHandle(1)->GetCoords();
    ::ULIS::FVec2D tangent;

    // Special case when control point are located at endpoint (tangentequals 0 then).

    if( ( t > 0.0f ) && ( t < 1.0f ) )
    {
        tangent = ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( point0
                                                                       , ctrlPoint0
                                                                       , ctrlPoint1
                                                                       , point1
                                                                       , t );
    }

    if( t == 0.0f )
    {
        tangent = ctrlPoint0 - point0;

        if( tangent.Distance() == 0.0f )
        {
            tangent = ctrlPoint1 - point0;
        }
    }

    if( t == 1.0f )
    {
        tangent =  point1 - ctrlPoint1;

        if( tangent.Distance() == 0.0f )
        {
            tangent = point1 - ctrlPoint0;
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
FOdysseyVectorSegmentCubic::ResetPolygonCache( )
{
    mFractionCache.clear();
}

// mask-based version of the picking process
bool
FOdysseyVectorSegmentCubic::Pick( const ::ULIS::FRectD& iMaskRect, uint8* iPixelData )
{
    BLMatrix2D& worldMatrix = GetOwner()->GetWorldMatrix();
    BLPoint pt[4] = { worldMatrix.mapPoint( mBezier[0].x, mBezier[0].y )
                    , worldMatrix.mapPoint( mBezier[1].x, mBezier[1].y )
                    , worldMatrix.mapPoint( mBezier[2].x, mBezier[2].y )
                    , worldMatrix.mapPoint( mBezier[3].x, mBezier[3].y ) };
    ::ULIS::FVec2D worldBezier[4] = { ::ULIS::FVec2D( pt[0].x, pt[0].y )
                                    , ::ULIS::FVec2D( pt[1].x, pt[1].y )
                                    , ::ULIS::FVec2D( pt[2].x, pt[2].y )
                                    , ::ULIS::FVec2D( pt[3].x, pt[3].y ) };

    return FOdysseyVector::PickBezier( worldBezier, iMaskRect, iPixelData );
}

bool
FOdysseyVectorSegmentCubic::Pick( double iLocalX
                                , double iLocalY
                                , double iRadius )
{
    for ( int i = 0; i < mFractionCache.size(); i++ )
    {
        double refQuantity = 0;
        bool collide = true;

        for ( int j = 0; j < 4; j++ )
        {
            int n = ( j + 1 ) % 4;
            ::ULIS::FVec2D vivn = { mFractionCache[i].polygon.point[n].x - mFractionCache[i].polygon.point[j].x
                                  , mFractionCache[i].polygon.point[n].y - mFractionCache[i].polygon.point[j].y };
            ::ULIS::FVec2D vivt = { iLocalX - mFractionCache[i].polygon.point[j].x
                                  , iLocalY - mFractionCache[i].polygon.point[j].y };
            // https://stackoverflow.com/questions/15490795/determine-if-a-2d-point-is-within-a-quadrilateral
            // Compute the quantity
            double quantity = (vivt.x) * (vivn.y) - (vivn.x) * (vivt.y);

            if( j == 0 )
            {
                refQuantity = quantity;
            }

            if ( refQuantity * quantity < 0.0f )
            {
                collide = false;

                break;
            }
        }

        if ( collide == true ) return true;
    }

    return false;
}

::ULIS::FVec2D FOdysseyVectorSegmentCubic::GetVectorAtEnd( bool iNormalize )
{
    ::ULIS::FVec2D& point0 = GetVertex(0)->GetCoords();
    ::ULIS::FVec2D& point1 = GetVertex(1)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = GetHandle(0)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = GetHandle(1)->GetCoords();
    ::ULIS::FVec2D vec = ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( point0
                                                                              , ctrlPoint0
                                                                              , ctrlPoint1
                                                                              , point1
                                                                              , 0.9999f );
/*
    ::ULIS::FVec2D vec = { GetPoint(1)->GetX() - GetHandle(1).GetX(),
                   GetPoint(1)->GetY() - GetHandle(1).GetY() };
*/
    if( iNormalize && vec.DistanceSquared() )
    {
        vec.Normalize();
    }

    return vec;
}

bool
FOdysseyVectorSegmentCubic::ProximityTest( double iLocalX, double iLocalY, double iDistanceTolerance, double &oSmallestDistance )
{
    ::ULIS::FVec2D& point0 = GetVertex(0)->GetCoords();
    ::ULIS::FVec2D& point1 = GetVertex(1)->GetCoords();
    ::ULIS::FVec2D pt = { iLocalX, iLocalY };
    double smallestDistance = DBL_MAX;
    double dist;
    double dx;
    double dy;

    for( uint32 i = 0; i < mFractionCache.size(); i++ )
    {
        ::ULIS::FVec2D& p0Coords = mFractionCache[i].point[0]->GetCoords();
        ::ULIS::FVec2D& p1Coords = mFractionCache[i].point[1]->GetCoords();
        double t = FOdysseyVector::DistanceToSegment( pt, p0Coords, p1Coords, dist );

        if( ( t >= 0.0f ) && ( t <= 1.0f ) )
        {
            if( dist < smallestDistance )
            {
                smallestDistance = dist;
            }
        }
    }

    // The test above works only if pt is projectable on the sub-segment.
    // We also should check the distance to the segment endpoints.
    // test first end-point of the segment
    dx = ( iLocalX - point0.x );
    dy = ( iLocalY - point0.y );

    if( dx && dy )
    {
        dist = sqrt( ( dx * dx ) + ( dy * dy ) );

        if( dist < smallestDistance )
        {
            smallestDistance = dist;
        }
    }

    // test second end-point of the segment
    dx = ( iLocalX - point1.x );
    dy = ( iLocalY - point1.y );

    if( dx && dy )
    {
        dist = sqrt( ( dx * dx ) + ( dy * dy ) );

        if( dist < smallestDistance )
        {
            smallestDistance = dist;
        }
    }

    if( smallestDistance < iDistanceTolerance )
    {
        return true;
    }

    return false;
}

::ULIS::FVec2D
FOdysseyVectorSegmentCubic::GetVectorAtStart( bool iNormalize )
{
    ::ULIS::FVec2D& point0 = GetVertex(0)->GetCoords();
    ::ULIS::FVec2D& point1 = GetVertex(1)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = GetHandle(0)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = GetHandle(1)->GetCoords();
    ::ULIS::FVec2D vec = ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( point0
                                                                              , ctrlPoint0
                                                                              , ctrlPoint1
                                                                              , point1
                                                                              , 0.0001f );
/*
    ::ULIS::FVec2D vec = { GetHandle(0).GetX() - GetPoint(0)->GetX(),
                   GetHandle(0).GetY() - GetPoint(0)->GetY() };
*/
    if( iNormalize && vec.DistanceSquared() )
    {
        vec.Normalize();
    }

    return vec;
}

FOdysseyVectorSegment*
FOdysseyVectorSegmentCubic::Sample( double iFromT
                                  , double iToT
                                  , std::vector<FOdysseyVectorVertex*>& oNewVertexArray )
{
    ::ULIS::FVec2D& point0 = mPoint[0]->GetCoords();
    ::ULIS::FVec2D& point1 = mPoint[1]->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = mCtrlPoint[0].GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = mCtrlPoint[1].GetCoords();
    ::ULIS::FVec2D pointAt0 = GetPointAt( iFromT );
    ::ULIS::FVec2D pointAt1 = GetPointAt( iToT );
    double radius0 = GetVertex(0)->GetRadius();
    double radius1 = GetVertex(1)->GetRadius();
    double deltaRadius = ( radius1 - radius0 );
    double fromRadius = radius0 + ( deltaRadius * iFromT );
    double toRadius = radius0 + ( deltaRadius * iToT );
    FOdysseyVectorVertex* vertex0 = ( iFromT == 0.0f ) ? static_cast<FOdysseyVectorVertex*>(mPoint[0]) : new FOdysseyVectorVertex( pointAt0.x, pointAt0.y, fromRadius );
    FOdysseyVectorVertex* vertex1 = ( iToT   == 1.0f ) ? static_cast<FOdysseyVectorVertex*>(mPoint[1]) : new FOdysseyVectorVertex( pointAt1.x, pointAt1.y, toRadius   );
    FOdysseyVectorSegmentCubic* sampleSegment = new FOdysseyVectorSegmentCubic( mOwner, vertex0, vertex1, true );
    ::ULIS::FVec2D& sampleCtrlPoint0 = sampleSegment->GetHandle(0)->GetCoords();
    ::ULIS::FVec2D& sampleCtrlPoint1 = sampleSegment->GetHandle(1)->GetCoords();
    ::ULIS::FVec2D& samplePoint0 = sampleSegment->GetVertex(0)->GetCoords();
    ::ULIS::FVec2D& samplePoint1 = sampleSegment->GetVertex(1)->GetCoords();

    if( vertex0 != static_cast<FOdysseyVectorVertex*>( mPoint[0]) )
    {
        oNewVertexArray.push_back( vertex0 );
    }

    if( vertex1 != static_cast<FOdysseyVectorVertex*>( mPoint[1]) )
    {
        oNewVertexArray.push_back( vertex1 );
    }

    FOdysseyVector::BezierExtract( point0, ctrlPoint0, ctrlPoint1, point1
                                 , iFromT, iToT
                                 , samplePoint0, sampleCtrlPoint0, sampleCtrlPoint1, samplePoint1 );

    return sampleSegment;
}

FOdysseyVectorHandleSegment*
FOdysseyVectorSegmentCubic::GetHandle( int iCtrlPointNum )
{
    return &mCtrlPoint[iCtrlPointNum];
}

FOdysseyVectorHandleSegment*
FOdysseyVectorSegmentCubic::GetHandle( FOdysseyVectorVertex* iAssociatedVertex )
{
    return ( static_cast<FOdysseyVectorPoint*>(iAssociatedVertex) == mPoint[0] ) ? &mCtrlPoint[0] : &mCtrlPoint[1];
}

void
FOdysseyVectorSegmentCubic::Split( const ::ULIS::FVec2D& iPoint
                                 , double iPoinT
                                 , std::vector<FOdysseyVectorVertex*>& oNewVertexArray
                                 , std::vector<FOdysseyVectorSegment*>& oNewSegmentArray )
{
    FOdysseyVectorVertex* newVertex = new FOdysseyVectorVertex( iPoint.x
                                                              , iPoint.y
                                                              , ( GetVertex(0)->GetRadius()
                                                                + GetVertex(1)->GetRadius() ) * 0.5f );
    ::ULIS::FVec2D tmpBezier0[4] = { mBezier[0], mBezier[1], mBezier[2], mBezier[3] };
    ::ULIS::FVec2D tmpBezier1[4] = { mBezier[0], mBezier[1], mBezier[2], mBezier[3] };
    FOdysseyVectorSegmentCubic* newCubicSegment[2];

    newVertex->SetHandleAligned( true );

    ::ULIS::CubicBezierSplitAtParameter<::ULIS::FVec2D>( &tmpBezier0[0]
                                                       , &tmpBezier0[1]
                                                       , &tmpBezier0[2]
                                                       , &tmpBezier0[3]
                                                       , iPoinT );

    // here we take the vertex coords and not the one we could retrieve from the
    // subBezier because it might be inconsistent due to the value at T found from
    // performing linear intersection and not from a bezier-bezier intersection.
    // for this reason T might no be reliable to find the endpoints of our bezier.
    // we only use it for the handles.
    newCubicSegment[0] = new FOdysseyVectorSegmentCubic( mOwner
                                                       , GetVertex(0)
                                                       , mBezier[0].x + ( tmpBezier0[1].x - tmpBezier0[0].x )
                                                       , mBezier[0].y + ( tmpBezier0[1].y - tmpBezier0[0].y )
                                                       , iPoint.x     + ( tmpBezier0[2].x - tmpBezier0[3].x )
                                                       , iPoint.y     + ( tmpBezier0[2].y - tmpBezier0[3].y )
                                                       , newVertex
                                                       , true );

    ::ULIS::CubicBezierInverseSplitAtParameter<::ULIS::FVec2D>( &tmpBezier1[0]
                                                              , &tmpBezier1[1]
                                                              , &tmpBezier1[2]
                                                              , &tmpBezier1[3]
                                                              , iPoinT );

    // here we take the vertex coords and not the one we could retrieve from the
    // subBezier because it might be inconsistent due to the value at T found from
    // performing linear intersection and not from a bezier-bezier intersection.
    // for this reason T might no be reliable to find the endpoints of our bezier.
    // we only use it for the handles.
    newCubicSegment[1] = new FOdysseyVectorSegmentCubic( mOwner
                                                       , newVertex
                                                       , iPoint.x     + ( tmpBezier1[1].x - tmpBezier1[0].x )
                                                       , iPoint.y     + ( tmpBezier1[1].y - tmpBezier1[0].y )
                                                       , mBezier[3].x + ( tmpBezier1[2].x - tmpBezier1[3].x )
                                                       , mBezier[3].y + ( tmpBezier1[2].y - tmpBezier1[3].y )
                                                       , GetVertex(1)
                                                       , true );

    oNewVertexArray.push_back( newVertex );

    oNewSegmentArray.push_back( newCubicSegment[0] );
    oNewSegmentArray.push_back( newCubicSegment[1] );
}

bool
FOdysseyVectorSegmentCubic::Cut( const ::ULIS::FVec2D& linePoint0
                               , const ::ULIS::FVec2D& linePoint1
                               , std::vector<FOdysseyVectorVertex*>& oNewVertexArray
                               , std::vector<FOdysseyVectorSegment*>& oNewSegmentArray )
{
    FOdysseyVectorVertex* vertex0 = GetVertex(0);
    FOdysseyVectorVertex* vertex1 = GetVertex(1);
    ::ULIS::FVec2D& point0 = GetVertex(0)->GetCoords();
    ::ULIS::FVec2D& point1 = GetVertex(1)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = GetHandle(0)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = GetHandle(1)->GetCoords();
    // we'll have 3 intersections at most and 2 points at tips.
    FOdysseyVectorVertex* pointChain[5] = { vertex0, nullptr, nullptr, nullptr, nullptr };
    double tChain[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    uint32 pointCount = 1;
    ::ULIS::FVec2D ctrlPoint0Vector = GetVectorAtStart( true );
    ::ULIS::FVec2D ctrlPoint1Vector = GetVectorAtEnd( true );
    double difRadius = vertex1->GetRadius() - vertex0->GetRadius();

    for( int i = 0; i < mFractionCache.size(); i++ )
    {
        FOdysseyVectorFraction* fraction = &mFractionCache[i];
        ::ULIS::FVec2D& p0Coords = fraction->point[0]->GetCoords();
        ::ULIS::FVec2D& p1Coords = fraction->point[1]->GetCoords();
        double polySubT, interPolySubT;

        if ( FOdysseyVector::IntersectSegment( p0Coords
                                             , p1Coords
                                             , linePoint0
                                             , linePoint1
                                             , &polySubT
                                             , &interPolySubT ) )
        {
            double segmentT = fraction->fromT + ( polySubT * ( fraction->toT - fraction->fromT ) );
            // Commented out: unprecise due to the linear nature of a fraction
            // compared to its "parent" bezier curve.
            //::ULIS::FVec2D pointAt = ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( point0
            //                                                                            , ctrlPoint0
            //                                                                            , ctrlPoint1
            //                                                                            , point1
            //                                                                            , segmentT );
            ::ULIS::FVec2D pointAt = p0Coords + ( ( p1Coords - p0Coords ) * polySubT );
            FOdysseyVectorVertex* newCubicPoint = new FOdysseyVectorVertex( pointAt.x, pointAt.y, 0.0f );

            newCubicPoint->SetRadius( vertex0->GetRadius() + ( difRadius * segmentT ) );

            tChain[pointCount] = segmentT;
            pointChain[pointCount] = newCubicPoint;

            pointCount++;
        }
    }

    if ( pointCount > 1 )
    {
        tChain[pointCount] = 1.0f;
        pointChain[pointCount] = static_cast<FOdysseyVectorVertex*>(mPoint[1]);

        for( uint32 i = 1; i < pointCount; i++ )
        {
            oNewVertexArray.push_back( pointChain[i] );
        }

        for( uint32 i = 0; i < pointCount; i++ )
        {
            uint32 n = i + 1;
            FOdysseyVectorSegmentCubic* newSegment = new FOdysseyVectorSegmentCubic( mOwner
                                                                                   , static_cast<FOdysseyVectorVertex*>(pointChain[i])
                                                                                   , static_cast<FOdysseyVectorVertex*>(pointChain[n])
                                                                                   , true );

            ::ULIS::FVec2D& newSegmentCtrlPoint0 = newSegment->GetHandle(0)->GetCoords();
            ::ULIS::FVec2D& newSegmentCtrlPoint1 = newSegment->GetHandle(1)->GetCoords();
            // we ignore the coords of the end points because the parametric value "t"
            // that we obtained when intersecting fractions is not accurate enough to be used
            // to position the points. Plus they were already positionned in the above step
            // when being created using "new FOdysseyVectorVertex"
            ::ULIS::FVec2D ignoredSegmentPoint0;
            ::ULIS::FVec2D ignoredSegmentPoint1;

            FOdysseyVector::BezierExtract( point0
                                         , ctrlPoint0
                                         , ctrlPoint1
                                         , point1
                                         , tChain[i]
                                         , tChain[n]
                                         , ignoredSegmentPoint0
                                         , newSegmentCtrlPoint0
                                         , newSegmentCtrlPoint1
                                         , ignoredSegmentPoint1 );

            oNewSegmentArray.push_back( newSegment );
        }

        return true;
    }

    return false;
}

void
FOdysseyVectorSegmentCubic::DrawStructure( BLContext* iBLContext
                                         , FOdysseyVectorObject* iParentObject
                                         , bool iWorld )
{
    BLMatrix2D& worldMatrix = iParentObject->GetWorldMatrix();
    BLPoint point0 = iWorld ? worldMatrix.mapPoint( mBezier[0].x, mBezier[0].y ) : BLPoint( mBezier[0].x, mBezier[0].y );
    BLPoint point1 = iWorld ? worldMatrix.mapPoint( mBezier[3].x, mBezier[3].y ) : BLPoint( mBezier[3].x, mBezier[3].y );
    BLPoint handlePoint0 = iWorld ? worldMatrix.mapPoint( mBezier[1].x, mBezier[1].y ) : BLPoint( mBezier[1].x, mBezier[1].y );
    BLPoint handlePoint1 = iWorld ? worldMatrix.mapPoint( mBezier[2].x, mBezier[2].y ) : BLPoint( mBezier[2].x, mBezier[2].y );
    BLPath path;

    path.moveTo( point0 );
    path.cubicTo( handlePoint0
                , handlePoint1
                , point1 );

    iBLContext->strokePath( path );
}

void
FOdysseyVectorSegmentCubic::Draw( BLContext* iBLContext, FOdysseyVectorEngine* iVectorEngine )
{
    std::vector<FOdysseyVectorBezierFragment>& offsetCurve0FragmentArray = mOffsetCurve[0].GetBezierFragmentArray();
    std::vector<FOdysseyVectorBezierFragment>& offsetCurve1FragmentArray = mOffsetCurve[1].GetBezierFragmentArray();
    // NOTE: Might not be super fast to call this for each segment
/*
    blctx->fillPath( mBLPath );
*/

    DrawFractionCache( iBLContext, iVectorEngine );
/*
    for( int i = 0; i < offsetCurve0FragmentArray.size(); i++ )
    {
        BLPath offsetPath;
        BLPath handlePath;

        offsetPath.moveTo ( offsetCurve0FragmentArray[i].bezier[0].x, offsetCurve0FragmentArray[i].bezier[0].y );
        offsetPath.cubicTo( offsetCurve0FragmentArray[i].bezier[1].x, offsetCurve0FragmentArray[i].bezier[1].y
                          , offsetCurve0FragmentArray[i].bezier[2].x, offsetCurve0FragmentArray[i].bezier[2].y
                          , offsetCurve0FragmentArray[i].bezier[3].x, offsetCurve0FragmentArray[i].bezier[3].y );

        iBLContext->setStrokeWidth( 0.5f );
        iBLContext->setStrokeStyle( BLRgba32( 255, 255, 0, 255 ) );
        iBLContext->strokePath( offsetPath );
        handlePath.moveTo ( offsetCurve0FragmentArray[i].bezier[0].x, offsetCurve0FragmentArray[i].bezier[0].y );
        handlePath.lineTo ( offsetCurve0FragmentArray[i].bezier[1].x, offsetCurve0FragmentArray[i].bezier[1].y );

        iBLContext->setStrokeWidth( 0.5f );
        iBLContext->setStrokeStyle( BLRgba32( 255, 0, 0, 255 ) );
        iBLContext->strokePath( handlePath );

        handlePath.moveTo ( offsetCurve0FragmentArray[i].bezier[3].x, offsetCurve0FragmentArray[i].bezier[3].y );
        handlePath.lineTo ( offsetCurve0FragmentArray[i].bezier[2].x, offsetCurve0FragmentArray[i].bezier[2].y );

        iBLContext->setStrokeWidth( 0.5f );
        iBLContext->setStrokeStyle( BLRgba32( 255, 0, 0, 255 ) );
        iBLContext->strokePath( handlePath );
    }

    for( int i = 0; i < offsetCurve1FragmentArray.size(); i++ )
    {
        BLPath offsetPath;
        BLPath handlePath;

        offsetPath.moveTo ( offsetCurve1FragmentArray[i].bezier[0].x, offsetCurve1FragmentArray[i].bezier[0].y );
        offsetPath.cubicTo( offsetCurve1FragmentArray[i].bezier[1].x, offsetCurve1FragmentArray[i].bezier[1].y
                          , offsetCurve1FragmentArray[i].bezier[2].x, offsetCurve1FragmentArray[i].bezier[2].y
                          , offsetCurve1FragmentArray[i].bezier[3].x, offsetCurve1FragmentArray[i].bezier[3].y );

        iBLContext->setStrokeWidth( 0.5f );
        iBLContext->setStrokeStyle( BLRgba32( 255, 255, 0, 255 ) );
        iBLContext->strokePath( offsetPath );

        handlePath.moveTo ( offsetCurve1FragmentArray[i].bezier[0].x, offsetCurve1FragmentArray[i].bezier[0].y );
        handlePath.lineTo ( offsetCurve1FragmentArray[i].bezier[1].x, offsetCurve1FragmentArray[i].bezier[1].y );

        iBLContext->setStrokeWidth( 0.5f );
        iBLContext->setStrokeStyle( BLRgba32( 255, 0, 255, 255 ) );
        iBLContext->strokePath( handlePath );

        handlePath.moveTo ( offsetCurve1FragmentArray[i].bezier[3].x, offsetCurve1FragmentArray[i].bezier[3].y );
        handlePath.lineTo ( offsetCurve1FragmentArray[i].bezier[2].x, offsetCurve1FragmentArray[i].bezier[2].y );

        iBLContext->setStrokeWidth( 0.5f );
        iBLContext->setStrokeStyle( BLRgba32( 255, 0, 255, 255 ) );
        iBLContext->strokePath( handlePath );
    }
*/
}

double
FOdysseyVectorSegmentCubic::ThickenFraction( FOdysseyVectorFraction* iFraction
                                           , double iStartU
                                           , double& oXmin
                                           , double& oYmin
                                           , double& oXmax
                                           , double& oYmax )
{
    ::ULIS::FVec2D point[6] = { GetPointAt( iFraction->fromT )
                              , mOffsetCurve[0].GetPointAt( iFraction->fromT )
                              , mOffsetCurve[0].GetPointAt( iFraction->toT   )
                              , GetPointAt( iFraction->toT )
                              , mOffsetCurve[1].GetPointAt( iFraction->toT   )
                              , mOffsetCurve[1].GetPointAt( iFraction->fromT ) };
    double startU = iStartU;
    double endU = mLength ? ( startU + ( iFraction->length / mLength ) ) : startU;
    //double startU = iFraction->fromT;
    //double endU = iFraction->toT;
    // we only need 4 of the Min Max, the others 2 are computed in BuildVariable()
    static uint32 idxMinMax[4] = { 1, 2, 4, 5 };

    iFraction->polygon.point[0].x = point[0].x;
    iFraction->polygon.point[0].y = point[0].y;

    iFraction->polygon.point[1].x = point[1].x;
    iFraction->polygon.point[1].y = point[1].y;

    iFraction->polygon.point[2].x = point[2].x;
    iFraction->polygon.point[2].y = point[2].y;

    iFraction->polygon.point[3].x = point[3].x;
    iFraction->polygon.point[3].y = point[3].y;

    iFraction->polygon.point[4].x = point[4].x;
    iFraction->polygon.point[4].y = point[4].y;

    iFraction->polygon.point[5].x = point[5].x;
    iFraction->polygon.point[5].y = point[5].y;

    iFraction->polygon.U[0] = startU;
    iFraction->polygon.V[0] = 0.5f;

    iFraction->polygon.U[1] = startU;
    iFraction->polygon.V[1] = 0.0f;

    iFraction->polygon.U[2] = endU;
    iFraction->polygon.V[2] = 0.0f;

    iFraction->polygon.U[3] = endU;
    iFraction->polygon.V[3] = 0.5f;

    iFraction->polygon.U[4] = endU;
    iFraction->polygon.V[4] = 1.0f;

    iFraction->polygon.U[5] = startU;
    iFraction->polygon.V[5] = 1.0f;

    // --- bounding --- //
    for( uint32 i = 0; i < 4; i++ )
    {
        uint32 idx = idxMinMax[i];

        if( iFraction->polygon.point[idx].x < oXmin ) oXmin = iFraction->polygon.point[idx].x;
        if( iFraction->polygon.point[idx].y < oYmin ) oYmin = iFraction->polygon.point[idx].y;
        if( iFraction->polygon.point[idx].x > oXmax ) oXmax = iFraction->polygon.point[idx].x;
        if( iFraction->polygon.point[idx].y > oYmax ) oYmax = iFraction->polygon.point[idx].y;
    }
    //-------------------//

    return endU;
}

// De Casteljau algorithm. Stopping condition : dot product between p0p3-p0p1 is bigger than some limit value. Same for p3p0-p3p2.
void
FOdysseyVectorSegmentCubic::BuildVariableAdaptive( FOdysseyVectorPoint* iFromPoint
                                                 , FOdysseyVectorPoint* iToPoint
                                                 , double iFromT
                                                 , double iToT
                                                 , double iRadiusFrom
                                                 , double iRadiusTo
                                                 , ::ULIS::FVec2D iBezier[4]
                                                 , uint32 iRecurseDepth
                                                 , uint32 iMinRecurse
                                                 , uint32 iMaxRecurse
                                                 , std::vector<FOdysseyVectorPoint>& iSubPointBuffer
                                                 , std::vector<FSegmentSubLine>& iSubLineBuffer )
{
    ::ULIS::FVec2D childBezier[2][4];
    ::ULIS::FVec2D worldBezier[4] = { FOdysseyVector::MapPoint( mOwner->GetWorldMatrix(), iBezier[0] )
                                    , FOdysseyVector::MapPoint( mOwner->GetWorldMatrix(), iBezier[1] )
                                    , FOdysseyVector::MapPoint( mOwner->GetWorldMatrix(), iBezier[2] )
                                    , FOdysseyVector::MapPoint( mOwner->GetWorldMatrix(), iBezier[3] ) };
    // we use world values because in case of non-uniform transformations, local values might give too shallow subdivision
    ::ULIS::FVec2D straightVector = worldBezier[3] - worldBezier[0];
    ::ULIS::FVec2D ctrlVector[2] = { worldBezier[1] - worldBezier[0]
                                   , worldBezier[2] - worldBezier[3] };
    double dotLimit = 0.9996f; // cos 1.62 deg

    if( straightVector.DistanceSquared() )
    {
        straightVector.Normalize();
    }

    if( ctrlVector[0].DistanceSquared() )
    {
        ctrlVector[0].Normalize();
    }
    else
    {
        ctrlVector[0] = straightVector;
    }

    if( ctrlVector[1].DistanceSquared() )
    {
        ctrlVector[1].Normalize();
    }
    else
    {
        ctrlVector[1] = -straightVector;
    }

    if( ( iRecurseDepth < iMinRecurse  )
     || ( ( iRecurseDepth < iMaxRecurse ) // <--- do not subdivide forever though.
       && ( ( ctrlVector[0].DotProduct(  straightVector ) < dotLimit )
         || ( ctrlVector[1].DotProduct( -straightVector ) < dotLimit ) ) ) )
    {
        FOdysseyVectorPoint* splitPoint;
        double radiusAt = ( iRadiusFrom + iRadiusTo ) * 0.5f;
        double splitsAt = ( iToT + iFromT ) * 0.5f;

        memcpy( childBezier[0], iBezier, sizeof( childBezier[0] ) );
        memcpy( childBezier[1], iBezier, sizeof( childBezier[1] ) );

        // First sub-bezier from the divided parent bezier
        // Note: we always split at 0.5f. The splitsAt variable just helps setting the fromT and toT variables of the polygon cache.
        ::ULIS::CubicBezierSplitAtParameter       <::ULIS::FVec2D>( &childBezier[0][0]
                                                                  , &childBezier[0][1]
                                                                  , &childBezier[0][2]
                                                                  , &childBezier[0][3]
                                                                  , 0.5f );

        splitPoint = &iSubPointBuffer.emplace_back( childBezier[0][3].x
                                                  , childBezier[0][3].y );

        BuildVariableAdaptive( iFromPoint
                              , splitPoint
                              , iFromT
                              , splitsAt
                              , iRadiusFrom
                              , radiusAt
                              , childBezier[0]
                              , iRecurseDepth + 1
                              , iMinRecurse
                              , iMaxRecurse
                              , iSubPointBuffer
                              , iSubLineBuffer );

        // Second sub-bezier from the divided parent bezier
        // Note: we always split at 0.5f. The splitsAt variable just helps setting the fromT and toT variables of the polygon cache.
        ::ULIS::CubicBezierInverseSplitAtParameter<::ULIS::FVec2D>( &childBezier[1][0]
                                                                  , &childBezier[1][1]
                                                                  , &childBezier[1][2]
                                                                  , &childBezier[1][3]
                                                                  , 0.5f );
        BuildVariableAdaptive( splitPoint
                              , iToPoint
                              , splitsAt
                              , iToT
                              , radiusAt
                              , iRadiusTo
                              , childBezier[1]
                              , iRecurseDepth + 1
                              , iMinRecurse
                              , iMaxRecurse
                              , iSubPointBuffer
                              , iSubLineBuffer );
    }
    else
    {
        FSegmentSubLine& subLine = iSubLineBuffer.emplace_back( iFromPoint
                                                              , iFromT
                                                              , iToPoint
                                                              , iToT );

        mLength += subLine.length;
    }
}

// De Casteljau algorithm. Stopping condition : dot product between p0p3-p0p1 is bigger than some limit value. Same for p3p0-p3p2.
void
FOdysseyVectorSegmentCubic::BuildOffsetCurvesRecursive( ::ULIS::FVec2D iBezier[4]
                                                      , double iFromT
                                                      , double iToT
                                                      , double iDotLimit
                                                      , uint32 iMinRecurse
                                                      , uint32 iMaxRecurse
                                                      , uint32 iCurrentRecurse
                                                      , std::vector<FOdysseyVectorBezierFragment>& oBezierFragmentArray )
{
    ::ULIS::FVec2D childBezier[2][4];
    ::ULIS::FVec2D straightVector = iBezier[3] - iBezier[0];
    ::ULIS::FVec2D ctrlVector[2] = { iBezier[1] - iBezier[0]
                                   , iBezier[2] - iBezier[3] };

    if( straightVector.DistanceSquared() )
    {
        straightVector.Normalize();
    }

    if( ctrlVector[0].DistanceSquared() )
    {
        ctrlVector[0].Normalize();
    }

    if( ctrlVector[1].DistanceSquared() )
    {
        ctrlVector[1].Normalize();
    }

    if( ( iCurrentRecurse < iMinRecurse ) // <-- Force at least 4 subdivisions because the intersections for paint groups are tested
                                          // linearly and we need precision. If the cubic segment is made of few linear sub-segments,
                                          // then the T value at intersection does not match the T value we would get with mathematically
                                          // accurate Bezier-Bezier intersection, but these are very complicated to implement so we just
                                          // stick with linear intersections. By dividing the bezier segment with smaller liner segments
                                          // whose T values at end points are known, we get almost correct values for T at intersections.
     || ( ( iCurrentRecurse < iMaxRecurse ) // <--- do not subdivide forever though.
       && ( ( ctrlVector[0].DotProduct(  straightVector ) < iDotLimit )
         || ( ctrlVector[1].DotProduct( -straightVector ) < iDotLimit ) ) ) )
    {
        ::ULIS::FVec2D tangent = ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( iBezier[0]
                                                                                      , iBezier[1]
                                                                                      , iBezier[2]
                                                                                      , iBezier[3]
                                                                                      , 0.5f );
        if( tangent.DistanceSquared() )
        {
            double splitsAt = ( iToT + iFromT ) * 0.5f;

            tangent.Normalize();

            memcpy( childBezier[0], iBezier, sizeof( childBezier[0] ) );
            memcpy( childBezier[1], iBezier, sizeof( childBezier[1] ) );

            // First sub-bezier from the divided parent bezier
            // Note: we always split at 0.5f. The splitsAt variable just helps setting the fromT and toT variables of the polygon cache.
            ::ULIS::CubicBezierSplitAtParameter       <::ULIS::FVec2D>( &childBezier[0][0]
                                                                      , &childBezier[0][1]
                                                                      , &childBezier[0][2]
                                                                      , &childBezier[0][3]
                                                                      , 0.5f );
            BuildOffsetCurvesRecursive( childBezier[0]
                                      , iFromT
                                      , splitsAt
                                      , iDotLimit
                                      , iMinRecurse
                                      , iMaxRecurse
                                      , iCurrentRecurse + 1
                                      , oBezierFragmentArray );

            // Second sub-bezier from the divided parent bezier
            // Note: we always split at 0.5f. The splitsAt variable just helps setting the fromT and toT variables of the polygon cache.
            ::ULIS::CubicBezierInverseSplitAtParameter<::ULIS::FVec2D>( &childBezier[1][0]
                                                                      , &childBezier[1][1]
                                                                      , &childBezier[1][2]
                                                                      , &childBezier[1][3]
                                                                      , 0.5f );
            BuildOffsetCurvesRecursive( childBezier[1]
                                      , splitsAt
                                      , iToT
                                      , iDotLimit
                                      , iMinRecurse
                                      , iMaxRecurse
                                      , iCurrentRecurse + 1
                                      , oBezierFragmentArray );
        }
    }
    else
    {
        FOdysseyVectorBezierFragment& fragment = oBezierFragmentArray.emplace_back( iBezier, iFromT, iToT );

        fragment.straightVector = ::ULIS::FVec2D( iBezier[3] - iBezier[0] );
        fragment.straightLength = fragment.straightVector.Distance();
    }
}

FOdysseyVectorOffsetCurveCubic*
FOdysseyVectorSegmentCubic::GetOffsetCurve( uint32 iID )
{
    return &mOffsetCurve[iID];
}

void
FOdysseyVectorSegmentCubic::PrepareOffsetBeziers( double iSegmentStartRadius
                                                , double iSegmentEndRadius
                                                , FOdysseyVectorBezierFragment& iGuideFragment
                                                , FOdysseyVectorBezierFragment& oFragment0
                                                , FOdysseyVectorBezierFragment& oFragment1 )
{
    ::ULIS::FVec2D tangent[2] = { ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( iGuideFragment.bezier[0]
                                                                                       , iGuideFragment.bezier[1]
                                                                                       , iGuideFragment.bezier[2]
                                                                                       , iGuideFragment.bezier[3]
                                                                                       , 0.0f )
                                , ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( iGuideFragment.bezier[0]
                                                                                       , iGuideFragment.bezier[1]
                                                                                       , iGuideFragment.bezier[2]
                                                                                       , iGuideFragment.bezier[3]
                                                                                       , 1.0f ) };
    ::ULIS::FVec2D tanvec[2] = { iGuideFragment.bezier[1] - iGuideFragment.bezier[0]
                               , iGuideFragment.bezier[2] - iGuideFragment.bezier[3] };
    //double ratio[2];
    ::ULIS::FVec2D widthPointStart[2];
    ::ULIS::FVec2D widthPointEnd[2];
    double fragmentStartRadius = ( iSegmentStartRadius * ( 1.0f - iGuideFragment.fromT ) )
                               + ( iSegmentEndRadius   * iGuideFragment.fromT );
    double fragmentEndRadius   = ( iSegmentStartRadius * ( 1.0f - iGuideFragment.toT ) )
                               + ( iSegmentEndRadius   * iGuideFragment.toT );
    //--
/*
    widthPointStart[0] = iFragment.bezier[0];
    widthPointStart[1] = iFragment.bezier[0];
    widthPointEnd[0]   = iFragment.bezier[3];
    widthPointEnd[1]   = iFragment.bezier[3];
*/
    if( tangent[0].DistanceSquared() == 0.0f )
    {
        tangent[0] = iGuideFragment.bezier[2] - iGuideFragment.bezier[0];
    }

    if( tangent[0].DistanceSquared() )
    {
        tangent[0].Normalize();
    }

    if( tangent[1].DistanceSquared() == 0.0f )
    {
        tangent[1] = iGuideFragment.bezier[3] - iGuideFragment.bezier[1];
    }

    if( tangent[1].DistanceSquared() )
    {
        tangent[1].Normalize();
    }

    ::ULIS::FVec2D perpendicular[2] = { ::ULIS::FVec2D( -tangent[0].y, tangent[0].x )
                                      , ::ULIS::FVec2D( -tangent[1].y, tangent[1].x ) };


    widthPointStart[0] = iGuideFragment.bezier[0] + ( perpendicular[0] * fragmentStartRadius );
    widthPointStart[1] = iGuideFragment.bezier[0] - ( perpendicular[0] * fragmentStartRadius );

    widthPointEnd[0]   = iGuideFragment.bezier[3] + ( perpendicular[1] * fragmentEndRadius );
    widthPointEnd[1]   = iGuideFragment.bezier[3] - ( perpendicular[1] * fragmentEndRadius );

/* RIBBON
    widthPointStart[0] = iGuideFragment.bezier[0] + fragmentStartRadius;
    widthPointStart[1] = iGuideFragment.bezier[0] - fragmentStartRadius;

    widthPointEnd[0]   = iGuideFragment.bezier[3] + fragmentEndRadius;
    widthPointEnd[1]   = iGuideFragment.bezier[3] - fragmentEndRadius;
*/
    oFragment0.bezier[0] = widthPointStart[0];
    oFragment0.bezier[3] = widthPointEnd[0];
    oFragment0.straightVector = ::ULIS::FVec2D( oFragment0.bezier[3] - oFragment0.bezier[0] );
    oFragment0.straightLength = oFragment0.straightVector.Distance();
    oFragment0.fromT = iGuideFragment.fromT;
    oFragment0.toT = iGuideFragment.toT;

    oFragment0.bezier[1] = oFragment0.bezier[0] + ( tangent[0] * oFragment0.straightLength * 0.35f );
    oFragment0.bezier[2] = oFragment0.bezier[3] - ( tangent[1] * oFragment0.straightLength * 0.35f );

    if ( oFragment0.straightLength )
    {
        oFragment0.straightVector.Normalize();
    }

    oFragment1.bezier[0] = widthPointStart[1];
    oFragment1.bezier[3] = widthPointEnd[1];
    oFragment1.straightVector = ::ULIS::FVec2D( oFragment1.bezier[3] - oFragment1.bezier[0] );
    oFragment1.straightLength = oFragment1.straightVector.Distance();
    oFragment1.fromT = iGuideFragment.fromT;
    oFragment1.toT = iGuideFragment.toT;

    oFragment1.bezier[1] = oFragment1.bezier[0] + ( tangent[0] * oFragment1.straightLength * 0.35f );
    oFragment1.bezier[2] = oFragment1.bezier[3] - ( tangent[1] * oFragment1.straightLength * 0.35f );

    if ( oFragment1.straightLength )
    {
        oFragment1.straightVector.Normalize();
    }
}

void
FOdysseyVectorSegmentCubic::SmoothOffsetCurves( std::vector<FOdysseyVectorBezierFragment>& iGuideBezierFragmentArray )
{
    std::vector<FOdysseyVectorBezierFragment>& offsetCurve0BezierFragmentArray = mOffsetCurve[0].GetBezierFragmentArray();
    std::vector<FOdysseyVectorBezierFragment>& offsetCurve1BezierFragmentArray = mOffsetCurve[1].GetBezierFragmentArray();
    FOdysseyVectorSegment* prevSegment = GetVertex(0)->GetOtherSegment( this );
    FOdysseyVectorSegment* nextSegment = GetVertex(1)->GetOtherSegment( this );
    double prevSegmentLength = prevSegment ? prevSegment->GetLength() : 0.0f;
    double nextSegmentLength = nextSegment ? nextSegment->GetLength() : 0.0f;
    // Note, it does note matter that firstOffsetPoint[2] is uninitialized, it will be afterwards
    //::ULIS::FVec2D* prevOffsetPoint[2] = { &firstOffsetPoint[0], &firstOffsetPoint[1] };
    //::ULIS::FVec2D* nextOffsetPoint[2] = { nullptr             , nullptr             };
    double segmentStartRadius = GetVertex(0)->GetRadius();
    double segmentEndRadius = GetVertex(1)->GetRadius();
    int guideBezierFragmentCount = iGuideBezierFragmentArray.size();

    mOffsetCurve[0].Resize( guideBezierFragmentCount );
    mOffsetCurve[1].Resize( guideBezierFragmentCount );

    // technically, we could as well iterate using offsetCurve0BezierFragmentArray or offsetCurve1BezierFragmentArray
    // because they have the same number of fragments, but for consistency we iterate using iGuideBezierFragmentArray
    for( int i = 0; i < guideBezierFragmentCount; i++ )
    {
        FOdysseyVectorBezierFragment* guideFragment = &iGuideBezierFragmentArray[i];
        int p = i - 1;

        PrepareOffsetBeziers( segmentStartRadius
                            , segmentEndRadius
                            , *guideFragment
                            , offsetCurve0BezierFragmentArray[i]
                            , offsetCurve1BezierFragmentArray[i] );

        if( p >= 0 )
        {
            FOdysseyVectorBezierFragment* prevFragment[2] = { &offsetCurve0BezierFragmentArray[p]
                                                            , &offsetCurve1BezierFragmentArray[p] };
            FOdysseyVectorBezierFragment* currFragment[2] = { &offsetCurve0BezierFragmentArray[i]
                                                            , &offsetCurve1BezierFragmentArray[i] };
            ::ULIS::FVec2D* prevVector[2] = { &offsetCurve0BezierFragmentArray[p].straightVector
                                            , &offsetCurve1BezierFragmentArray[p].straightVector };
            ::ULIS::FVec2D* currVector[2] = { &offsetCurve0BezierFragmentArray[i].straightVector
                                            , &offsetCurve1BezierFragmentArray[i].straightVector };
            ::ULIS::FVec2D tangent[2] = { *prevVector[0] + *currVector[0]
                                        , *prevVector[1] + *currVector[1] };
            //::ULIS::FVec2D tangent[2] = { currFragment[0]->bezier[3] - prevFragment[0]->bezier[0]
            //                            , currFragment[1]->bezier[3] - prevFragment[1]->bezier[0] };

            //double dot[2] = { prevFragment[0]->straightVector.DotProduct( currFragment[0]->straightVector )
            //                , prevFragment[1]->straightVector.DotProduct( currFragment[1]->straightVector ) };
            //double dotFactor[2] = { acos( dot[0] ) / M_PI * 0.55191502449f
            //                      , acos( dot[1] ) / M_PI * 0.55191502449f };

            if( tangent[0].DistanceSquared() ) tangent[0].Normalize();
            if( tangent[1].DistanceSquared() ) tangent[1].Normalize();

            //if( ( guideFragment->straightVector.DotProduct( *prevVector[0] ) > 0.0f )
            // && ( guideFragment->straightVector.DotProduct( *currVector[0] ) > 0.0f ) )
            {
                currFragment[0]->bezier[1] = currFragment[0]->bezier[0] + ( tangent[0] * currFragment[0]->straightLength * 0.33f );
                prevFragment[0]->bezier[2] = prevFragment[0]->bezier[3] - ( tangent[0] * prevFragment[0]->straightLength * 0.33f );
            }

            //if( ( guideFragment->straightVector.DotProduct( *prevVector[1] ) > 0.0f )
            // && ( guideFragment->straightVector.DotProduct( *currVector[1] ) > 0.0f ) )
            {
                currFragment[1]->bezier[1] = currFragment[1]->bezier[0] + ( tangent[1] * currFragment[1]->straightLength * 0.33f );
                prevFragment[1]->bezier[2] = prevFragment[1]->bezier[3] - ( tangent[1] * prevFragment[1]->straightLength * 0.33f );
            }
        }
    }
}

void
FOdysseyVectorSegmentCubic::BuildOffsetCurves()
{
    std::vector<FOdysseyVectorBezierFragment> guideBezierFragmentArray;

    guideBezierFragmentArray.reserve( 20 );

    // TODO: Should be renamed. This does not build the offset curve. It only builds a guide curve to the offset curves.
    BuildOffsetCurvesRecursive( mBezier
                              , 0.0f
                              , 1.0f
                              , 0.97814760073f // 12deg //0.9945f // 6deg - //
                              , 3 // min recurse
                              , 4 // max recurse
                              , 0 // current recurse
                              , guideBezierFragmentArray );

    if( guideBezierFragmentArray.size() )
    {
        SmoothOffsetCurves( guideBezierFragmentArray );
    }
}

void
FOdysseyVectorSegmentCubic::Update( uint32 iUpdateFlags )
{
    FOdysseyVectorCell* cell = mOwner->GetCell();
    ::ULIS::FRectD previousBBox = mBBox;
    double xmin, ymin, xmax, ymax;


    FOdysseyVectorSegment::Update( iUpdateFlags );

    mBezier[0] = mPoint[0]->GetCoords();
    mBezier[1] = mCtrlPoint[0].GetCoords();
    mBezier[2] = mCtrlPoint[1].GetCoords();
    mBezier[3] = mPoint[1]->GetCoords();

    xmin = ULIS::FMath::Min4<double>( mBezier[0].x
                                    , mBezier[1].x
                                    , mBezier[2].x
                                    , mBezier[3].x );

    ymin = ULIS::FMath::Min4<double>( mBezier[0].y
                                    , mBezier[1].y
                                    , mBezier[2].y
                                    , mBezier[3].y );

    xmax = ULIS::FMath::Max4<double>( mBezier[0].x
                                    , mBezier[1].x
                                    , mBezier[2].x
                                    , mBezier[3].x );

    ymax = ULIS::FMath::Max4<double>( mBezier[0].y
                                    , mBezier[1].y
                                    , mBezier[2].y
                                    , mBezier[3].y );

    if( mNeedsWidth )
    {
        BuildVariable( iUpdateFlags & FOdysseyVectorObject::UPDATE_NEEDPOLYLINE ? 5 : MINRECURSE
                     , MAXRECURSE
                     , xmin
                     , ymin
                     , xmax
                     , ymax );
    }

    mBBox = ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax );

    if( ( iUpdateFlags & FOdysseyVectorObject::UPDATE_NOINVALIDATERECT ) == 0 )
    {
        // auto invalidation of the region that needs to be redrawn
        if( cell )
        {
            cell->InvalidateRect( FOdysseyVector::MapRect( mOwner->GetWorldMatrix(), ( previousBBox | mBBox ) ) );
        }
    }
}

static bool IntersectSegment( const ::ULIS::FVec2D& iLine0p0
                            , const ::ULIS::FVec2D& iLine0p1
                            , const ::ULIS::FVec2D& iLine1p0
                            , const ::ULIS::FVec2D& iLine1p1
                            , ::ULIS::FVec2D& oIntersection )
{
    ::ULIS::FVec2D L0Vec   = { iLine0p1.x - iLine0p0.x, iLine0p1.y - iLine0p0.y };
    ::ULIS::FVec2D L1Vec   = { iLine1p0.x - iLine1p1.x, iLine1p0.y - iLine1p1.y }; // reverted order is normal
    ::ULIS::FVec2D L0L1Vec = { iLine1p0.x - iLine0p0.x, iLine1p0.y - iLine0p0.y };

    double det = L0Vec.x * L1Vec.y - L0Vec.y * L1Vec.x;

    if( fabs(det) == 0.0f ) return false;

    double t = ( L0L1Vec.x * L1Vec.y   - L0L1Vec.y * L1Vec.x ) / det;

    oIntersection = iLine0p0 + ( t * L0Vec );

    return true;
}

void
FOdysseyVectorSegmentCubic::BuildVariable( uint32 iMinRecurse
                                         , uint32 iMaxRecurse
                                         , double& oXmin
                                         , double& oYmin
                                         , double& oXmax
                                         , double& oYmax )
{
    std::vector<FOdysseyVectorPoint> subPointBuffer;
    std::vector<FSegmentSubLine> subLineBuffer;
    //static std::mutex mutex;

    subLineBuffer.reserve( MAXSUBLINE );
     // Note: segment end points don't belong to the buffer
    subPointBuffer.reserve( MAXSUBLINE - 1 );

    //mLength = FOdysseyVector::GetBezierApproximateLength( mBezier, 8 );

    mLength = 0.0f; // note: BuildVariableAdaptive will update the length

    ResetPolygonCache();

    //if( mLength )
    {
        double segmentStartRadius = static_cast<FOdysseyVectorVertex*>(mPoint[0])->GetRadius();
        double segmentEndRadius = static_cast<FOdysseyVectorVertex*>(mPoint[1])->GetRadius();
        double startU = 0.0f;

        BuildOffsetCurves();
        // offset curves must be built before the polygon cache is built, as the builidng process
        // uses the offset curves to build the polygons.

        BuildVariableAdaptive ( mPoint[0]
                              , mPoint[1]
                              , 0.0f
                              , 1.0f
                              , segmentStartRadius
                              , segmentEndRadius
                              , mBezier
                              , 0
                              , iMinRecurse
                              , iMaxRecurse
                              , subPointBuffer
                              , subLineBuffer );


        if( subPointBuffer.size() )
        {
            mFractionPointBuffer = subPointBuffer;
        }

        mFractionCache.reserve( subLineBuffer.size() );

        for( FSegmentSubLine& subLine : subLineBuffer )
        {
            FOdysseyVectorPoint* point0 = subLine.point[0];
            FOdysseyVectorPoint* point1 = subLine.point[1];
            double x0 = point0->GetX();
            double y0 = point0->GetY();
            double x1 = point1->GetX();
            double y1 = point1->GetY();

            // ------ bounding -------//
            if( x0 < oXmin ) oXmin = x0;
            if( y0 < oYmin ) oYmin = y0;
            if( x0 > oXmax ) oXmax = x0;
            if( y0 > oYmax ) oYmax = y0;

            if( x1 < oXmin ) oXmin = x1;
            if( y1 < oYmin ) oYmin = y1;
            if( x1 > oXmax ) oXmax = x1;
            if( y1 > oYmax ) oYmax = y1;
            // ---------------------- //

            if( subLine.point[0]->GetClass() == FOdysseyVectorPoint::StaticClass() )
            {
                // pointer arithmetic gives us the index directly
                uint32 index = ( subLine.point[0] - &subPointBuffer[0] );

                point0 = &mFractionPointBuffer[index];
            }

            if( subLine.point[1]->GetClass() == FOdysseyVectorPoint::StaticClass() )
            {
                // pointer arithmetic gives us the index directly
                uint32 index = ( subLine.point[1] - &subPointBuffer[0] );

                point1 = &mFractionPointBuffer[index];
            }

            FOdysseyVectorFraction& fraction = mFractionCache.emplace_back( point0
                                                                          , subLine.t[0]
                                                                          , point1
                                                                          , subLine.t[1]
                                                                          , subLine.length );

            startU = ThickenFraction( &fraction, startU, oXmin, oYmin, oXmax, oYmax );
        }
    }
}

::ULIS::FVec2D
FOdysseyVectorSegmentCubic::GetOffsetPoint( uint32 iSide, double iT )
{
    return mOffsetCurve[iSide].GetPointAt( iT );
}

uint32
FOdysseyVectorSegmentCubic::CompareBezier( FOdysseyVectorSegmentCubic* iOtherCubicSegment )
{
    ::ULIS::FVec2D* otherBezier = iOtherCubicSegment->GetBezier();

    if( ( otherBezier[0] == mBezier[0] )
      &&( otherBezier[1] == mBezier[1] )
      &&( otherBezier[2] == mBezier[2] )
      &&( otherBezier[3] == mBezier[3] ) )
    {
        return 0;
    }

    return 1;
}
