#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorPathCubic.h"

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
    FOdysseyVectorSegmentCubic* sampleSegment = new FOdysseyVectorSegmentCubic( static_cast<FOdysseyVectorPathCubic*>(mPath), vertex0, ctrlPoint0.x, ctrlPoint0.y, ctrlPoint1.x, ctrlPoint1.y, vertex1 );
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

    ::ULIS::CubicBezierInverseSplitAtParameter<::ULIS::FVec2D>( &samplePoint0, &sampleCtrlPoint0, &sampleCtrlPoint1, &samplePoint1, iFromT );
    ::ULIS::CubicBezierSplitAtParameter       <::ULIS::FVec2D>( &samplePoint0, &sampleCtrlPoint0, &sampleCtrlPoint1, &samplePoint1, iToT   );

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
    ::ULIS::FVec2D tangentChain[5] = { GetVectorAtStart( true ), { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f } };
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
            ::ULIS::FVec2D newTangent = ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( point0
                                                                                             , ctrlPoint0
                                                                                             , ctrlPoint1
                                                                                             , point1
                                                                                             , segmentT );
            

            newCubicPoint->SetRadius( mPoint[0]->GetRadius() + ( difRadius * segmentT ) );

            if ( newTangent.DistanceSquared() )
            {
                newTangent.Normalize();
            }

            tangentChain[pointCount] = newTangent;
            pointChain[pointCount] = newCubicPoint;

            pointCount++;
        }
    }

    if ( pointCount > 1 )
    {
        tangentChain[pointCount] = GetVectorAtEnd( true );
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
            FOdysseyVectorSegmentCubic* segment = new FOdysseyVectorSegmentCubic( static_cast<FOdysseyVectorPathCubic*>(mPath)
                                                                                , static_cast<FOdysseyVectorVertex*>(pointChain[i])
                                                                                , static_cast<FOdysseyVectorVertex*>(pointChain[n]) );
            double distance = segment->GetStraightDistance();

            segment->GetHandle(0)->Set( segment->GetPoint(0)->GetX() + ( tangentChain[i].x * distance * 0.35f )
                                      , segment->GetPoint(0)->GetY() + ( tangentChain[i].y * distance * 0.35f ) );

            segment->GetHandle(1)->Set( segment->GetPoint(1)->GetX() - ( tangentChain[n].x * distance * 0.35f )
                                      , segment->GetPoint(1)->GetY() - ( tangentChain[n].y * distance * 0.35f ) );

            mPath->AddSegment( segment );

            oNewSegmentArray.push_back( segment );
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
        if( ( iVertex0->GetSegmentCount() == 1 ) && ( iVertex1->GetSegmentCount() == 1 ) )
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
                            FOdysseyVectorVertexIntersection* intersectionVertex[2] = { new FOdysseyVectorVertexIntersection( nullptr, ( this == iOther ), coords.x, coords.y, segmentT )
                                                                                      , new FOdysseyVectorVertexIntersection( nullptr, ( this == iOther ), coords.x, coords.y, iOtherT  ) };

                            iIntersectionArray.push_back( new FOdysseyVectorIntersection( intersectionVertex[0], intersectionVertex[1] ) );

                            this->AddIntersection ( intersectionVertex[0] );
                            iOther->AddIntersection ( intersectionVertex[1] );

                            intersectionCount++;
                        }
                    }
