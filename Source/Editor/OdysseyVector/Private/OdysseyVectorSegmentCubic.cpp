#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorPathCubic.h"

#define MINRECURSE 4
#define MAXRECURSE 8

FOdysseyVectorSegmentCubic::~FOdysseyVectorSegmentCubic()
{

}

FOdysseyVectorSegmentCubic::FOdysseyVectorSegmentCubic()
    : FOdysseyVectorSegment()
{
}

FOdysseyVectorSegmentCubic::FOdysseyVectorSegmentCubic( FOdysseyVectorPathCubic* iPath
                                                      , FOdysseyVectorVertex* iPoint0
                                                      , double iCtrlPoint0x
                                                      , double iCtrlPoint0y
                                                      , double iCtrlPoint1x
                                                      , double iCtrlPoint1y
                                                      , FOdysseyVectorVertex* iPoint1 )
    : FOdysseyVectorSegment( iPath, iPoint0, iPoint1 )
{
    Init ( iPath, iPoint0, iCtrlPoint0x, iCtrlPoint0y, iCtrlPoint1x, iCtrlPoint1y, iPoint1 );
}

FOdysseyVectorSegmentCubic::FOdysseyVectorSegmentCubic( FOdysseyVectorPathCubic* iPath
                                                      , FOdysseyVectorVertex* iPoint0
                                                      , FOdysseyVectorVertex* iPoint1 )
    : FOdysseyVectorSegment( iPath, iPoint0, iPoint1 )
{
    Init( iPath, iPoint0, iPoint1 );
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
FOdysseyVectorSegmentCubic::Init( FOdysseyVectorPathCubic* iPath
                                , FOdysseyVectorVertex* iPoint0
                                , double iCtrlPoint0x
                                , double iCtrlPoint0y
                                , double iCtrlPoint1x
                                , double iCtrlPoint1y
                                , FOdysseyVectorVertex* iPoint1 )
{
    mPoint[0] = iPoint0;
    mPoint[1] = iPoint1;

    mCtrlPoint[0].Init( this, iCtrlPoint0x, iCtrlPoint0y );
    mCtrlPoint[1].Init( this, iCtrlPoint1x, iCtrlPoint1y );

    Invalidate();
}

void
FOdysseyVectorSegmentCubic::Init( FOdysseyVectorPathCubic* iPath
                                , FOdysseyVectorVertex* iPoint0
                                , FOdysseyVectorVertex* iPoint1 )
{
    Init( iPath, iPoint0, iPoint0->GetX(), iPoint0->GetY(), iPoint1->GetX(), iPoint1->GetY(), iPoint1 );
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
    ::ULIS::FVec2D tangent = ( iVertex == mPoint[0] ) ?  GetTangentAt( 0.0f )
                                                      : -GetTangentAt( 1.0f );

    if( iNormalize )
    {
        if( tangent.DistanceSquared() )
        {
            tangent.Normalize();
        }
    }

    return tangent;
}

::ULIS::FVec2D
FOdysseyVectorSegmentCubic::GetTangentAt( double t )
{
    ::ULIS::FVec2D& point0 = GetVertex(0)->GetCoords();
    ::ULIS::FVec2D& point1 = GetVertex(1)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = GetHandle(0)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = GetHandle(1)->GetCoords();

    // Special case when control point are located at endpoint (tangentequals 0 then).

    if( t == 0.0f )
    {
        ::ULIS::FVec2D dif0 = ctrlPoint0 - point0;

        if( dif0.Distance() == 0.0f )
        {
            t = 0.0001f;
        }
    }

    if( t == 1.0f )
    {
        ::ULIS::FVec2D dif1 = ctrlPoint1 - point1;

        if( dif1.Distance() == 0.0f )
        {
            t = 0.9999f;
        }
    }

    return ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( point0
                                                                , ctrlPoint0
                                                                , ctrlPoint1
                                                                , point1
                                                                , t );
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

bool
FOdysseyVectorSegmentCubic::Pick( double iX
                                , double iY
                                , double iRadius )
{
    for ( int i = 0; i < mPolygonCache.size(); i++ )
    {
        double refQuantity;
        bool collide = true;

        for ( int j = 0; j < 4; j++ )
        {
            int n = ( j + 1 ) % 4;
            ::ULIS::FVec2D vivn = { mPolygonCache[i].quadVertex[n].x - mPolygonCache[i].quadVertex[j].x
                                  , mPolygonCache[i].quadVertex[n].y - mPolygonCache[i].quadVertex[j].y };
            ::ULIS::FVec2D vivt = { iX - mPolygonCache[i].quadVertex[j].x
                                  , iY - mPolygonCache[i].quadVertex[j].y };
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
    FOdysseyVectorSegmentCubic* sampleSegment = new FOdysseyVectorSegmentCubic( static_cast<FOdysseyVectorPathCubic*>(mPath), vertex0, vertex1 );
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
FOdysseyVectorSegmentCubic::Cut( ::ULIS::FVec2D& linePoint0
                               , ::ULIS::FVec2D& linePoint1
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

        mPath->RemoveSegment( this );

        for( uint32 i = 1; i < pointCount; i++ )
        {
            mPath->AddVertex( pointChain[i] );

            oNewVertexArray.push_back( pointChain[i] );
        }

        for( uint32 i = 0; i < pointCount; i++ )
        {
            uint32 n = i + 1;
            FOdysseyVectorSegmentCubic* newSegment = new FOdysseyVectorSegmentCubic( static_cast<FOdysseyVectorPathCubic*>(mPath)
                                                                                   , static_cast<FOdysseyVectorVertex*>(pointChain[i])
                                                                                   , static_cast<FOdysseyVectorVertex*>(pointChain[n]) );
            ::ULIS::FVec2D& newSegmentPoint0 = newSegment->GetVertex(0)->GetCoords();
            ::ULIS::FVec2D& newSegmentPoint1 = newSegment->GetVertex(1)->GetCoords();
            ::ULIS::FVec2D& newSegmentCtrlPoint0 = newSegment->GetHandle(0)->GetCoords();
            ::ULIS::FVec2D& newSegmentCtrlPoint1 = newSegment->GetHandle(1)->GetCoords();

            FOdysseyVector::BezierExtract( point0, ctrlPoint0, ctrlPoint1, point1
                                         , tChain[i]
                                         , tChain[n]
                                         , newSegmentPoint0, newSegmentCtrlPoint0, newSegmentCtrlPoint1, newSegmentPoint1 );

            mPath->AddSegment( newSegment );

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

static double
DistanceToSegmentConstrained( const ::ULIS::FVec2D& iPt
                            , const ::ULIS::FVec2D& iSegmentP0
                            , const ::ULIS::FVec2D& iSegmentP1
                            , double&         oDistance)
{
    double t = FOdysseyVector::DistanceToSegment( iPt, iSegmentP0, iSegmentP1, oDistance );

    if( t < 0.0f )
    {
        t = 0.0f;

        oDistance = ( iSegmentP0 - iPt ).Distance();
    }


    if( t > 1.0f )
    {
        t = 1.0f;

        oDistance = ( iSegmentP1 - iPt ).Distance();
    }

    return t;
}

uint32
FOdysseyVectorSegmentCubic::Intersect( FOdysseyVectorSegment* iOther
                                     , double iTolerance
                                     , std::vector<FOdysseyVectorIntersection*>& iIntersectionArray )
{
    FOdysseyVectorVertex* vertex0 = GetVertex(0);
    FOdysseyVectorVertex* vertex1 = GetVertex(1);
    ::ULIS::FVec2D& point0 = vertex0->GetCoords();
    ::ULIS::FVec2D& point1 = vertex1->GetCoords();
    FOdysseyVectorVertex* otherVertex0 = iOther->GetVertex(0);
    FOdysseyVectorVertex* otherVertex1 = iOther->GetVertex(1);
    ::ULIS::FVec2D& otherPoint0 = otherVertex0->GetCoords();
    ::ULIS::FVec2D& otherPoint1 = otherVertex1->GetCoords();
    uint32 intersectionCount = 0;

    for ( int i = 0; i < mPolygonCache.size(); i++ )
    {
        FPolygon* poly = &mPolygonCache[i];
        int p = i - 1;
        int n = i + 1;

      if( iOther->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
      {
        FOdysseyVectorSegmentCubic* otherCubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(iOther);

        for( int j = 0; j < otherCubicSegment->mPolygonCache.size(); j++ )
        {
            FPolygon* interPoly = &otherCubicSegment->mPolygonCache[j];
            double polySubT, interPolySubT;

            // to speed things up a bit
            if( ( ( poly->xmax + iTolerance ) > ( interPoly->xmin - iTolerance ) ) && ( ( poly->xmin - iTolerance ) < ( interPoly->xmax + iTolerance ) )
             && ( ( poly->ymax + iTolerance ) > ( interPoly->ymin - iTolerance ) ) && ( ( poly->ymin - iTolerance ) < ( interPoly->ymax + iTolerance ) ) )
            {
                if(   ( this != iOther )
                // check this is not the same sub-segment or adjacent sub-segment, or else they would always intersect
                 || ( ( this == iOther ) && ( ( i - j ) > 1 ) ) )
                {
                    if ( FOdysseyVector::IntersectSegment ( poly->lineVertex[0]
                                                          , poly->lineVertex[1]
                                                          , interPoly->lineVertex[0]
                                                          , interPoly->lineVertex[1]
                                                          , &polySubT
                                                          , &interPolySubT ) )
                    {
                        ::ULIS::FVec2D polyVector = ( poly->lineVertex[1] - poly->lineVertex[0] );
                        ::ULIS::FVec2D coords = { poly->lineVertex[0].x + ( polyVector.x * polySubT )
                                                , poly->lineVertex[0].y + ( polyVector.y * polySubT ) };
                        double segmentT =      poly->fromT + (      polySubT * (      poly->toT -      poly->fromT ) );
                        double iOtherT  = interPoly->fromT + ( interPolySubT * ( interPoly->toT - interPoly->fromT ) );

                        if( ( segmentT != 0.0f && iOtherT != 1.0f )
                         && ( segmentT != 1.0f && iOtherT != 0.0f ) )
                        {
                            FOdysseyVectorVertexIntersection* intersectionVertex[2] = { new FOdysseyVectorVertexIntersection(   this->GetPath()/*nullptr*/, ( this == iOther ), coords.x, coords.y, segmentT )
                                                                                      , new FOdysseyVectorVertexIntersection( iOther->GetPath()/*nullptr*/, ( this == iOther ), coords.x, coords.y, iOtherT  ) };

                            iIntersectionArray.push_back( new FOdysseyVectorIntersection( intersectionVertex[0], intersectionVertex[1] ) );

                              this->AddIntersection( intersectionVertex[0] );
                            iOther->AddIntersection( intersectionVertex[1] );

                            intersectionCount++;

//BLPoint pt = iOther->GetPath()->GetParent()->GetWorldMatrix().mapPoint( coords.x, coords.y );
//UE_LOG(LogTemp, Warning, TEXT("Intersection: %f %f"), pt.x, pt.y );
                        }
                    }
/////////////////////////////// NEEDS REFACTORING !!!! //////////////////
                    /*else
                    {*/
                      if( iTolerance && ( this != otherCubicSegment ) ) // limitation: tolerance can only work with different segments, otherwise it's too complicated to have something coherent
                      {
                        if( ( j == 0 ) && ( otherVertex0->GetSegmentCount() == 1 ) )
                        {
                            double distance;
                            double t = DistanceToSegmentConstrained( otherPoint0
                                                                   , poly->lineVertex[0]
                                                                   , poly->lineVertex[1]
                                                                   , distance );

                            if( distance < otherVertex0->GetDistanceToNearestSegment() )
                            {
                                double segmentT = poly->fromT + ( ( poly->toT - poly->fromT ) * t );

                                otherVertex0->SetNearestSegment( this, distance, segmentT );
                            }
                        }

                        if( ( j == ( otherCubicSegment->mPolygonCache.size() - 1 ) ) && ( otherVertex1->GetSegmentCount() == 1 ) )
                        {
                            double distance;
                            double t = DistanceToSegmentConstrained( otherPoint1
                                                                   , poly->lineVertex[0]
                                                                   , poly->lineVertex[1]
                                                                   , distance );

                            if( distance < otherVertex1->GetDistanceToNearestSegment() )
                            {
                                double segmentT = poly->fromT + ( ( poly->toT - poly->fromT ) * t );

                                otherVertex1->SetNearestSegment( this, distance, segmentT );
                            }
                        }

                        if( ( i == 0 ) && ( vertex0->GetSegmentCount() == 1 ) )
                        {
                            double distance;
                            double t = DistanceToSegmentConstrained( point0
                                                                   , interPoly->lineVertex[0]
                                                                   , interPoly->lineVertex[1]
                                                                   , distance );

                            if( distance < vertex0->GetDistanceToNearestSegment() )
                            {
                                double otherSegmentT = interPoly->fromT + ( ( interPoly->toT - interPoly->fromT ) * t );

                                vertex0->SetNearestSegment( iOther, distance, otherSegmentT );
                            }
                        }

                        if( ( i == ( mPolygonCache.size() - 1 ) ) && ( vertex1->GetSegmentCount() == 1 ) )
                        {
                            double distance;
                            double t = DistanceToSegmentConstrained( point1
                                                                   , interPoly->lineVertex[0]
                                                                   , interPoly->lineVertex[1]
                                                                   , distance );

                            if( distance < vertex1->GetDistanceToNearestSegment() )
                            {
                                double otherSegmentT = interPoly->fromT + ( ( interPoly->toT - interPoly->fromT ) * t );

                                vertex1->SetNearestSegment( iOther, distance, otherSegmentT );
                            }
                        }
                      }
                    /*}*/
///////////////////////////////////
                }
            }
        }
      }
    }
/*
    if( iTolerance )
    {
        IntersectVertices( vertex0, otherVertex0, iTolerance );
        IntersectVertices( vertex0, otherVertex1, iTolerance );
        IntersectVertices( vertex1, otherVertex0, iTolerance );
        IntersectVertices( vertex1, otherVertex1, iTolerance );

        IntersectVertices( otherVertex0, vertex0, iTolerance );
        IntersectVertices( otherVertex0, vertex1, iTolerance );
        IntersectVertices( otherVertex1, vertex0, iTolerance );
        IntersectVertices( otherVertex1, vertex1, iTolerance );
    }
*/
/*
    if( shortestP0Distance < 3.0f )
    {

    }
*/
/*
    intersectionCount += CreateAlmostIntersection( aisx, iTolerance, this, &iOther, iIntersectionVertexArray );
*/
    return intersectionCount;
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
    FOdysseyVectorVertex* vertex0 = GetVertex(0);
    FOdysseyVectorVertex* vertex1 = GetVertex(1);
    FOdysseyVectorHandleSegment* handle0 = GetHandle(0);
    FOdysseyVectorHandleSegment* handle1 = GetHandle(1);
    BLPoint point0 = iWorld ? worldMatrix.mapPoint( vertex0->GetX(), vertex0->GetY() ) : BLPoint( vertex0->GetX(), vertex0->GetY() );
    BLPoint point1 = iWorld ? worldMatrix.mapPoint( vertex1->GetX(), vertex1->GetY() ) : BLPoint( vertex1->GetX(), vertex1->GetY() );
    BLPoint handlePoint0 = iWorld ? worldMatrix.mapPoint( handle0->GetX(), handle0->GetY() ) : BLPoint( handle0->GetX(), handle0->GetY() );
    BLPoint handlePoint1 = iWorld ? worldMatrix.mapPoint( handle1->GetX(), handle1->GetY() ) : BLPoint( handle1->GetX(), handle1->GetY() );
    BLPath path;

    path.moveTo( point0 );
    path.cubicTo( handlePoint0
                , handlePoint1
                , point1 );

    blctx->strokePath( path );
}

void
FOdysseyVectorSegmentCubic::Draw( )
{
    // NOTE: Might not be super fast to call this for each segment
    BLContext* blctx = mPath->GetScene()->GetEngine()->GetBLContext();
    uint32 segmentCount = GetVertex(0)->GetSegmentCount();
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

/*
    blctx->save();
    blctx->resetMatrix();
    blctx->setStrokeStyle( BLRgba32( 0xFF00FF00 ) );
    blctx->setStrokeWidth( 1.0f );
    for ( int i = 0; i < mPolygonCache.size(); i++ )
    {
        blctx->strokeLine( worldMatrix.mapPoint( mPolygonCache[i].lineVertex[0].x, mPolygonCache[i].lineVertex[0].y )
                         , worldMatrix.mapPoint( mPolygonCache[i].lineVertex[1].x, mPolygonCache[i].lineVertex[1].y ) );
    }
    blctx->restore();
*/
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

        // this may be a bit too memory-consuming. Don't know. Keep it for now.
        polygon->xmax = ::ULIS::FMath::Max( polygon->lineVertex[0].x, polygon->lineVertex[1].x );
        polygon->ymax = ::ULIS::FMath::Max( polygon->lineVertex[0].y, polygon->lineVertex[1].y );
        polygon->xmin = ::ULIS::FMath::Min( polygon->lineVertex[0].x, polygon->lineVertex[1].x );
        polygon->ymin = ::ULIS::FMath::Min( polygon->lineVertex[0].y, polygon->lineVertex[1].y );

        ThickenPolygon( polygon, iRadiusFrom, iRadiusTo, iNormalizedTangentFrom, iNormalizedTangentTo );
    }
}

void
FOdysseyVectorSegmentCubic::Update()
{
    FOdysseyVectorSegment::Update();

    BuildVariable();
}

void
FOdysseyVectorSegmentCubic::BuildVariable()
{
    double segmentStartRadius = static_cast<FOdysseyVectorVertex*>(mPoint[0])->GetRadius();
    double segmentEndRadius = static_cast<FOdysseyVectorVertex*>(mPoint[1])->GetRadius();
    static ::ULIS::FVec2D zeroVector = { 0.0f, 0.0f };
    ::ULIS::FVec2D bezier[4] = { mPoint[0]->GetCoords(), mCtrlPoint[0].GetCoords(), mCtrlPoint[1].GetCoords(), mPoint[1]->GetCoords() };
    ::ULIS::FVec2D tangent[2] = { ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( bezier[0]
                                                                                       , bezier[1]
                                                                                       , bezier[2]
                                                                                       , bezier[3]
                                                                                       , 0.0f )
                                , ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( bezier[0]
                                                                                       , bezier[1]
                                                                                       , bezier[2]
                                                                                       , bezier[3]
                                                                                       , 1.0f ) };

    ResetPolygonCache();
    UpdateBoundingBox();

    if( tangent[0].DistanceSquared() )
    {
        tangent[0].Normalize();
    }

    if( tangent[1].DistanceSquared() )
    {
        tangent[1].Normalize();
    }

    BuildVariableAdaptive ( 0.0f
                          , 1.0f
                          , segmentStartRadius
                          , segmentEndRadius
                          , bezier
                          , tangent[0]
                          , tangent[1]
                          , 0 );
}