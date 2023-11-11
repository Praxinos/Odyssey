#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorPath.h"

#define MINRECURSE 4
#define MAXRECURSE 7

static bool IntersectSegment( const ::ULIS::FVec2D& iLine0p0
                            , const ::ULIS::FVec2D& iLine0p1
                            , const ::ULIS::FVec2D& iLine1p0
                            , const ::ULIS::FVec2D& iLine1p1
                            , ::ULIS::FVec2D& oIntersection );

FOdysseyVectorSegmentCubic::~FOdysseyVectorSegmentCubic()
{

}

FOdysseyVectorSegmentCubic::FOdysseyVectorSegmentCubic( FOdysseyVectorPath* iPath
                                                      , FOdysseyVectorVertex* iPoint0
                                                      , double iCtrlPoint0x
                                                      , double iCtrlPoint0y
                                                      , double iCtrlPoint1x
                                                      , double iCtrlPoint1y
                                                      , FOdysseyVectorVertex* iPoint1
                                                      , bool iNeedWidth )
    : FOdysseyVectorSegment( iPath, iPoint0, iPoint1 )
    , mNeedWidth( iNeedWidth )
    , mCtrlPoint { FOdysseyVectorHandleSegment( this, 0, 0.0f, 0.0f )
                 , FOdysseyVectorHandleSegment( this, 1, 0.0f, 0.0f ) }
{
    Init ( iPoint0, iCtrlPoint0x, iCtrlPoint0y, iCtrlPoint1x, iCtrlPoint1y, iPoint1 );
}

FOdysseyVectorSegmentCubic::FOdysseyVectorSegmentCubic( FOdysseyVectorPath* iPath
                                                      , FOdysseyVectorVertex* iPoint0
                                                      , FOdysseyVectorVertex* iPoint1
                                                      , bool iNeedWidth )
    : FOdysseyVectorSegment( iPath, iPoint0, iPoint1 )
    , mNeedWidth( iNeedWidth )
    , mCtrlPoint { FOdysseyVectorHandleSegment( this, 0, 0.0f, 0.0f )
                 , FOdysseyVectorHandleSegment( this, 1, 0.0f, 0.0f ) }
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
    mPoint[0] = iPoint0;
    mPoint[1] = iPoint1;

    mCtrlPoint[0].Set( iCtrlPoint0x, iCtrlPoint0y );
    mCtrlPoint[1].Set( iCtrlPoint1x, iCtrlPoint1y );

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
FOdysseyVectorSegmentCubic::IncreasePolygonCache( uint32 iSize )
{
     mFractionCache.resize( mFractionCache.size() + iSize );
}

void
FOdysseyVectorSegmentCubic::ResetPolygonCache( )
{
    mFractionCache.clear();
    mFractionCache.reserve( 200 );
}