/////////////////////////////// NEEDS REFACTORING !!!! //////////////////
                    else
                    {
                      if( iTolerance )
                      {
                        if( ( j == 0 ) && ( otherVertex0->GetSegmentCount() == 1 ) )
                        {
                            double distance;
                            double t = FOdysseyVector::DistanceToSegment( otherPoint0
                                                                        , poly->lineVertex[0]
                                                                        , poly->lineVertex[1]
                                                                        , distance );

                            if( ( t >= 0.0f ) && ( t <= 1.0f ) )
                                if( distance < otherVertex0->GetDistanceToNearestSegment() )
                                {
                                    double segmentT = poly->fromT + ( ( poly->toT - poly->fromT ) * t );

                                    otherVertex0->SetNearestSegment( this, distance, segmentT );

                                    //aisx[0].distance = distance;
                                    //aisx[0].t        = poly->fromT + ( ( poly->toT - poly->fromT ) * t );
                                    //aisx[0].otherT   = 0.0f;
                                }
                        }

                        if( ( j == ( otherCubicSegment->mPolygonCache.size() - 1 ) ) && ( otherVertex1->GetSegmentCount() == 1 ) )
                        {
                            double distance;
                            double t = FOdysseyVector::DistanceToSegment( otherPoint1
                                                                        , poly->lineVertex[0]
                                                                        , poly->lineVertex[1]
                                                                        , distance );

                            if( ( t >= 0.0f ) && ( t <= 1.0f ) )
                                if( distance < otherVertex1->GetDistanceToNearestSegment() )
                                {
                                    double segmentT = poly->fromT + ( ( poly->toT - poly->fromT ) * t );

                                    otherVertex1->SetNearestSegment( this, distance, segmentT );

                                    //aisx[1].distance = distance;
                                    //aisx[1].t        = poly->fromT + ( ( poly->toT - poly->fromT ) * t );
                                    //aisx[1].otherT   = 1.0f;
                                }
                        }

                        if( ( i == 0 ) && ( vertex0->GetSegmentCount() == 1 ) )
                        {
                            double distance;
                            double t = FOdysseyVector::DistanceToSegment( point0
                                                                        , interPoly->lineVertex[0]
                                                                        , interPoly->lineVertex[1]
                                                                        , distance );

                            if( ( t >= 0.0f ) && ( t <= 1.0f ) )
                                if( distance < vertex0->GetDistanceToNearestSegment() )
                                {
                                    double otherSegmentT = interPoly->fromT + ( ( interPoly->toT - interPoly->fromT ) * t );

                                    vertex0->SetNearestSegment( iOther, distance, otherSegmentT );

                                    //aisx[2].distance = distance;
                                    //aisx[2].t        = 0.0f;
                                    //aisx[2].otherT   = interPoly->fromT + ( ( interPoly->toT - interPoly->fromT ) * t );
                                }
                        }

                        if( ( i == ( mPolygonCache.size() - 1 ) ) && ( vertex1->GetSegmentCount() == 1 ) )
                        {
                            double distance;
                            double t = FOdysseyVector::DistanceToSegment( point1
                                                                        , interPoly->lineVertex[0]
                                                                        , interPoly->lineVertex[1]
                                                                        , distance );

                            if( ( t >= 0.0f ) && ( t <= 1.0f ) )
                                if( distance < vertex1->GetDistanceToNearestSegment() )
                                {
                                    double otherSegmentT = interPoly->fromT + ( ( interPoly->toT - interPoly->fromT ) * t );

                                    vertex1->SetNearestSegment( iOther, distance, otherSegmentT );

                                    //aisx[3].distance = distance;
                                    //aisx[3].t        = 1.0f;
                                    //aisx[3].otherT   = interPoly->fromT + ( ( interPoly->toT - interPoly->fromT ) * t );
                                }
                        }
                      }
                    }
