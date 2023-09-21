#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorPath.h"

#define MINRECURSE 3
#define MAXRECURSE 7

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
     mPolygonCache.resize( mPolygonCache.size() + iSize );
}

void
FOdysseyVectorSegmentCubic::ResetPolygonCache( )
{
    mPolygonCache.clear();
    mPolygonCache.reserve( 200 );
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
    for ( int i = 0; i < mPolygonCache.size(); i++ )
    {
        double refQuantity = 0;
        bool collide = true;

        for ( int j = 0; j < 4; j++ )
        {
            int n = ( j + 1 ) % 4;
            ::ULIS::FVec2D vivn = { mPolygonCache[i].quadVertex[n].x - mPolygonCache[i].quadVertex[j].x
                                  , mPolygonCache[i].quadVertex[n].y - mPolygonCache[i].quadVertex[j].y };
            ::ULIS::FVec2D vivt = { iLocalX - mPolygonCache[i].quadVertex[j].x
                                  , iLocalY - mPolygonCache[i].quadVertex[j].y };
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

    for( uint32 i = 0; i < mPolygonCache.size(); i++ )
    {
        ::ULIS::FVec2D p0 = { mPolygonCache[i].lineVertex[0].x, mPolygonCache[i].lineVertex[0].y };
        ::ULIS::FVec2D p1 = { mPolygonCache[i].lineVertex[1].x, mPolygonCache[i].lineVertex[1].y };
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

    for( int i = 0; i < mPolygonCache.size(); i++ )
    {
        FPolygon* poly = &mPolygonCache[i];
        double polySubT, interPolySubT;

        if ( FOdysseyVector::IntersectSegment( poly->lineVertex[0]
                                             , poly->lineVertex[1]
                                             , linePoint0
                                             , linePoint1
                                             , &polySubT
                                             , &interPolySubT ) )
        {
            double segmentT = poly->fromT + ( polySubT * ( poly->toT - poly->fromT ) );
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

uint32
FOdysseyVectorSegmentCubic::GetPolygonCount()
{
    return mPolygonCache.size();
}

std::vector<FPolygon>&
 FOdysseyVectorSegmentCubic::GetPolygonCache()
{
    return mPolygonCache;
}

void
FOdysseyVectorSegmentCubic::DrawStructure( FOdysseyVectorObject* iParentObject, bool iWorld )
{
    BLContext* blctx = iParentObject->GetScene()->GetEngine()->GetBLContext();
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

    blctx->strokePath( path );
}

void
FOdysseyVectorSegmentCubic::MakeBLPath()
{
    mBLPath.clear();

    if( mPolygonCache.size() )
    {
        int i;

        mBLPath.moveTo( mPolygonCache[0].quadVertex[0].x
                      , mPolygonCache[0].quadVertex[0].y );

        for ( i = 0; i < mPolygonCache.size(); i++ )
        {
            mBLPath.lineTo( mPolygonCache[i].quadVertex[1].x
                          , mPolygonCache[i].quadVertex[1].y );
        }

        for ( --i ; i >= 0; i-- )
        {
            mBLPath.lineTo( mPolygonCache[i].quadVertex[2].x
                          , mPolygonCache[i].quadVertex[2].y );
        }

        mBLPath.lineTo( mPolygonCache[0].quadVertex[3].x
                      , mPolygonCache[0].quadVertex[3].y );

        mBLPath.close();
    }
}

void
FOdysseyVectorSegmentCubic::DrawPolygonCache()
{
    BLContext* blctx = mPath->GetScene()->GetEngine()->GetBLContext();
    BLMatrix2D& worldMatrix = mPath->GetWorldMatrix();

    for ( int i = 0; i < mPolygonCache.size(); i++ )
    {
        // the stroke thing is very slow and slows the all thing, we have to find something better
        //iBLContext.strokePolygon( mPolygonCache[i].vertex, 4 );
        BLPoint pt[4] = { { mPolygonCache[i].quadVertex[0].x, mPolygonCache[i].quadVertex[0].y }
                        , { mPolygonCache[i].quadVertex[1].x, mPolygonCache[i].quadVertex[1].y }
                        , { mPolygonCache[i].quadVertex[2].x, mPolygonCache[i].quadVertex[2].y }
                        , { mPolygonCache[i].quadVertex[3].x, mPolygonCache[i].quadVertex[3].y } };

        blctx->fillPolygon( pt, 4 );
    }

    // we draw lines between the polygons to correct the artefacts, otherwise there is a thin line between the polygons
    // line stroking is done in world coordinates because we need a 1 pixel width
    blctx->save();
    blctx->resetMatrix();
    blctx->setStrokeWidth( 1.0f );
    for ( int i = 1; i < mPolygonCache.size(); i++ )
    {
        blctx->strokeLine( worldMatrix.mapPoint( mPolygonCache[i].quadVertex[0].x, mPolygonCache[i].quadVertex[0].y )
                         , worldMatrix.mapPoint( mPolygonCache[i].quadVertex[3].x, mPolygonCache[i].quadVertex[3].y ) );
    }
    blctx->restore();
}

void
FOdysseyVectorSegmentCubic::Draw( )
{
    // NOTE: Might not be super fast to call this for each segment
    BLContext* blctx = mPath->GetScene()->GetEngine()->GetBLContext();

    blctx->fillPath( mBLPath );
#ifdef unused
    DrawPolygonCache();
#endif
}

static void
ThickenPolygon( FPolygon* iPolygon
              , double iRadiusFrom
              , double iRadiusTo
              , const ::ULIS::FVec2D& iNormalizedTangentFrom
              , const ::ULIS::FVec2D& iNormalizedTangentTo )
{
    ::ULIS::FVec2D perpendicularVecFrom = { iNormalizedTangentFrom.y, -iNormalizedTangentFrom.x };
    ::ULIS::FVec2D perpendicularVecTo   = { iNormalizedTangentTo.y  , -iNormalizedTangentTo.x   };

    perpendicularVecFrom *= iRadiusFrom;
    perpendicularVecTo   *= iRadiusTo;

    iPolygon->quadVertex[0].x = iPolygon->lineVertex[0].x + perpendicularVecFrom.x;
    iPolygon->quadVertex[0].y = iPolygon->lineVertex[0].y + perpendicularVecFrom.y;

    iPolygon->quadVertex[1].x = iPolygon->lineVertex[1].x + perpendicularVecTo.x;
    iPolygon->quadVertex[1].y = iPolygon->lineVertex[1].y + perpendicularVecTo.y;

    iPolygon->quadVertex[2].x = iPolygon->lineVertex[1].x - perpendicularVecTo.x;
    iPolygon->quadVertex[2].y = iPolygon->lineVertex[1].y - perpendicularVecTo.y;

    iPolygon->quadVertex[3].x = iPolygon->lineVertex[0].x - perpendicularVecFrom.x;
    iPolygon->quadVertex[3].y = iPolygon->lineVertex[0].y - perpendicularVecFrom.y;
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
    double dotLimit = 0.99975f;

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
        uint32 polyCount = mPolygonCache.size();
        FPolygon* polygon;

        mPolygonCache.emplace_back();

        polygon = &mPolygonCache[polyCount];

        polygon->lineVertex[0] = iBezier[0];
        polygon->lineVertex[1] = iBezier[3];
        polygon->fromT = iFromT;
        polygon->toT = iToT;

        // InParent
/*
        if( iInParentConversionMatrix )
        {
            BLPoint inParent0 = iInParentConversionMatrix->mapPoint( polygon->lineVertex[0].x
                                                                   , polygon->lineVertex[0].y );
            polygon->lineVertexInParent[0].x = inParent0.x;
            polygon->lineVertexInParent[0].y = inParent0.y;

            BLPoint inParent1 = iInParentConversionMatrix->mapPoint( polygon->lineVertex[1].x
                                                                   , polygon->lineVertex[1].y );
            polygon->lineVertexInParent[1].x = inParent1.x;
            polygon->lineVertexInParent[1].y = inParent1.y;
        }
*/
        // this may be a bit too memory-consuming. Don't know. Keep it for now.
/*
        polygon->xmax = ::ULIS::FMath::Max( polygon->lineVertex[0].x, polygon->lineVertex[1].x );
        polygon->ymax = ::ULIS::FMath::Max( polygon->lineVertex[0].y, polygon->lineVertex[1].y );
        polygon->xmin = ::ULIS::FMath::Min( polygon->lineVertex[0].x, polygon->lineVertex[1].x );
        polygon->ymin = ::ULIS::FMath::Min( polygon->lineVertex[0].y, polygon->lineVertex[1].y );
*/
        ThickenPolygon( polygon, iRadiusFrom, iRadiusTo, iNormalizedTangentFrom, iNormalizedTangentTo );
    }
}

::ULIS::FVec2D
FOdysseyVectorSegmentCubic::GetPolygonCacheStartPointInParent()
{
    if( mPolygonCache.size() )
    {
        uint32 index = 0;

        return ::ULIS::FVec2D( mPolygonCache[index].lineVertexInParent[0].x
                             , mPolygonCache[index].lineVertexInParent[0].y );
    }

    return ::ULIS::FVec2D( 0.0f, 0.0f );
}

::ULIS::FVec2D
FOdysseyVectorSegmentCubic::GetPolygonCacheEndPointInParent()
{
    if( mPolygonCache.size() )
    {
        uint32 index = mPolygonCache.size() - 1;

        return ::ULIS::FVec2D( mPolygonCache[index].lineVertexInParent[1].x
                             , mPolygonCache[index].lineVertexInParent[1].y );
    }

    return ::ULIS::FVec2D( 0.0f, 0.0f );
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

void
FOdysseyVectorSegmentCubic::BuildVariable()
{
    double segmentStartRadius = static_cast<FOdysseyVectorVertex*>(mPoint[0])->GetRadius();
    double segmentEndRadius = static_cast<FOdysseyVectorVertex*>(mPoint[1])->GetRadius();
    static ::ULIS::FVec2D zeroVector = { 0.0f, 0.0f };
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
#ifdef unused
    ::ULIS::FVec2D perpendicular[2] = { ::ULIS::FVec2D( -tangent[0].y, tangent[0].x )
                                      , ::ULIS::FVec2D( -tangent[1].y, tangent[1].x ) };
#endif

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

#ifdef unused
    mWidthBezier[0][0] = ::ULIS::FVec2D( 0.0f, 0.0f);
    mWidthBezier[0][1] = ::ULIS::FVec2D( 0.0f, 0.0f );
    mWidthBezier[0][2] = ::ULIS::FVec2D( 0.0f, 0.0f );
    mWidthBezier[0][3] = ::ULIS::FVec2D( 0.0f, 0.0f );

    if( perpendicular[0].Distance() && perpendicular[1].Distance() )
    {
        ::ULIS::FVec2D tanvec[2] = { bezier[1] - bezier[0], bezier[3] - bezier[2] };
        double straightDistance = GetStraightDistance();
        double widthBezierDistance;
        double ratio[2] = { 1.0f, 1.0f };

        perpendicular[0].Normalize();
        perpendicular[1].Normalize();

        mWidthBezier[0][0] = bezier[0] + ( perpendicular[0] * segmentStartRadius );
        mWidthBezier[0][3] = bezier[3] + ( perpendicular[1] * segmentEndRadius   );

        widthBezierDistance = ::ULIS::FVec2D( mWidthBezier[0][0] - mWidthBezier[0][3] ).Distance();

        ratio[0] = tanvec[0].Distance() / straightDistance;
        ratio[1] = tanvec[1].Distance() / straightDistance;
/*
        ratio[0] = ::ULIS::FVec2D( mWidthBezier[0][0].x - bezier[3].x
                                 , mWidthBezier[0][0].y - bezier[3].y ).Distance()
                 / ::ULIS::FVec2D( bezier[0].x - bezier[3].x
                                 , bezier[0].y - bezier[3].y ).Distance();


        ratio[1] = ::ULIS::FVec2D( mWidthBezier[0][3].x - bezier[0].x
                                 , mWidthBezier[0][3].y - bezier[0].y ).Distance()
                 / ::ULIS::FVec2D( bezier[0].x - bezier[3].x
                                 , bezier[0].y - bezier[3].y ).Distance();
*/
//UE_LOG(LogTemp, Warning, TEXT("Some warning message %f %f"), ratio[0], ratio[1] );
        tanvec[0].Normalize();
        tanvec[1].Normalize();

        mWidthBezier[0][1] = mWidthBezier[0][0] + ( tanvec[0] * ratio[0] * ratio[0] * widthBezierDistance );
        mWidthBezier[0][2] = mWidthBezier[0][3] - ( tanvec[1] * ratio[1] * ratio[1] * widthBezierDistance );
    }
#endif

    BuildVariableAdaptive ( 0.0f
                          , 1.0f
                          , segmentStartRadius
                          , segmentEndRadius
                          , mBezier
                          , tangent[0]
                          , tangent[1]
                          , 0 );

    MakeBLPath();
}

double
FOdysseyVectorSegmentCubic::GetApproximateLength( uint32 iDivisions )
{
    ::ULIS::FVec2D p0 = mPoint[0]->GetCoords();
    double step = 1.0f / iDivisions;
    double length = 0.0f;
    double t0 = 0.0f;

    for( uint32 i = 0; i < iDivisions; i++ )
    {
        double t1 = t0 + step;
        ::ULIS::FVec2D p1 = ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( mBezier[0]
                                                                                 , mBezier[1]
                                                                                 , mBezier[2]
                                                                                 , mBezier[3]
                                                                                 , t1 );
        length += ::ULIS::FVec2D( p1 - p0 ).Distance();

        t0 = t1;
        p0 = p1;
    }

    return length;
}