// mask-based version of the picking process
bool
FOdysseyVectorSegmentCubic::Pick( const ::ULIS::FRectD& iMaskRect, uint8* iPixelData )
{
    BLMatrix2D& worldMatrix = GetPath()->GetWorldMatrix();
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
        ::ULIS::FVec2D p0 = { mFractionCache[i].lineVertex[0].x, mFractionCache[i].lineVertex[0].y };
        ::ULIS::FVec2D p1 = { mFractionCache[i].lineVertex[1].x, mFractionCache[i].lineVertex[1].y };
        double t = FOdysseyVector::DistanceToSegment( pt, p0, p1, dist );

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

FOdysseyVectorSegmentCubic*
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
    double radius0 = mPoint[0]->GetRadius();
    double radius1 = mPoint[1]->GetRadius();
    double deltaRadius = ( radius1 - radius0 );
    double fromRadius = radius0 + ( deltaRadius * iFromT );
    double toRadius = radius0 + ( deltaRadius * iToT );
    FOdysseyVectorVertex* vertex0 = ( iFromT == 0.0f ) ? static_cast<FOdysseyVectorVertex*>(mPoint[0]) : new FOdysseyVectorVertex( mPath, pointAt0.x, pointAt0.y, fromRadius );
    FOdysseyVectorVertex* vertex1 = ( iToT   == 1.0f ) ? static_cast<FOdysseyVectorVertex*>(mPoint[1]) : new FOdysseyVectorVertex( mPath, pointAt1.x, pointAt1.y, toRadius   );
    FOdysseyVectorSegmentCubic* sampleSegment = new FOdysseyVectorSegmentCubic( mPath, vertex0, vertex1, true );
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

void
FOdysseyVectorSegmentCubic::UpdateBoundingBox ()
{
   mBBox.x = ULIS::FMath::Min4<double>( mPoint[0]->GetX() - mPoint[0]->GetRadius()
                                      , mCtrlPoint[0].GetX()
                                      , mPoint[1]->GetX() - mPoint[1]->GetRadius()
                                      , mCtrlPoint[1].GetX() );

   mBBox.y = ULIS::FMath::Min4<double>( mPoint[0]->GetY() - mPoint[0]->GetRadius()
                                      , mCtrlPoint[0].GetY()
                                      , mPoint[1]->GetY() - mPoint[1]->GetRadius()
                                      , mCtrlPoint[1].GetY() );

   mBBox.w = ULIS::FMath::Max4<double>( mPoint[0]->GetX() + mPoint[0]->GetRadius()
                                      , mCtrlPoint[0].GetX()
                                      , mPoint[1]->GetX() + mPoint[1]->GetRadius()
                                      , mCtrlPoint[1].GetX() ) - mBBox.x;

   mBBox.h = ULIS::FMath::Max4<double>( mPoint[0]->GetY() + mPoint[0]->GetRadius()
                                      , mCtrlPoint[0].GetY()
                                      , mPoint[1]->GetY() + mPoint[1]->GetRadius()
                                      , mCtrlPoint[1].GetY() ) - mBBox.y;
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

::ULIS::FRectD
FOdysseyVectorSegmentCubic::GetBoundingBox( bool iWorld )
{
    if( iWorld == true )
    {
        BLMatrix2D& worldMatrix = GetPath()->GetWorldMatrix();
        BLPoint p0 = worldMatrix.mapPoint( mBBox.x          , mBBox.y           );
        BLPoint p1 = worldMatrix.mapPoint( mBBox.x + mBBox.w, mBBox.y           );
        BLPoint p2 = worldMatrix.mapPoint( mBBox.x + mBBox.w, mBBox.y + mBBox.h );
        BLPoint p3 = worldMatrix.mapPoint( mBBox.x          , mBBox.y + mBBox.h );
        ::ULIS::FRectD bbox = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                                        , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                                        , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                                        , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );

        return bbox;
    }

    return mBBox;
}

bool
FOdysseyVectorSegmentCubic::Cut( const ::ULIS::FVec2D& linePoint0
                               , const ::ULIS::FVec2D& linePoint1
                               , std::vector<FOdysseyVectorVertex*>& oNewVertexArray
                               , std::vector<FOdysseyVectorSegment*>& oNewSegmentArray )
{
    ::ULIS::FVec2D& point0 = GetVertex(0)->GetCoords();
    ::ULIS::FVec2D& point1 = GetVertex(1)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = GetHandle(0)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = GetHandle(1)->GetCoords();
    // we'll have 3 intersections at most and 2 points at tips.
    FOdysseyVectorVertex* pointChain[5] = { static_cast<FOdysseyVectorVertex*>(mPoint[0]), nullptr, nullptr, nullptr, nullptr };
    double tChain[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    uint32 pointCount = 1;
    ::ULIS::FVec2D ctrlPoint0Vector = GetVectorAtStart( true );
    ::ULIS::FVec2D ctrlPoint1Vector = GetVectorAtEnd( true );
    double difRadius = mPoint[1]->GetRadius() - mPoint[0]->GetRadius();

    for( int i = 0; i < mFractionCache.size(); i++ )
    {
        FOdysseyVectorFraction* fraction = &mFractionCache[i];
        double polySubT, interPolySubT;

        if ( FOdysseyVector::IntersectSegment( fraction->lineVertex[0]
                                             , fraction->lineVertex[1]
                                             , linePoint0
                                             , linePoint1
                                             , &polySubT
                                             , &interPolySubT ) )
        {
            double segmentT = fraction->fromT + ( polySubT * ( fraction->toT - fraction->fromT ) );
            ::ULIS::FVec2D pointAt = ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( point0
                                                                                        , ctrlPoint0
                                                                                        , ctrlPoint1
                                                                                        , point1
                                                                                        , segmentT );
            FOdysseyVectorVertex* newCubicPoint = new FOdysseyVectorVertex( mPath, pointAt.x, pointAt.y, 0.0f );

            newCubicPoint->SetRadius( mPoint[0]->GetRadius() + ( difRadius * segmentT ) );

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
            FOdysseyVectorSegmentCubic* newSegment = new FOdysseyVectorSegmentCubic( mPath
                                                                                   , static_cast<FOdysseyVectorVertex*>(pointChain[i])
                                                                                   , static_cast<FOdysseyVectorVertex*>(pointChain[n])
                                                                                   , true );
            ::ULIS::FVec2D& newSegmentPoint0 = newSegment->GetVertex(0)->GetCoords();
            ::ULIS::FVec2D& newSegmentPoint1 = newSegment->GetVertex(1)->GetCoords();
            ::ULIS::FVec2D& newSegmentCtrlPoint0 = newSegment->GetHandle(0)->GetCoords();
            ::ULIS::FVec2D& newSegmentCtrlPoint1 = newSegment->GetHandle(1)->GetCoords();

            FOdysseyVector::BezierExtract( point0, ctrlPoint0, ctrlPoint1, point1
                                         , tChain[i]
                                         , tChain[n]
                                         , newSegmentPoint0, newSegmentCtrlPoint0, newSegmentCtrlPoint1, newSegmentPoint1 );

            oNewSegmentArray.push_back( newSegment );
        }

        return true;
    }

    return false;
}

typedef struct _FAlmostIntersect
{
    double distance;
    double t;
    double otherT;
} FAlmostIntersect;
/*
static uint32
CreateAlmostIntersection( FAlmostIntersect aisx[4]
                        , double iTolerance
                        , FOdysseyVectorSegmentCubic* segment
                        , FOdysseyVectorSegmentCubic* otherSegment
                        , std::vector<FOdysseyVectorVertexIntersection*>& iIntersectionVertexArray )
{
    uint32 intersectionCount = 0;

    for( int i = 0; i < 4; i++ )
    {

        if( aisx[i].distance < iTolerance )
        {
            FOdysseyVectorVertexIntersection* intersectionVertex = new FOdysseyVectorVertexIntersection();

            iIntersectionVertexArray.push_back( intersectionVertex );

            // AddSegment() MUST be called before AddIntersection because AddIntersection uses the value of t that is stored by AddSegment()
            intersectionVertex->AddSegment (      segment, aisx[i].t      );
            intersectionVertex->AddSegment ( otherSegment, aisx[i].otherT );

            segment->AddIntersection ( intersectionVertex );
            otherSegment->AddIntersection ( intersectionVertex );

            intersectionCount++;
        }
    }

    return intersectionCount;
}
*/
static void
IntersectVertices( FOdysseyVectorVertex* iVertex0, FOdysseyVectorVertex* iVertex1, double iTolerance )
{
    if( iVertex0 != iVertex1 )
    {
        if( ( iVertex0->GetSegmentCount() == 1 ) /*&& ( iVertex1->GetSegmentCount() == 1 )*/ )
        {
            ::ULIS::FVec2D& point0 = iVertex0->GetCoords();
            ::ULIS::FVec2D& point1 = iVertex1->GetCoords();
            ::ULIS::FVec2D dif = point0 - point1;
            double distance = dif.Distance();

            if( distance < iTolerance )
            {
                if( distance < iVertex0->GetDistanceToNearestSegment() )
                {
                    //FOdysseyVectorSegment* segment0 = iVertex0->GetFirstSegment();
                    FOdysseyVectorSegment* segment1 = iVertex1->GetFirstSegment();
                    //double t0 = iVertex0->GetT( segment0 );
                    double t1 = iVertex1->GetT( segment1 );

                    iVertex0->SetNearestSegment( segment1, distance, t1 );
                    //iVertex1->SetNearestSegment( segment0, distance, t0 );
                }
            }
        }
    }
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
FOdysseyVectorSegmentCubic::Draw( BLContext* iBLContext )
{
    std::vector<FOdysseyVectorBezierFragment>& offsetCurve0FragmentArray = mOffsetCurve[0].GetBezierFragmentArray();
    std::vector<FOdysseyVectorBezierFragment>& offsetCurve1FragmentArray = mOffsetCurve[1].GetBezierFragmentArray();
    // NOTE: Might not be super fast to call this for each segment
/*
    blctx->fillPath( mBLPath );
*/

    DrawFractionCache( iBLContext );

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
}

void
FOdysseyVectorSegmentCubic::ThickenFraction( FOdysseyVectorFraction* iFraction )
{
    ::ULIS::FVec2D point[4] = { mOffsetCurve[0].GetPointAt( iFraction->fromT )
                              , mOffsetCurve[0].GetPointAt( iFraction->toT   )
                              , mOffsetCurve[1].GetPointAt( iFraction->toT   )
                              , mOffsetCurve[1].GetPointAt( iFraction->fromT ) };

    iFraction->polygon.point[0].x = point[0].x;
    iFraction->polygon.point[0].y = point[0].y;

    iFraction->polygon.point[1].x = point[1].x;
    iFraction->polygon.point[1].y = point[1].y;

    iFraction->polygon.point[2].x = point[2].x;
    iFraction->polygon.point[2].y = point[2].y;

    iFraction->polygon.point[3].x = point[3].x;
    iFraction->polygon.point[3].y = point[3].y;

    iFraction->polygon.U[0] = iFraction->fromT;
    iFraction->polygon.V[0] = 0.0f;

    iFraction->polygon.U[1] = iFraction->toT;
    iFraction->polygon.V[1] = 0.0f;

    iFraction->polygon.U[2] = iFraction->toT;
    iFraction->polygon.V[2] = 1.0f;

    iFraction->polygon.U[3] = iFraction->fromT;
    iFraction->polygon.V[3] = 1.0f;
}

// De Casteljau algorithm. Stopping condition : dot product between p0p3-p0p1 is bigger than some limit value. Same for p3p0-p3p2.
void
FOdysseyVectorSegmentCubic::BuildVariableAdaptive( double  iFromT
                                                 , double  iToT
                                                 , double  iRadiusFrom
                                                 , double  iRadiusTo
                                                 , ::ULIS::FVec2D iBezier[4]
                                                 , const ::ULIS::FVec2D& iNormalizedTangentFrom
                                                 , const ::ULIS::FVec2D& iNormalizedTangentTo
                                                 , int32   iRecurseDepth )
{
    ::ULIS::FVec2D childBezier[2][4];
    ::ULIS::FVec2D straightVector = iBezier[3] - iBezier[0];
    ::ULIS::FVec2D ctrlVector[2] = { iBezier[1] - iBezier[0]
                                   , iBezier[2] - iBezier[3] };
    double dotLimit = 0.9996f; // cos 1.62 deg

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

    if( ( iRecurseDepth < MINRECURSE  ) // <-- Force at least 4 subdivisions because the intersections for paint groups are tested 
                                        // linearly and we need precision. If the cubic segment is made of few linear sub-segments, 
                                        // then the T value at intersection does not match the T value we would get with mathematically
                                        // accurate Bezier-Bezier intersection, but these are very complicated to implement so we just 
                                        // stick with linear intersections. By dividing the bezier segment with smaller liner segments
                                        // whose T values at end points are known, we get almost correct values for T at intersections.
     || ( ( iRecurseDepth < MAXRECURSE ) // <--- do not subdivide forever though.
       && ( ( ctrlVector[0].DotProduct(  straightVector ) < dotLimit )
         || ( ctrlVector[1].DotProduct( -straightVector ) < dotLimit ) ) ) )
    {
        ::ULIS::FVec2D tangent = ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( iBezier[0]
                                                                                      , iBezier[1]
                                                                                      , iBezier[2]
                                                                                      , iBezier[3]
                                                                                      , 0.5f );
        if( tangent.DistanceSquared() )
        {
            double radiusAt = ( iRadiusFrom + iRadiusTo ) * 0.5f;
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
            BuildVariableAdaptive( iFromT  
                                 , splitsAt
                                 , iRadiusFrom
                                 , radiusAt
                                 , childBezier[0]
                                 , iNormalizedTangentFrom
                                 , tangent
                                 , iRecurseDepth + 1 );

            // Second sub-bezier from the divided parent bezier
            // Note: we always split at 0.5f. The splitsAt variable just helps setting the fromT and toT variables of the polygon cache.
            ::ULIS::CubicBezierInverseSplitAtParameter<::ULIS::FVec2D>( &childBezier[1][0]
                                                                      , &childBezier[1][1]
                                                                      , &childBezier[1][2]
                                                                      , &childBezier[1][3]
                                                                      , 0.5f );
            BuildVariableAdaptive( splitsAt
                                 , iToT
                                 , radiusAt
                                 , iRadiusTo
                                 , childBezier[1]
                                 , tangent
                                 , iNormalizedTangentTo
                                 , iRecurseDepth + 1 );
        }
    }
    else
    {
        uint32 polyCount = mFractionCache.size();
        FOdysseyVectorFraction* fraction;

        mFractionCache.emplace_back();

        fraction = &mFractionCache[polyCount];

        fraction->lineVertex[0] = iBezier[0];
        fraction->lineVertex[1] = iBezier[3];
        fraction->fromT = iFromT;
        fraction->toT = iToT;

        ThickenFraction( fraction );
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
        oBezierFragmentArray.emplace_back( iBezier, iFromT, iToT );
    }
}

void
FOdysseyVectorSegmentCubic::PrepareOffsetBeziers( double iSegmentStartRadius
                                                , double iSegmentEndRadius
                                                , const FOdysseyVectorBezierFragment& iFragment
                                                , FOdysseyVectorBezierFragment& oFragment0
                                                , FOdysseyVectorBezierFragment& oFragment1 )
{
    ::ULIS::FVec2D tangent[2] = { ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( iFragment.bezier[0]
                                                                                       , iFragment.bezier[1]
                                                                                       , iFragment.bezier[2]
                                                                                       , iFragment.bezier[3]
                                                                                       , 0.0f )
                                , ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( iFragment.bezier[0]
                                                                                       , iFragment.bezier[1]
                                                                                       , iFragment.bezier[2]
                                                                                       , iFragment.bezier[3]
                                                                                       , 1.0f ) };
    ::ULIS::FVec2D perpendicular[2] = { ::ULIS::FVec2D( -tangent[0].y, tangent[0].x )
                                      , ::ULIS::FVec2D( -tangent[1].y, tangent[1].x ) };
    ::ULIS::FVec2D tanvec[2] = { iFragment.bezier[1] - iFragment.bezier[0]
                               , iFragment.bezier[2] - iFragment.bezier[3] };
    //double ratio[2];
    ::ULIS::FVec2D widthPointStart[2];
    ::ULIS::FVec2D widthPointEnd[2];
    double fragmentStartRadius = ( iSegmentStartRadius * ( 1.0f - iFragment.fromT ) )
                               + ( iSegmentEndRadius * iFragment.fromT );
    double fragmentEndRadius = ( iSegmentStartRadius * ( 1.0f - iFragment.toT ) )
                             + ( iSegmentEndRadius * iFragment.toT );
    //--

    widthPointStart[0] = iFragment.bezier[0];
    widthPointStart[1] = iFragment.bezier[0];
    widthPointEnd[0]   = iFragment.bezier[3];
    widthPointEnd[1]   = iFragment.bezier[3];

    if( perpendicular[0].Distance() )
    {
        perpendicular[0].Normalize();

        widthPointStart[0] = iFragment.bezier[0] + ( perpendicular[0] * fragmentStartRadius );
        widthPointStart[1] = iFragment.bezier[0] - ( perpendicular[0] * fragmentStartRadius );
    }

    if( perpendicular[1].Distance() )
    {
        perpendicular[1].Normalize();

        widthPointEnd[0] = iFragment.bezier[3] + ( perpendicular[1] * fragmentEndRadius );
        widthPointEnd[1] = iFragment.bezier[3] - ( perpendicular[1] * fragmentEndRadius );
    }

    oFragment0.bezier[0] = oFragment0.bezier[1] = widthPointStart[0];
    oFragment0.bezier[3] = oFragment0.bezier[2] = widthPointEnd[0];
    oFragment0.straightLength = ::ULIS::FVec2D( oFragment0.bezier[3] - oFragment0.bezier[0] ).Distance();
    oFragment0.fromT = iFragment.fromT;
    oFragment0.toT = iFragment.toT;


    oFragment1.bezier[0] = oFragment1.bezier[1] = widthPointStart[1];
    oFragment1.bezier[3] = oFragment1.bezier[2] = widthPointEnd[1];
    oFragment1.straightLength = ::ULIS::FVec2D( oFragment1.bezier[3] - oFragment1.bezier[0] ).Distance();
    oFragment1.fromT = iFragment.fromT;
    oFragment1.toT = iFragment.toT;
}

// static
void
FOdysseyVectorSegmentCubic::GetOffsetPoints( ::ULIS::FVec2D& iPoint0
                                           , ::ULIS::FVec2D& iPoint1
                                           , ::ULIS::FVec2D& iPoint2
                                           , ::ULIS::FVec2D& iPoint3
                                           , double iRadius0
                                           , double iRadius3
                                           , double iSampleT
                                           , ::ULIS::FVec2D* oPointOut0
                                           , ::ULIS::FVec2D* oPointOut1 )
{
    ::ULIS::FVec2D tangent = ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( iPoint0
                                                                                  , iPoint1
                                                                                  , iPoint2
                                                                                  , iPoint3
                                                                                  , iSampleT );
    ::ULIS::FVec2D sample = ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( iPoint0
                                                                               , iPoint1
                                                                               , iPoint2
                                                                               , iPoint3
                                                                               , iSampleT );
    ::ULIS::FVec2D perpendicular = ::ULIS::FVec2D( -tangent.y, tangent.x );
    double sampleRadius = ( iSampleT * iRadius3 ) + ( ( 1.0f - iSampleT ) * iRadius0 );

    if( perpendicular.Distance() )
    {
        perpendicular.Normalize();

        *oPointOut0 = sample + ( perpendicular * sampleRadius );
        *oPointOut1 = sample - ( perpendicular * sampleRadius );
    }
}

void
FOdysseyVectorSegmentCubic::SmoothOffsetCurvesFragments( FOdysseyVectorBezierFragment* iBackwardFragment
                                                       , FOdysseyVectorBezierFragment* iForwardFragment
                                                       , const ::ULIS::FVec2D& iTangent )
{
    if( iBackwardFragment )
    {
        iBackwardFragment->bezier[2] = iBackwardFragment->bezier[3] - ( iTangent * iBackwardFragment->straightLength * 0.35f );
    }

    if( iForwardFragment )
    {
        iForwardFragment->bezier[1] = iForwardFragment->bezier[0] + ( iTangent * iForwardFragment->straightLength * 0.35f );
    }
}

void
FOdysseyVectorSegmentCubic::SmoothOffsetCurves( const std::vector<FOdysseyVectorBezierFragment>& iGuideBezierFragmentArray )
{
    std::vector<FOdysseyVectorBezierFragment>& offsetCurve0BezierFragmentArray = mOffsetCurve[0].GetBezierFragmentArray();
    std::vector<FOdysseyVectorBezierFragment>& offsetCurve1BezierFragmentArray = mOffsetCurve[1].GetBezierFragmentArray();
    FOdysseyVectorSegment* prevSegment = GetVertex(0)->GetOtherSegment( this );
    FOdysseyVectorSegment* nextSegment = GetVertex(1)->GetOtherSegment( this );
    ::ULIS::FVec2D firstOffsetPoint[2];
    ::ULIS::FVec2D lastOffsetPoint[2];
    // Note, it does note matter that firstOffsetPoint[2] is uninitialized, it will be afterwards
    //::ULIS::FVec2D* prevOffsetPoint[2] = { &firstOffsetPoint[0], &firstOffsetPoint[1] };
    //::ULIS::FVec2D* nextOffsetPoint[2] = { nullptr             , nullptr             };
    double segmentStartRadius = mPoint[0]->GetRadius();
    double segmentEndRadius = mPoint[1]->GetRadius();
    int guideBezierFragmentCount = iGuideBezierFragmentArray.size();
    ::ULIS::FVec2D* prevOffsetPoint[2] = { nullptr, nullptr };
    bool firstSmooth = false;
    bool lastSmooth = false;

    // retrieve a sample point before this segment, which will help us compute the tangent
    // that we use for smoothing the bezier segment at endpoint
    if( GetVertex(0)->IsHandleAligned() && prevSegment && prevSegment->HasBaseClass( FOdysseyVectorSegmentCubic::StaticClass() ) )
    {
        FOdysseyVectorSegmentCubic* prevCubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(prevSegment);
        ::ULIS::FVec2D* prevBezier = prevCubicSegment->GetBezier();

        // previous segment is correctly oriented
        if( prevSegment->GetVertex(1) == mPoint[0] )
        {
            // static call
            FOdysseyVectorSegmentCubic::GetOffsetPoints( prevBezier[0]
                                                       , prevBezier[1]
                                                       , prevBezier[2]
                                                       , prevBezier[3]
                                                       , prevSegment->GetVertex(0)->GetRadius()
                                                       , prevSegment->GetVertex(1)->GetRadius() // could be replaced with segmentStartRadius
                                                       , 0.95f
                                                       , &firstOffsetPoint[0]
                                                       , &firstOffsetPoint[1] );
        }
        // previous segment is NOT correctly oriented
        else //if( prevSegment->GetVertex(0) == mPoint[0] )
        {
            FOdysseyVectorSegmentCubic::GetOffsetPoints( prevBezier[3]
                                                       , prevBezier[2]
                                                       , prevBezier[1]
                                                       , prevBezier[0]
                                                       , prevSegment->GetVertex(1)->GetRadius()
                                                       , prevSegment->GetVertex(0)->GetRadius() // could be replaced with segmentStartRadius
                                                       , 0.05f
                                                       , &firstOffsetPoint[0]
                                                       , &firstOffsetPoint[1] );
        }

        prevOffsetPoint[0] = &firstOffsetPoint[0];
        prevOffsetPoint[1] = &firstOffsetPoint[1];

        firstSmooth = true;
    }

    // retrieve a sample point after this segment, which will help us compute the tangent
    // that we use for smoothing the bezier segment at endpoint
    if( GetVertex(1)->IsHandleAligned() && nextSegment && nextSegment->HasBaseClass( FOdysseyVectorSegmentCubic::StaticClass() ) )
    {
        FOdysseyVectorSegmentCubic* nextCubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(nextSegment);
        ::ULIS::FVec2D* nextBezier = nextCubicSegment->GetBezier();

        // next segment is correctly oriented
        if( nextSegment->GetVertex(0) == mPoint[1] )
        {
            // static call
            FOdysseyVectorSegmentCubic::GetOffsetPoints( nextBezier[0]
                                                       , nextBezier[1]
                                                       , nextBezier[2]
                                                       , nextBezier[3]
                                                       , nextSegment->GetVertex(0)->GetRadius()
                                                       , nextSegment->GetVertex(1)->GetRadius() // could be replaced with segmentStartRadius
                                                       , 0.05f
                                                       , &lastOffsetPoint[0]
                                                       , &lastOffsetPoint[1] );
        }
        // previous segment is NOT correctly oriented
        else //if( nextSegment->GetVertex(1) == mPoint[1] )
        {
            FOdysseyVectorSegmentCubic::GetOffsetPoints( nextBezier[3]
                                                       , nextBezier[2]
                                                       , nextBezier[1]
                                                       , nextBezier[0]
                                                       , nextSegment->GetVertex(1)->GetRadius()
                                                       , nextSegment->GetVertex(0)->GetRadius() // could be replaced with segmentStartRadius
                                                       , 0.95f
                                                       , &lastOffsetPoint[0]
                                                       , &lastOffsetPoint[1] );
        }

        lastSmooth = true;
    }

    mOffsetCurve[0].Resize( guideBezierFragmentCount );
    mOffsetCurve[1].Resize( guideBezierFragmentCount );

    // technically, we could as well iterate using offsetCurve0BezierFragmentArray or offsetCurve1BezierFragmentArray
    // because they have the same number of fragments, but for consistency we iterate using iGuideBezierFragmentArray
    for( int i = 0; i <= guideBezierFragmentCount; i++ )
    {
        int p = i - 1;
        ::ULIS::FVec2D currVector[2] = { ::ULIS::FVec2D( 0.0f, 0.0f ), ::ULIS::FVec2D( 0.0f, 0.0f ) };
        ::ULIS::FVec2D prevVector[2] = { ::ULIS::FVec2D( 0.0f, 0.0f ), ::ULIS::FVec2D( 0.0f, 0.0f ) };
        FOdysseyVectorBezierFragment* currFragment[2] = { nullptr, nullptr };
        FOdysseyVectorBezierFragment* prevFragment[2] = { nullptr, nullptr };

        if( i < guideBezierFragmentCount )
        {
            PrepareOffsetBeziers( segmentStartRadius
                                , segmentEndRadius
                                , iGuideBezierFragmentArray[i]
                                , offsetCurve0BezierFragmentArray[i]
                                , offsetCurve1BezierFragmentArray[i] );

            if( firstSmooth )
            {
                prevVector[0] = offsetCurve0BezierFragmentArray[i].bezier[0] - *prevOffsetPoint[0];
                prevVector[1] = offsetCurve1BezierFragmentArray[i].bezier[0] - *prevOffsetPoint[1];
            }

            currVector[0] = offsetCurve0BezierFragmentArray[i].bezier[3] - offsetCurve0BezierFragmentArray[i].bezier[0];
            currVector[1] = offsetCurve1BezierFragmentArray[i].bezier[3] - offsetCurve1BezierFragmentArray[i].bezier[0];

            prevFragment[0] = ( i == 0 ) ? nullptr : &offsetCurve0BezierFragmentArray[p];
            prevFragment[1] = ( i == 0 ) ? nullptr : &offsetCurve1BezierFragmentArray[p];

            currFragment[0] = &offsetCurve0BezierFragmentArray[i];
            currFragment[1] = &offsetCurve1BezierFragmentArray[i];

            prevOffsetPoint[0] = &offsetCurve0BezierFragmentArray[i].bezier[0];
            prevOffsetPoint[1] = &offsetCurve1BezierFragmentArray[i].bezier[0];

            firstSmooth = true;
        }

        if( i == guideBezierFragmentCount )
        {
            prevVector[0] = offsetCurve0BezierFragmentArray[p].bezier[3] - offsetCurve0BezierFragmentArray[p].bezier[0];
            prevVector[1] = offsetCurve1BezierFragmentArray[p].bezier[3] - offsetCurve1BezierFragmentArray[p].bezier[0];

            if( lastSmooth )
            {
                currVector[0] = lastOffsetPoint[0] - offsetCurve0BezierFragmentArray[p].bezier[3];
                currVector[1] = lastOffsetPoint[1] - offsetCurve1BezierFragmentArray[p].bezier[3];
            }

            prevFragment[0] = &offsetCurve0BezierFragmentArray[p];
            prevFragment[1] = &offsetCurve1BezierFragmentArray[p];

            currFragment[0] = nullptr;
            currFragment[1] = nullptr;
        }

        if( prevVector[0].DistanceSquared() ) prevVector[0].Normalize();
        if( prevVector[1].DistanceSquared() ) prevVector[1].Normalize();
        if( currVector[0].DistanceSquared() ) currVector[0].Normalize();
        if( currVector[1].DistanceSquared() ) currVector[1].Normalize();

        ::ULIS::FVec2D tangent[2] = { prevVector[0] + currVector[0]
                                    , prevVector[1] + currVector[1] };

        if( tangent[0].DistanceSquared() ) tangent[0].Normalize();
        if( tangent[1].DistanceSquared() ) tangent[1].Normalize();

        if( prevVector[0].DotProduct( currVector[0] ) > 0.0f )
        {
            SmoothOffsetCurvesFragments( prevFragment[0]
                                       , currFragment[0]
                                       , tangent[0] );
        }

        if( prevVector[1].DotProduct( currVector[1] ) > 0.0f )
        {
            SmoothOffsetCurvesFragments( prevFragment[1]
                                       , currFragment[1]
                                       , tangent[1] );
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
FOdysseyVectorSegmentCubic::Update()
{
    FOdysseyVectorSegment::Update();

    mBezier[0] = mPoint[0]->GetCoords();
    mBezier[1] = mCtrlPoint[0].GetCoords();
    mBezier[2] = mCtrlPoint[1].GetCoords();
    mBezier[3] = mPoint[1]->GetCoords();

    if( mNeedWidth )
    {
        BuildVariable();
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
FOdysseyVectorSegmentCubic::BuildVariable()
{
    double segmentStartRadius = static_cast<FOdysseyVectorVertex*>(mPoint[0])->GetRadius();
    double segmentEndRadius = static_cast<FOdysseyVectorVertex*>(mPoint[1])->GetRadius();
    ::ULIS::FVec2D tangent[2] = { ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( mBezier[0]
                                                                                       , mBezier[1]
                                                                                       , mBezier[2]
                                                                                       , mBezier[3]
                                                                                       , 0.0f )
                                , ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( mBezier[0]
                                                                                       , mBezier[1]
                                                                                       , mBezier[2]
                                                                                       , mBezier[3]
                                                                                       , 1.0f ) };

    if( tangent[0].DistanceSquared() )
    {
        tangent[0].Normalize();
    }

    if( tangent[1].DistanceSquared() )
    {
        tangent[1].Normalize();
    }

    ResetPolygonCache();
    UpdateBoundingBox();
    BuildOffsetCurves();
    BuildVariableAdaptive ( 0.0f
                          , 1.0f
                          , segmentStartRadius
                          , segmentEndRadius
                          , mBezier
                          , tangent[0]
                          , tangent[1]
                          , 0 );

    mLength = FOdysseyVector::GetBezierApproximateLength( mBezier, 8 );
}