///////////////////////////////////
                }
            }
        }
      }
    }

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
FOdysseyVectorSegmentCubic::DrawStructure( FOdysseyVectorObject* iParentObject, ::ULIS::FRectD &iRoi, bool iWorld )
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
FOdysseyVectorSegmentCubic::Draw( ::ULIS::FRectD &iRoi )
{
    // NOTE: Might not be super fast to call this for each segment
    BLContext* blctx = mPath->GetScene()->GetEngine()->GetBLContext();
    uint32 segmentCount = GetVertex(0)->GetSegmentCount();
    BLMatrix2D& worldMatrix = mPath->GetWorldMatrix();

    for ( int i = 0; i < mPolygonCache.size(); i++ )
    {
       /* int n = i + 1;*/

        // the stroke thing is very slow and slows the all thing, we have to find something better
        //iBLContext.strokePolygon( mPolygonCache[i].vertex, 4 );
        BLPoint pt[4] = { { mPolygonCache[i].quadVertex[0].x, mPolygonCache[i].quadVertex[0].y }
                        , { mPolygonCache[i].quadVertex[1].x, mPolygonCache[i].quadVertex[1].y }
                        , { mPolygonCache[i].quadVertex[2].x, mPolygonCache[i].quadVertex[2].y }
                        , { mPolygonCache[i].quadVertex[3].x, mPolygonCache[i].quadVertex[3].y } };

        /*blctx->strokePolygon( pt, 4 );*/ // commented-out: makes the path too thick and creates artefacts
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

#ifdef UNUSED // commented-out : it makes path too thick and creates artefacts
    blctx->setFillRule( BL_FILL_RULE_NON_ZERO );

    //blctx->strokePath ( mBLPath );
    blctx->fillPath ( mBLPath );
#endif

}

void
FOdysseyVectorSegmentCubic::BuildVariableThickness( double iFromT
                                                  , double iToT
                                                  , ::ULIS::FVec2D& iFromPoint
                                                  , ::ULIS::FVec2D& iToPoint
                                                  , ::ULIS::FVec2D* iPrevSegmentVector
                                                  , ::ULIS::FVec2D* iNextSegmentVector
                                                  , double iStartRadius
                                                  , double iEndRadius
                                                  , int    iPolygonID )
{
    // TODO: use references for speed
    ::ULIS::FVec2D& point0 = GetVertex(0)->GetCoords();
    ::ULIS::FVec2D& point1 = GetVertex(1)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = GetHandle(0)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = GetHandle(1)->GetCoords();
    ::ULIS::FVec2D sampleTangent[2] = { ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( point0
                                                                                             , ctrlPoint0
                                                                                             , ctrlPoint1
                                                                                             , point1
                                                                                             , iFromT ),
                                        ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( point0
                                                                                             , ctrlPoint0
                                                                                             , ctrlPoint1
                                                                                             , point1
                                                                                             , ( iToT == 1.0f ) ? 0.999f : iToT ) };
    static ::ULIS::FVec2D zeroVector = { 0.0f, 0.0f };
    FPolygon* cachedPolygon = &mPolygonCache[iPolygonID];

    // Note: no need to normalize sampleTangent[] vectors. The averaging won't be any different than with a normalized version.
    ::ULIS::FVec2D parallelVecFrom[2] = { *iPrevSegmentVector, sampleTangent[0]    };
    ::ULIS::FVec2D   parallelVecTo[2] = { sampleTangent[1]   , *iNextSegmentVector };
    ::ULIS::FVec2D averageVecFrom = ( parallelVecFrom[0] + parallelVecFrom[1] ) * 0.5f;
    ::ULIS::FVec2D averageVecTo   = (   parallelVecTo[0] +   parallelVecTo[1] ) * 0.5f;
    ::ULIS::FVec2D perpendicularVecFrom = { averageVecFrom.y, -averageVecFrom.x };
    ::ULIS::FVec2D perpendicularVecTo = { averageVecTo.y, -averageVecTo.x };
    /*int prevPolygonID = iPolygonID - 1;*/

    if ( perpendicularVecFrom.DistanceSquared() && perpendicularVecTo.DistanceSquared() )
    {
        perpendicularVecFrom.Normalize();
        perpendicularVecTo.Normalize();
    }

    perpendicularVecFrom *= iStartRadius;
    perpendicularVecTo *= iEndRadius;

    cachedPolygon->quadVertex[0].x = iFromPoint.x + perpendicularVecFrom.x;
    cachedPolygon->quadVertex[0].y = iFromPoint.y + perpendicularVecFrom.y;

    cachedPolygon->quadVertex[1].x = iToPoint.x + perpendicularVecTo.x;
    cachedPolygon->quadVertex[1].y = iToPoint.y + perpendicularVecTo.y;

    cachedPolygon->quadVertex[2].x = iToPoint.x - perpendicularVecTo.x;
    cachedPolygon->quadVertex[2].y = iToPoint.y - perpendicularVecTo.y;

    cachedPolygon->quadVertex[3].x = iFromPoint.x - perpendicularVecFrom.x;
    cachedPolygon->quadVertex[3].y = iFromPoint.y - perpendicularVecFrom.y;

    cachedPolygon->lineVertex[0].x = iFromPoint.x;
    cachedPolygon->lineVertex[0].y = iFromPoint.y;

    cachedPolygon->lineVertex[1].x = iToPoint.x;
    cachedPolygon->lineVertex[1].y = iToPoint.y;

    cachedPolygon->xmax = ( cachedPolygon->lineVertex[0].x > cachedPolygon->lineVertex[1].x ) ? cachedPolygon->lineVertex[0].x
                                                                                              : cachedPolygon->lineVertex[1].x;
    cachedPolygon->ymax = ( cachedPolygon->lineVertex[0].y > cachedPolygon->lineVertex[1].y ) ? cachedPolygon->lineVertex[0].y
                                                                                              : cachedPolygon->lineVertex[1].y;
    cachedPolygon->xmin = ( cachedPolygon->lineVertex[0].x < cachedPolygon->lineVertex[1].x ) ? cachedPolygon->lineVertex[0].x
                                                                                              : cachedPolygon->lineVertex[1].x;
    cachedPolygon->ymin = ( cachedPolygon->lineVertex[0].y < cachedPolygon->lineVertex[1].y ) ? cachedPolygon->lineVertex[0].y
                                                                                              : cachedPolygon->lineVertex[1].y;

    cachedPolygon->fromT = iFromT;
    cachedPolygon->toT = iToT;
/*
printf("cached polygon: %d - %f %f - %f %f - %f %f - %f %f - %f %f\n", iPolygonID
                                                                , cachedPolygon->quadVertex[0].x
                                                                , cachedPolygon->quadVertex[0].y
                                                                , cachedPolygon->quadVertex[1].x
                                                                , cachedPolygon->quadVertex[1].y
                                                                , cachedPolygon->quadVertex[2].x
                                                                , cachedPolygon->quadVertex[2].y
                                                                , cachedPolygon->quadVertex[3].x
                                                                , cachedPolygon->quadVertex[3].y
                                                                , cachedPolygon->fromT
                                                                , cachedPolygon->toT );
*/
}

void
FOdysseyVectorSegmentCubic::BuildVariableAdaptive( double  iFromT
                                                 , double  iToT
                                                 , double  iStartRadius
                                                 , double  iEndRadius
                                                 , ::ULIS::FVec2D* iPrevSegmentVector
                                                 , ::ULIS::FVec2D* iNextSegmentVector
                                                 , int32   iMaxRecurseDepth
                                                 , int    *iPolygonID )
{
    ::ULIS::FVec2D& point0 = GetVertex(0)->GetCoords();
    ::ULIS::FVec2D& point1 = GetVertex(1)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = GetHandle(0)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = GetHandle(1)->GetCoords();
    double radiusDiff = iEndRadius - iStartRadius;
    double radiusStep = radiusDiff / 3;
    double rangeDiff = iToT - iFromT;
    double rangeStep = rangeDiff / 3;
    double sampleRange[4] = { iFromT, iFromT + rangeStep, iToT - rangeStep, iToT };
    ::ULIS::FVec2D samplePoint[4] = { ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( point0, ctrlPoint0, ctrlPoint1, point1, sampleRange[0] ),
                                      ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( point0, ctrlPoint0, ctrlPoint1, point1, sampleRange[1] ),
                                      ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( point0, ctrlPoint0, ctrlPoint1, point1, sampleRange[2] ),
                                      ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( point0, ctrlPoint0, ctrlPoint1, point1, sampleRange[3] ), };
    ::ULIS::FVec2D subSegment[3] = { { samplePoint[1].x - samplePoint[0].x, samplePoint[1].y - samplePoint[0].y },
                                     { samplePoint[2].x - samplePoint[1].x, samplePoint[2].y - samplePoint[1].y },
                                     { samplePoint[3].x - samplePoint[2].x, samplePoint[3].y - samplePoint[2].y } };
    ::ULIS::FVec2D subTangent[3] = { ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( point0, ctrlPoint0, ctrlPoint1, point1, sampleRange[0] ),
                                     ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( point0, ctrlPoint0, ctrlPoint1, point1, sampleRange[1] ),
                                     ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( point0, ctrlPoint0, ctrlPoint1, point1, sampleRange[2] ) };
    bool doRefine = false;
    double radius = iStartRadius;
    ::ULIS::FVec2D *prevSegmentVector = iPrevSegmentVector;
    ::ULIS::FVec2D *nextSegmentVector;
    double angleCosineLimit = 0.999f;
    static ::ULIS::FVec2D nilVector = { 0.0f, 0.0f };

    for( int i = 0; i < 3; i++ )
    {
        int n = ( i + 1 );
        double subTanDistanceSquared = subTangent[i].DistanceSquared();
        double subSegDistanceSquared = subSegment[i].DistanceSquared();



        if( /*subTanDistanceSquared &&*/ subSegDistanceSquared )
        {
            // Note: must be normalized in order to get a correct value for the dot product
            if ( subTanDistanceSquared )
            {
                subTangent[i].Normalize();
            }

            subSegment[i].Normalize();

            if( subTangent[i].DotProduct( subSegment[i] ) <= angleCosineLimit )
            {
                doRefine = true;
            }
        }
    }

    --iMaxRecurseDepth;

    if( ( doRefine == true ) && iMaxRecurseDepth >= 0 )
    {
        for( int i = 0; i < 3; i++ )
        {
            int n = ( i + 1 );

            nextSegmentVector = ( i == 0x02 ) ? iNextSegmentVector : &subTangent[n];

            BuildVariableAdaptive( sampleRange[i]
                                 , sampleRange[n]
                                 , radius
                                 , radius + radiusStep
                                 , prevSegmentVector
                                 , nextSegmentVector
                                 , iMaxRecurseDepth
                                 , iPolygonID );

            // Note: within the segment, the previous vector (at origin) is the segment's vector itself
            prevSegmentVector = &subTangent[n];

            radius += radiusStep;
        }
    }
    else
    {
        IncreasePolygonCache (3);

        /*printf("increasing cache %d\n",mPolygonCache.size());*/

        for( int i = 0; i < 3; i++ )
        {
            int n = ( i + 1 );
            ::ULIS::FVec2D dist = samplePoint[n] - samplePoint[i];

            nextSegmentVector = ( i == 0x02 ) ? iNextSegmentVector : &subTangent[n];

                BuildVariableThickness( sampleRange[i]
                                      , sampleRange[n]
                                      , samplePoint[i]
                                      , samplePoint[n]
                                      , prevSegmentVector
                                      , nextSegmentVector
                                      , radius
                                      , radius + radiusStep
                                      , *iPolygonID );

                (*iPolygonID)++; // insures polygons are ordered

            // Note: within the segment, the previous vector (at origin) is the segment's vector itself
            prevSegmentVector = &subTangent[n];

            radius += radiusStep;
        }
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
    int polygonID = 0;

    ResetPolygonCache();
    UpdateBoundingBox();

/*printf("\n");*/
    BuildVariableAdaptive ( 0.0f
                          , 1.0f
                          , segmentStartRadius
                          , segmentEndRadius
                          , &zeroVector
                          , &zeroVector
                          , 8
                          , &polygonID );
/*
    if ( mPolygonCache.size() )
    {
        mBLPath.clear();

        mBLPath.moveTo ( mPolygonCache[0].quadVertex[0].x
                       , mPolygonCache[0].quadVertex[0].y );

        for ( int i = 0; i < mPolygonCache.size(); i++ )
        {
            mBLPath.lineTo ( mPolygonCache[i].quadVertex[1].x
                           , mPolygonCache[i].quadVertex[1].y );
        }

        for ( int i = mPolygonCache.size(); --i >= 0; )
        {
            mBLPath.lineTo ( mPolygonCache[i].quadVertex[2].x
                           , mPolygonCache[i].quadVertex[2].y );
        }

        mBLPath.lineTo ( mPolygonCache[0].quadVertex[3].x
                       , mPolygonCache[0].quadVertex[3].y );

        mBLPath.close();
    }
*/
}