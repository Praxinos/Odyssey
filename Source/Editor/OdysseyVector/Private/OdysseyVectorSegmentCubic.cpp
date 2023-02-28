#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorPathCubic.h"

UOdysseyVectorSegmentCubic::~UOdysseyVectorSegmentCubic()
{

}

UOdysseyVectorSegmentCubic::UOdysseyVectorSegmentCubic()
{

}

void
UOdysseyVectorSegmentCubic::Init( UOdysseyVectorPathCubic* iPath
                                , UOdysseyVectorVertexCubic* iPoint0
                                , double iCtrlPoint0x
                                , double iCtrlPoint0y
                                , double iCtrlPoint1x
                                , double iCtrlPoint1y
                                , UOdysseyVectorVertexCubic* iPoint1 )
{
    UOdysseyVectorSegment::Init( iPath, iPoint0, iPoint1 );

    mCtrlPoint[0] = UOdysseyVectorHandleSegment::New( this, iCtrlPoint0x, iCtrlPoint0y );
    mCtrlPoint[1] = UOdysseyVectorHandleSegment::New( this, iCtrlPoint1x, iCtrlPoint1y );

    Update();
}

void
UOdysseyVectorSegmentCubic::Init( UOdysseyVectorPathCubic* iPath
                                , UOdysseyVectorVertexCubic* iPoint0
                                , UOdysseyVectorVertexCubic* iPoint1 )
{
    Init( iPath, iPoint0, iPoint0->GetX(), iPoint0->GetY(), iPoint1->GetX(), iPoint1->GetY(), iPoint1 );
}

//static
UOdysseyVectorSegmentCubic* 
UOdysseyVectorSegmentCubic::New( UOdysseyVectorPathCubic* iPath
                               , UOdysseyVectorVertexCubic* iPoint0
                               , double iCtrlPoint0x
                               , double iCtrlPoint0y
                               , double iCtrlPoint1x
                               , double iCtrlPoint1y
                               , UOdysseyVectorVertexCubic* iPoint1 )
{
    UOdysseyVectorSegmentCubic* cubicSegment = NewObject<UOdysseyVectorSegmentCubic>();

    cubicSegment->Init (iPath, iPoint0, iCtrlPoint0x, iCtrlPoint0y, iCtrlPoint1x, iCtrlPoint1y, iPoint1 );

    return cubicSegment;
}

//static
UOdysseyVectorSegmentCubic* 
UOdysseyVectorSegmentCubic::New( UOdysseyVectorPathCubic* iPath
                               , UOdysseyVectorVertexCubic* iPoint0
                               , UOdysseyVectorVertexCubic* iPoint1 )
{
    UOdysseyVectorSegmentCubic* cubicSegment = NewObject<UOdysseyVectorSegmentCubic>();

    cubicSegment->Init (iPath, iPoint0, iPoint1 );

    return cubicSegment;
}

::ULIS::FVec2D
UOdysseyVectorSegmentCubic::GetPointAt( double t )
{
    ::ULIS::FVec2D& point0 = mPoint[0]->GetCoords();
    ::ULIS::FVec2D& point1 = mPoint[1]->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = mCtrlPoint[0]->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = mCtrlPoint[1]->GetCoords();
    ::ULIS::FVec2D pointAt = ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( point0
                                                                                , ctrlPoint0
                                                                                , ctrlPoint1
                                                                                , point1
                                                                                , t );

    return pointAt;
}

::ULIS::FVec2D
UOdysseyVectorSegmentCubic::GetTangentAt( double t )
{
    ::ULIS::FVec2D& point0 = mPoint[0]->GetCoords();
    ::ULIS::FVec2D& point1 = mPoint[1]->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = mCtrlPoint[0]->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = mCtrlPoint[1]->GetCoords();
    ::ULIS::FVec2D tangentAt = ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( point0
                                                                                    , ctrlPoint0
                                                                                    , ctrlPoint1
                                                                                    , point1
                                                                                    , t );

    return tangentAt;
}

void
UOdysseyVectorSegmentCubic::IncreasePolygonCache( uint32 iSize )
{
     mPolygonCache.resize( mPolygonCache.size() + iSize );
}

void
UOdysseyVectorSegmentCubic::ResetPolygonCache( )
{
    mPolygonCache.clear();
}

bool
UOdysseyVectorSegmentCubic::Pick( double iX
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

::ULIS::FVec2D UOdysseyVectorSegmentCubic::GetVectorAtEnd( bool iNormalize )
{
    ::ULIS::FVec2D& point0 = mPoint[0]->GetCoords();
    ::ULIS::FVec2D& point1 = mPoint[1]->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = mCtrlPoint[0]->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = mCtrlPoint[1]->GetCoords();
    ::ULIS::FVec2D vec = ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( point0
                                                                              , ctrlPoint0
                                                                              , ctrlPoint1
                                                                              , point1
                                                                              , 0.9999f );
/*
    ::ULIS::FVec2D vec = { GetPoint(1)->GetX() - GetControlPoint(1).GetX(),
                   GetPoint(1)->GetY() - GetControlPoint(1).GetY() };
*/
    if( iNormalize && vec.DistanceSquared() )
    {
        vec.Normalize();
    }

    return vec;
}

bool
UOdysseyVectorSegmentCubic::ProximityTest( double iLocalX, double iLocalY, double iDistanceTolerance, double &oSmallestDistance )
{
    ::ULIS::FVec2D& point0 = mPoint[0]->GetCoords();
    ::ULIS::FVec2D& point1 = mPoint[1]->GetCoords();
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

    // test first end-point of the segment
    dx = ( iLocalX - point0.x );
    dy = ( iLocalY - point0.y );
    dist = sqrt( ( dx * dx ) + ( dy * dy ) );

    if( dist < smallestDistance )
    {
        smallestDistance = dist;
    }

    // test second end-point of the segment
    dx = ( iLocalX - point1.x );
    dy = ( iLocalY - point1.y );
    dist = sqrt( ( dx * dx ) + ( dy * dy ) );

    if( dist < smallestDistance )
    {
        smallestDistance = dist;
    }

    if( smallestDistance < iDistanceTolerance )
    {
        return true;
    }

    return false;
}

::ULIS::FVec2D
UOdysseyVectorSegmentCubic::GetVectorAtStart( bool iNormalize )
{
    ::ULIS::FVec2D& point0 = mPoint[0]->GetCoords();
    ::ULIS::FVec2D& point1 = mPoint[1]->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = mCtrlPoint[0]->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = mCtrlPoint[1]->GetCoords();
    ::ULIS::FVec2D vec = ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( point0
                                                                              , ctrlPoint0
                                                                              , ctrlPoint1
                                                                              , point1
                                                                              , 0.0001f );
/*
    ::ULIS::FVec2D vec = { GetControlPoint(0).GetX() - GetPoint(0)->GetX(),
                   GetControlPoint(0).GetY() - GetPoint(0)->GetY() };
*/
    if( iNormalize && vec.DistanceSquared() )
    {
        vec.Normalize();
    }

    return vec;
}

UOdysseyVectorSegmentCubic*
UOdysseyVectorSegmentCubic::Sample( double iFromT
                                  , double iFromRadius
                                  , double iToT
                                  , double itoRadius
                                  , std::vector<UOdysseyVectorVertexCubic*>& newVertexArray )
{
    ::ULIS::FVec2D& ctrlPoint0 = mCtrlPoint[0]->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = mCtrlPoint[1]->GetCoords();
    ::ULIS::FVec2D pointAt0 = GetPointAt( iFromT );
    ::ULIS::FVec2D pointAt1 = GetPointAt( iToT );

    UOdysseyVectorVertexCubic* vertex0 = ( iFromT == 0.0f ) ? static_cast<UOdysseyVectorVertexCubic*>(mPoint[0]) : UOdysseyVectorVertexCubic::New( pointAt0.x, pointAt0.y, iFromRadius );
    UOdysseyVectorVertexCubic* vertex1 = ( iToT   == 1.0f ) ? static_cast<UOdysseyVectorVertexCubic*>(mPoint[1]) : UOdysseyVectorVertexCubic::New( pointAt1.x, pointAt1.y, itoRadius   );
    UOdysseyVectorSegmentCubic* sampleSegment = UOdysseyVectorSegmentCubic::New( static_cast<UOdysseyVectorPathCubic*>(mPath), vertex0, ctrlPoint0.x, ctrlPoint0.y, ctrlPoint1.x, ctrlPoint1.y, vertex1 );
    ::ULIS::FVec2D& sampleCtrlPoint0 = sampleSegment->GetControlPoint(0)->GetCoords();
    ::ULIS::FVec2D& sampleCtrlPoint1 = sampleSegment->GetControlPoint(1)->GetCoords();
    ::ULIS::FVec2D& samplePoint0 = sampleSegment->GetPoint(0)->GetCoords();
    ::ULIS::FVec2D& samplePoint1 = sampleSegment->GetPoint(1)->GetCoords();

    if( vertex0 != static_cast<UOdysseyVectorVertexCubic*>(mPoint[0]) )
    {
        newVertexArray.push_back( vertex0 );
    }

    if( vertex1 != static_cast<UOdysseyVectorVertexCubic*>(mPoint[1]) )
    {
        newVertexArray.push_back( vertex1 );
    }

    ::ULIS::CubicBezierInverseSplitAtParameter<::ULIS::FVec2D>( &samplePoint0, &sampleCtrlPoint0, &sampleCtrlPoint1, &samplePoint1, iFromT );
    ::ULIS::CubicBezierSplitAtParameter       <::ULIS::FVec2D>( &samplePoint0, &sampleCtrlPoint0, &sampleCtrlPoint1, &samplePoint1, iToT   );

    return sampleSegment;
}

void
UOdysseyVectorSegmentCubic::UpdateBoundingBox ()
{
   mBBox.x = ULIS::FMath::Min4<double>( mPoint[0]->GetX() - mPoint[0]->GetRadius()
                                      , mCtrlPoint[0]->GetX()
                                      , mPoint[1]->GetX() - mPoint[1]->GetRadius()
                                      , mCtrlPoint[1]->GetX() );

   mBBox.y = ULIS::FMath::Min4<double>( mPoint[0]->GetY() - mPoint[0]->GetRadius()
                                      , mCtrlPoint[0]->GetY()
                                      , mPoint[1]->GetY() - mPoint[1]->GetRadius()
                                      , mCtrlPoint[1]->GetY() );

   mBBox.w = ULIS::FMath::Max4<double>( mPoint[0]->GetX() + mPoint[0]->GetRadius()
                                      , mCtrlPoint[0]->GetX()
                                      , mPoint[1]->GetX() + mPoint[1]->GetRadius()
                                      , mCtrlPoint[1]->GetX() ) - mBBox.x;

   mBBox.h = ULIS::FMath::Max4<double>( mPoint[0]->GetY() + mPoint[0]->GetRadius()
                                      , mCtrlPoint[0]->GetY()
                                      , mPoint[1]->GetY() + mPoint[1]->GetRadius()
                                      , mCtrlPoint[1]->GetY() ) - mBBox.y;
}

UOdysseyVectorHandleSegment*
UOdysseyVectorSegmentCubic::GetControlPoint( int iCtrlPointNum )
{
    return mCtrlPoint[iCtrlPointNum];
}

::ULIS::FRectD&
UOdysseyVectorSegmentCubic::GetBoundingBox( )
{
    return mBBox;
}

void
UOdysseyVectorSegmentCubic::IntersectPath( UOdysseyVectorPathCubic& iPath )
{
    std::list<UOdysseyVectorSegment*>& segmentList = iPath.GetSegmentList();

    for( std::list<UOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
    {
        UOdysseyVectorSegmentCubic* cubicSegment = static_cast<UOdysseyVectorSegmentCubic*>(*it);
        ::ULIS::FRectD bbox = this->GetBoundingBox() & cubicSegment->GetBoundingBox();

        /*if ( bbox.Area() > 0.0f )
        {*/
           /* if ( cubicSegment != this )  // commented out: a bezier can intersect itself
            {*/
              /*  Intersect( *cubicSegment ); */
           /* }*/
        /*}*/
    }
}

double
UOdysseyVectorSegmentCubic::GetDistanceSquared()
{
    return mDistanceSquared;
}

bool
UOdysseyVectorSegmentCubic::Cut( ::ULIS::FVec2D& linePoint0
                        , ::ULIS::FVec2D& linePoint1 )
{
    ::ULIS::FVec2D& point0 = mPoint[0]->GetCoords();
    ::ULIS::FVec2D& point1 = mPoint[1]->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = mCtrlPoint[0]->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = mCtrlPoint[1]->GetCoords();
    // we'll have 3 intersections at most and 2 points at tips.
    UOdysseyVectorVertexCubic* pointChain[5] = { static_cast<UOdysseyVectorVertexCubic*>(mPoint[0]), nullptr, nullptr, nullptr, nullptr };
    ::ULIS::FVec2D tangentChain[5] = { GetVectorAtStart( true ), { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f } };
    uint32 pointCount = 1;
    ::ULIS::FVec2D ctrlPoint0Vector = GetVectorAtStart( true );
    ::ULIS::FVec2D ctrlPoint1Vector = GetVectorAtEnd( true );

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
            UOdysseyVectorVertexCubic* newCubicPoint = UOdysseyVectorVertexCubic::New( pointAt.x, pointAt.y, 0.0f );
            ::ULIS::FVec2D newTangent = ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( point0
                                                                                             , ctrlPoint0
                                                                                             , ctrlPoint1
                                                                                             , point1
                                                                                             , segmentT );

            newCubicPoint->SetRadius( ( mPoint[0]->GetRadius() * segmentT ) + ( mPoint[1]->GetRadius() * ( 1.0f - segmentT ) ), false );

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
        pointChain[pointCount] = static_cast<UOdysseyVectorVertexCubic*>(mPoint[1]);

        mPath->RemoveSegment( this );

        for( uint32 i = 1; i < pointCount; i++ )
        {
            mPath->AddVertex( pointChain[i] );
        }

        for( uint32 i = 0; i < pointCount; i++ )
        {
            uint32 n = i + 1;
            UOdysseyVectorSegmentCubic* segment = UOdysseyVectorSegmentCubic::New( static_cast<UOdysseyVectorPathCubic*>(mPath)
                                                                                 , static_cast<UOdysseyVectorVertexCubic*>(pointChain[i])
                                                                                 , static_cast<UOdysseyVectorVertexCubic*>(pointChain[n]) );
            double distance = segment->GetStraightDistance();

            segment->GetControlPoint(0)->Set( segment->GetPoint(0)->GetX() + ( tangentChain[i].x * distance * 0.35f )
                                            , segment->GetPoint(0)->GetY() + ( tangentChain[i].y * distance * 0.35f ) );

            segment->GetControlPoint(1)->Set( segment->GetPoint(1)->GetX() - ( tangentChain[n].x * distance * 0.35f )
                                            , segment->GetPoint(1)->GetY() - ( tangentChain[n].y * distance * 0.35f ) );

            mPath->AddSegment( segment );
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

static uint32
CreateAlmostIntersection( FAlmostIntersect aisx[4]
                        , double iTolerance
                        , UOdysseyVectorSegmentCubic* segment
                        , UOdysseyVectorSegmentCubic* otherSegment
                        , std::vector<UOdysseyVectorVertexIntersection*>& iIntersectionVertexArray )
{
    uint32 intersectionCount = 0;

    for( int i = 0; i < 4; i++ )
    {

        if( aisx[i].distance <= iTolerance )
        {
            UOdysseyVectorVertexIntersection* intersectionVertex = NewObject<UOdysseyVectorVertexIntersection>();

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

uint32
UOdysseyVectorSegmentCubic::Intersect( UOdysseyVectorSegmentCubic& iOther
                                     , std::vector<UOdysseyVectorVertexIntersection*>& iIntersectionVertexArray )
{
    ::ULIS::FVec2D& point0 = mPoint[0]->GetCoords();
    ::ULIS::FVec2D& point1 = mPoint[1]->GetCoords();
    ::ULIS::FVec2D& otherPoint0 = iOther.mPoint[0]->GetCoords();
    ::ULIS::FVec2D& otherPoint1 = iOther.mPoint[1]->GetCoords();
    /*::ULIS::FVec2D ctrlPoint0 = { mCtrlPoint[0]->GetX(), mCtrlPoint[0]->GetY() };
    ::ULIS::FVec2D ctrlPoint1 = { mCtrlPoint[1]->GetX(), mCtrlPoint[1]->GetY() };*/
    uint32 intersectionCount = 0;
/*
    double shortestP0Distance = DBL_MAX;
    int shortP0SegmentIndex = -1;
    double shortestP1Distance = DBL_MAX;
    int shortP1SegmentIndex = -1;
*/
    FAlmostIntersect aisx[4];

    aisx[0].distance = DBL_MAX;
    aisx[1].distance = DBL_MAX;
    aisx[2].distance = DBL_MAX;
    aisx[3].distance = DBL_MAX;

    for ( int i = 0; i < mPolygonCache.size(); i++ )
    {
        FPolygon* poly = &mPolygonCache[i];
        int p = i - 1;
        int n = i + 1;

        for( int j = 0; j < iOther.mPolygonCache.size(); j++ )
        {
            FPolygon* interPoly = &iOther.mPolygonCache[j];
            double polySubT, interPolySubT;

            // to speed things up a bit
            /*if( ( poly->xmax > interPoly->xmin ) && ( poly->xmin < interPoly->xmax )
             && ( poly->ymax > interPoly->ymin ) && ( poly->ymin < interPoly->ymax ) )
            {*/
                if(   ( this != &iOther )
                // check this is not the same sub-segment or adjacent sub-segment, or else they would always intersect
                 || ( ( this == &iOther ) && ( ( i - j ) > 1 ) ) )
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
                            UOdysseyVectorVertexIntersection* intersectionVertex = NewObject<UOdysseyVectorVertexIntersection>();

                            iIntersectionVertexArray.push_back( intersectionVertex );

                            // AddSegment() MUST be called before AddIntersection because AddIntersection uses the value of t that is stored by AddSegment()
                            intersectionVertex->AddSegment (    this, segmentT );
                            intersectionVertex->AddSegment ( &iOther,  iOtherT );

                            this->AddIntersection ( intersectionVertex );
                            iOther.AddIntersection ( intersectionVertex );

                            intersectionCount++;
                        }
                    }
/////////////////////////////////////////////////
                    else
                    {
                        if( j == 0 )
                        {
                            double distance;
                            double t = FOdysseyVector::DistanceToSegment( otherPoint0
                                                        , poly->lineVertex[0]
                                                        , poly->lineVertex[1]
                                                        , distance );

                            if( ( t >= 0.0f ) && ( t <= 1.0f ) )
                                if( distance < aisx[0].distance )
                                {
                                    aisx[0].distance = distance;
                                    aisx[0].t        = poly->fromT + ( ( poly->toT - poly->fromT ) * t );
                                    aisx[0].otherT   = 0.0f;
                                }
                        }

                        if( j == ( iOther.mPolygonCache.size() - 1 ) )
                        {
                            double distance;
                            double t = FOdysseyVector::DistanceToSegment( otherPoint1
                                                        , poly->lineVertex[0]
                                                        , poly->lineVertex[1]
                                                        , distance );

                            if( ( t >= 0.0f ) && ( t <= 1.0f ) )
                                if( distance < aisx[1].distance )
                                {
                                    aisx[1].distance = distance;
                                    aisx[1].t        = poly->fromT + ( ( poly->toT - poly->fromT ) * t );
                                    aisx[1].otherT   = 1.0f;
                                }
                        }

                        if( i == 0 )
                        {
                            double distance;
                            double t = FOdysseyVector::DistanceToSegment( point0
                                                        , interPoly->lineVertex[0]
                                                        , interPoly->lineVertex[1]
                                                        , distance );

                            if( ( t >= 0.0f ) && ( t <= 1.0f ) )
                                if( distance < aisx[2].distance )
                                {
                                    aisx[2].distance = distance;
                                    aisx[2].t        = 0.0f;
                                    aisx[2].otherT   = interPoly->fromT + ( ( interPoly->toT - interPoly->fromT ) * t );
                                }
                        }

                        if( i == ( mPolygonCache.size() - 1 ) )
                        {
                            double distance;
                            double t = FOdysseyVector::DistanceToSegment( point1
                                                        , interPoly->lineVertex[0]
                                                        , interPoly->lineVertex[1]
                                                        , distance );

                            if( ( t >= 0.0f ) && ( t <= 1.0f ) )
                                if( distance < aisx[3].distance )
                                {
                                    aisx[3].distance = distance;
                                    aisx[3].t        = 1.0f;
                                    aisx[3].otherT   = interPoly->fromT + ( ( interPoly->toT - interPoly->fromT ) * t );
                                }
                        }
                    }
///////////////////////////////////
                }
           /* }*/
        }
    }

/*
    if( shortestP0Distance < 3.0f )
    {

    }
*/

    intersectionCount += CreateAlmostIntersection( aisx, 20.0f, this, &iOther, iIntersectionVertexArray );

    return intersectionCount;
}

void
UOdysseyVectorSegmentCubic::DrawIntersections ( UOdysseyVectorPathCubic* iPath
                                              , ::ULIS::FRectD &iRoi
                                              , double iZoomFactor )
{
    // NOTE: Might not be super fast to call this for each segment
    BLContext* blctx = iPath->GetRoot()->GetEngine()->GetBLContext();
    double intersectionSize = 4 * iZoomFactor;
    double intersectionHalfSize = intersectionSize * 0.5f;
    ::ULIS::FVec2D& point0 = mPoint[0]->GetCoords();
    ::ULIS::FVec2D& point1 = mPoint[1]->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = mCtrlPoint[0]->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = mCtrlPoint[1]->GetCoords();

    for( std::list<UOdysseyVectorVertexIntersection*>::iterator it = mIntersectionVertexList.begin(); it != mIntersectionVertexList.end(); ++it )
    {
        UOdysseyVectorVertexIntersection* intersectionVertex = static_cast<UOdysseyVectorVertexIntersection*>(*it);
        ::ULIS::FVec2D pt = intersectionVertex->GetPosition(*this);

        blctx->setFillStyle( BLRgba32( 0xFFFF8000 ) );
        blctx->fillRect( pt.x - intersectionHalfSize
                      , pt.y - intersectionHalfSize, intersectionSize, intersectionSize );
    }
}

void
UOdysseyVectorSegmentCubic::DrawStructure( UOdysseyVectorPathCubic* iPath
                                         , ::ULIS::FRectD &iRoi
                                         , double iFactorX
                                         , double iFactorY )
{
    // NOTE: Might not be super fast to call this for each segment
    BLContext* blctx = iPath->GetRoot()->GetEngine()->GetBLContext();
    BLPath path;
    BLPath ctrlPath0;
    BLPath ctrlPath1;
    BLPoint point0 = { mPoint[0]->GetX(), mPoint[0]->GetY() };
    BLPoint point1 = { mPoint[1]->GetX(), mPoint[1]->GetY() };
    BLPoint ctrlPoint0 = { mCtrlPoint[0]->GetX(), mCtrlPoint[0]->GetY() };
    BLPoint ctrlPoint1 = { mCtrlPoint[1]->GetX(), mCtrlPoint[1]->GetY() };
    double handleRadiusX = 4.0f * iFactorX;
    double handleRadiusY = 4.0f * iFactorY;
    double handleWidth = handleRadiusX * 2.0f;
    double handleHeight = handleRadiusY * 2.0f;
    ::ULIS::FVec2D factor = { iFactorX, iFactorY };

    blctx->setStrokeStyle(BLRgba32(0xFF00FF00));
    blctx->setStrokeWidth( factor.Distance() );

    // cubic path
    path.moveTo( point0.x, point0.y );
    path.cubicTo( ctrlPoint0.x
                , ctrlPoint0.y
                , ctrlPoint1.x
                , ctrlPoint1.y
                , point1.x
                , point1.y );

    blctx->strokePath( path );

    // line to control handle 0
    ctrlPath0.moveTo( point0.x, point0.y );
    ctrlPath0.lineTo( ctrlPoint0.x, ctrlPoint0.y );

    blctx->setStrokeStyle( BLRgba32( 0xFFFF0000 ) );

    blctx->strokePath( ctrlPath0 );

    // line to control handle 1
    ctrlPath1.moveTo( point1.x, point1.y );
    ctrlPath1.lineTo( ctrlPoint1.x, ctrlPoint1.y );

    blctx->strokePath( ctrlPath1 );

    // control handles
    blctx->setFillStyle( BLRgba32( 0xFFFF0000 ) );
    blctx->fillRect( ctrlPoint0.x - handleRadiusX, ctrlPoint0.y - handleRadiusY, handleWidth, handleHeight );
    blctx->fillRect( ctrlPoint1.x - handleRadiusX, ctrlPoint1.y - handleRadiusY, handleWidth, handleHeight );

    /*DrawIntersections (  iRoi, iZoomFactor );*/
}

uint32
UOdysseyVectorSegmentCubic::GetPolygonCount()
{
    return mPolygonCache.size();
}

std::vector<FPolygon>&
 UOdysseyVectorSegmentCubic::GetPolygonCache()
{
    return mPolygonCache;
}

void
UOdysseyVectorSegmentCubic::Draw( UOdysseyVectorPathCubic* iPath, ::ULIS::FRectD &iRoi )
{
    // NOTE: Might not be super fast to call this for each segment
    BLContext* blctx = iPath->GetRoot()->GetEngine()->GetBLContext();

    blctx->setStrokeWidth( 1.0f );

#ifdef UNUSED
    for ( int i = 0; i < mPolygonCache.size(); i++ )
    {
       /* int n = i + 1;*/

        // the stroke thing is very slow and slows the all thing, we have to find something better
        //iBLContext.strokePolygon( mPolygonCache[i].vertex, 4 );
        BLPoint pt[4] = { { mPolygonCache[i].quadVertex[0].x, mPolygonCache[i].quadVertex[0].y }
                        , { mPolygonCache[i].quadVertex[1].x, mPolygonCache[i].quadVertex[1].y }
                        , { mPolygonCache[i].quadVertex[2].x, mPolygonCache[i].quadVertex[2].y }
                        , { mPolygonCache[i].quadVertex[3].x, mPolygonCache[i].quadVertex[3].y } };

        /*iBLContext.strokeLine( pt[1].x
                             , pt[1].y
                             , pt[2].x
                             , pt[2].y );*/

        blctx->fillPolygon( pt, 4 );
    }
#endif

/*#ifdef UNUSED*/
    blctx->strokePath ( mBLPath );
    blctx->fillPath ( mBLPath );

/*#endif*/

}

void
UOdysseyVectorSegmentCubic::BuildVariableThickness( double iFromT
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
    ::ULIS::FVec2D& point0 = mPoint[0]->GetCoords();
    ::ULIS::FVec2D& point1 = mPoint[1]->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = mCtrlPoint[0]->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = mCtrlPoint[1]->GetCoords();
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
UOdysseyVectorSegmentCubic::BuildVariableAdaptive( double  iFromT
                                                 , double  iToT
                                                 , double  iStartRadius
                                                 , double  iEndRadius
                                                 , ::ULIS::FVec2D* iPrevSegmentVector
                                                 , ::ULIS::FVec2D* iNextSegmentVector
                                                 , int32   iMaxRecurseDepth
                                                 , int    *iPolygonID )
{
    ::ULIS::FVec2D& point0 = mPoint[0]->GetCoords();
    ::ULIS::FVec2D& point1 = mPoint[1]->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = mCtrlPoint[0]->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = mCtrlPoint[1]->GetCoords();
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

            mDistanceSquared += dist.DistanceSquared();

            nextSegmentVector = ( i == 0x02 ) ? iNextSegmentVector : &subTangent[n];

    /*printf("%f %f - %f %f\n", iFromT, iToT, sampleRange[i], sampleRange[n] );*/
                BuildVariableThickness( sampleRange[i]
                                      , sampleRange[n]
                                      , samplePoint[i]
                                      , samplePoint[n]
                                      , prevSegmentVector
                                      , nextSegmentVector
                                      , radius
                                      , radius + radiusStep
                                      , *iPolygonID );
    /*printf("%f %f\n", mPolygonCache[(*iPolygonID)].fromT, mPolygonCache[(*iPolygonID)].toT );*/
                (*iPolygonID)++; // insures polygons are ordered

            // Note: within the segment, the previous vector (at origin) is the segment's vector itself
            prevSegmentVector = &subTangent[n];

            radius += radiusStep;
        }
    }
}

void
UOdysseyVectorSegmentCubic::Update()
{
    BuildVariable();
}

void
UOdysseyVectorSegmentCubic::BuildVariable()
{
    double segmentStartRadius = static_cast<UOdysseyVectorVertexCubic*>(mPoint[0])->GetRadius();
    double segmentEndRadius = static_cast<UOdysseyVectorVertexCubic*>(mPoint[1])->GetRadius();
    static ::ULIS::FVec2D zeroVector = { 0.0f, 0.0f };
    int polygonID = 0;

    mDistanceSquared = 0.0f;

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
}