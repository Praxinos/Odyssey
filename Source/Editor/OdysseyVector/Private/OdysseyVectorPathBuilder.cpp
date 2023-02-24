#include "OdysseyVectorPathBuilder.h"

UOdysseyVectorPathBuilder::~UOdysseyVectorPathBuilder()
{

}

UOdysseyVectorPathBuilder::UOdysseyVectorPathBuilder()
    : mCubicPath ( nullptr )
    , mCumulAngle ( 0.0f )
    , mCumulAngleLimit ( 1.5708f ) // 90 degrees
    , mLastCubicAngleLimit ( 1.0472f ) // 60 deg
{
    Foreground.R = 128;
    Foreground.G = 128;
    Foreground.B = 128;
    Foreground.A = 255;
}

void 
UOdysseyVectorPathBuilder::Attach( UOdysseyVectorPathCubic* iCubicPath )
{
    mCubicPath = iCubicPath;
}

UOdysseyVectorLink*
UOdysseyVectorPathBuilder::GetLastSampleLink()
{
    if( mSampleLinkList.size() == 0 ) return nullptr;

    return mSampleLinkList.back();
}

UOdysseyVectorPoint*
UOdysseyVectorPathBuilder::GetLastSamplePoint()
{
    if( mSamplePointList.size() == 0 ) return nullptr;

    return mSamplePointList.back();
}

UOdysseyVectorObject*
UOdysseyVectorPathBuilder::CopyShape()
{
    return nullptr;
}

void
UOdysseyVectorPathBuilder::Sharp( UOdysseyVectorSegmentCubic& iCubicSegment
                                , ::ULIS::FVec2D iEntryVector
                                , ::ULIS::FVec2D iExitVector )
{
    double length = iCubicSegment.GetVector( false ).Distance();

    iCubicSegment.GetControlPoint(0)->Set( iCubicSegment.GetPoint(0)->GetX() + iEntryVector.x * length * 0.35f
                                         , iCubicSegment.GetPoint(0)->GetY() + iEntryVector.y * length * 0.35f );

    iCubicSegment.GetControlPoint(1)->Set( iCubicSegment.GetPoint(1)->GetX() - iExitVector.x * length * 0.35f
                                         , iCubicSegment.GetPoint(1)->GetY() - iExitVector.y * length * 0.35f );

    iCubicSegment.Update();
}

double
UOdysseyVectorPathBuilder::GetTotalSampleLinkLength()
{
    double length = 0.0f;

    for( std::list<UOdysseyVectorLink*>::iterator it = mSampleLinkList.begin(); it != mSampleLinkList.end(); ++it )
    {
        UOdysseyVectorLink *link = (*it);

        length += link->GetStraightDistance();
    }

    return length;
}

::ULIS::FVec2D
UOdysseyVectorPathBuilder::GetSamplePointAtParameter( double iToTalLinkLength, double iT )
{
    ::ULIS::FVec2D point = { 0.0f, 0.0f };
    double currentT = 0.0f;

    for( std::list<UOdysseyVectorLink*>::iterator it = mSampleLinkList.begin(); it != mSampleLinkList.end(); ++it )
    {
        UOdysseyVectorLink *link = (*it);
        double t = currentT + ( link->GetStraightDistance() / iToTalLinkLength );

        if ( ( iT >= currentT ) && ( iT <= t ) )
        {
            double deltaT = iT - currentT;
            ::ULIS::FVec2D linkVec = link->GetVector( false );

            point.x = link->GetPoint(0)->GetX() + ( deltaT * linkVec.x );
            point.y = link->GetPoint(0)->GetY() + ( deltaT * linkVec.y );

            return point;
        }

        currentT = t;
    }

    return point;
}

/*
 * The idea here is to adjust the cubic curve if it drifts away too much from the sample points
 * How do we do that ? We have the samples points :
 *
 *      o o o o o o o
 *   A                 B
 *
 * and we have computed the cubic curve from entry vector and exit vector (first and last sample points).
 * The length of the handles by default is 0.35 the length of the distance between A and B.
 *      _____________
 *     /             \
 *    /               \
 *   A                 B
 *
 * We then pick a point P1 at position 0.33f and P2 at 0.66f on this cubic curve :
 *      _____________
 *     P1           P2
 *    /               \
 *   A                 B
 *
 * we compare this position with an interpolated sample point "s" located at 0.33f and 0.66f as well. 
 *      _____________
 *     P1           P2
 *    /   s1     s2   \
 *   A                 B
 *
 * from the angles between vectors [A-B,A-P1] and [A-B,A-s1], we get a ratio that we use to decrease the length of the handles.
 * same goes for A-P2 and A-s2. Not perfect but not bad. Works only when decreasing the handles' length though, not increasing.
 *      _____________
 *    /               \
 *   A                 B
 *
 */
void
UOdysseyVectorPathBuilder::Adjust( UOdysseyVectorSegmentCubic& iCubicSegment )
{
    double totalLinkLength = GetTotalSampleLinkLength();
    ::ULIS::FVec2D& point0 = iCubicSegment.GetPoint(0)->GetCoords();
    ::ULIS::FVec2D& point1 = iCubicSegment.GetPoint(1)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = iCubicSegment.GetControlPoint(0)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = iCubicSegment.GetControlPoint(1)->GetCoords();
    ::ULIS::FVec2D expectedPoint0 = CubicBezierPointAtParameter( point0, ctrlPoint0, ctrlPoint1, point1, 0.33f );
    ::ULIS::FVec2D sampledPoint0 = GetSamplePointAtParameter( totalLinkLength, 0.33f );
    ::ULIS::FVec2D p0ToExpectedPoint0 = expectedPoint0 - point0;
    ::ULIS::FVec2D p0ToSampledPoint0 = sampledPoint0 - point0;
    ::ULIS::FVec2D cubicSegmentVector = iCubicSegment.GetVector( true );

    if ( p0ToExpectedPoint0.DistanceSquared() && p0ToSampledPoint0.DistanceSquared() )
    {
        p0ToExpectedPoint0.Normalize();
        p0ToSampledPoint0.Normalize();

        double dot = p0ToSampledPoint0.DotProduct( cubicSegmentVector );
        double angle = acos( ULIS::FMath::Clamp<double>( dot, -1.0f, 1.0f ) );
        double refDot = p0ToExpectedPoint0.DotProduct ( cubicSegmentVector );
        double refAngle = acos( ULIS::FMath::Clamp<double>( refDot, -1.0f, 1.0f ) );

        if ( fabs ( refAngle ) > 0.0001f )
        {
            double ratio = angle / refAngle;

            if( ratio < 1.0f )
            {
                ::ULIS::FVec2D ctrlVec = ctrlPoint0 - point0;

                ctrlPoint0.x = point0.x + ( ctrlVec.x * ratio );
                ctrlPoint0.y = point0.y + ( ctrlVec.y * ratio );
            }
        }
    }

    ::ULIS::FVec2D expectedPoint1 = CubicBezierPointAtParameter( point0, ctrlPoint0, ctrlPoint1, point1, 0.66f );
    ::ULIS::FVec2D sampledPoint1 = GetSamplePointAtParameter( totalLinkLength, 0.66f );
    ::ULIS::FVec2D p1ToExpectedPoint1 = expectedPoint1 - point1;
    ::ULIS::FVec2D p1ToSampledPoint1 = sampledPoint1 - point1;

    if ( p1ToExpectedPoint1.DistanceSquared() && p1ToSampledPoint1.DistanceSquared() )
    {
        p1ToExpectedPoint1.Normalize();
        p1ToSampledPoint1.Normalize();

        double dot = p1ToSampledPoint1.DotProduct( - cubicSegmentVector );
        double angle = acos( ULIS::FMath::Clamp<double>( dot, -1.0f, 1.0f ) );
        double refDot = p1ToExpectedPoint1.DotProduct ( - cubicSegmentVector );
        double refAngle = acos( ULIS::FMath::Clamp<double>( refDot, -1.0f, 1.0f ) );

        if ( fabs ( refAngle ) > 0.0001f )
        {
            double ratio = angle / refAngle;

            if ( ratio < 1.0f )
            {
                ::ULIS::FVec2D ctrlVec = ctrlPoint1 - point1;

                ctrlPoint1.x = point1.x + ( ctrlVec.x * ratio );
                ctrlPoint1.y = point1.y + ( ctrlVec.y * ratio );
            }
        }
    }
}

UOdysseyVectorSegmentCubic*
UOdysseyVectorPathBuilder::Sample( UOdysseyVectorPoint* iSamplePoint, double iRadius, bool iEnforce )
{
    if ( mCubicPath )
    {
        UOdysseyVectorPoint* lastSamplePoint = GetLastSamplePoint();
        UOdysseyVectorSegmentCubic* lastCubicSegment = static_cast<UOdysseyVectorSegmentCubic*>(mCubicPath->GetLastSegment());
        UOdysseyVectorSegmentCubic* cubicSegment = nullptr;

        mSamplePointList.push_back( iSamplePoint );

        if ( lastSamplePoint )
        {
            UOdysseyVectorLink* lastSampleLink = GetLastSampleLink();
            UOdysseyVectorLink* sampleLink = UOdysseyVectorLink::New( lastSamplePoint, iSamplePoint );

            mSampleLinkList.push_back( sampleLink );

            if ( lastSampleLink )
            {
                ::ULIS::FVec2D lastSampleSegmentVector = lastSampleLink->GetVector( true );
                ::ULIS::FVec2D     sampleSegmentVector =     sampleLink->GetVector( true );
                double dot = lastSampleSegmentVector.DotProduct( sampleSegmentVector );
                double angle = acos( ULIS::FMath::Clamp<double>( dot, -1.0f, 1.0f ) );

                mCumulAngle += angle;

                if ( ( angle > mLastCubicAngleLimit ) || ( mCumulAngle >= mCumulAngleLimit ) || iEnforce )
                {
                    UOdysseyVectorVertexCubic* cubicVertex = UOdysseyVectorVertexCubic::New( lastSamplePoint->GetX()
                                                                                           , lastSamplePoint->GetY()
                                                                                           , iRadius );
                    ::ULIS::FVec2D entryVector = mSampleLinkList.front()->GetVector( true );
                    ::ULIS::FVec2D exitVector =  lastSampleSegmentVector;

                    cubicSegment = mCubicPath->AppendVertex( cubicVertex, true, true );

                    Sharp ( *cubicSegment
                           , entryVector
                           , exitVector );

                    if ( lastCubicSegment )
                    {
                        ::ULIS::FVec2D lastCubicSegmentVector = lastCubicSegment->GetVectorAtEnd( true );
                        double smoothDot = entryVector.DotProduct( lastCubicSegmentVector );
                        double smoothAngle = acos( ULIS::FMath::Clamp<double>( smoothDot, -1.0f, 1.0f ) );

                        if ( fabs(smoothAngle) < mLastCubicAngleLimit )
                        {
                            double length = cubicSegment->GetStraightDistance();

                            cubicSegment->GetControlPoint(0)->Set ( cubicSegment->GetPoint(0)->GetX() + ( lastCubicSegmentVector.x * length * 0.35f )
                                                                  , cubicSegment->GetPoint(0)->GetY() + ( lastCubicSegmentVector.y * length * 0.35f ) );
                        }
                    }

                    mCumulAngle = 0.0f;
                }
            }
        }

        return cubicSegment;
    }

    return nullptr;
}

void
UOdysseyVectorPathBuilder::ClearUntil( UOdysseyVectorPoint* iPoint )
{
    while ( mPointList.size() && ( mPointList.front() != iPoint ) )
    {
        mPointList.pop_front();
    }

    while ( mSamplePointList.size() && ( mSamplePointList.front() != iPoint ) )
    {
        mSamplePointList.pop_front();
    }

    while ( mLinkList.size() && ( mLinkList.front()->GetPoint(0) != iPoint ) )
    {
        mLinkList.pop_front();
    }

    while ( mSampleLinkList.size() && ( mSampleLinkList.front()->GetPoint(0) != iPoint ) )
    {
        mSampleLinkList.pop_front();
    }
}

UOdysseyVectorSegmentCubic*
UOdysseyVectorPathBuilder::AppendPoint( double iX
                                      , double iY
                                      , double iRadius
                                      , bool   iEnforce )
{
    UOdysseyVectorPoint* point = UOdysseyVectorPoint::New( iX, iY, iRadius );
    UOdysseyVectorPoint* lastPoint = ( mPointList.size() ) ? mPointList.back() : nullptr;
    UOdysseyVectorSegmentCubic* cubicSegment = nullptr;

    mPointList.push_back( point );

    if ( lastPoint == nullptr )
    {
        UOdysseyVectorVertexCubic* cubicVertex = UOdysseyVectorVertexCubic::New( point->GetX()
                                                                               , point->GetY()
                                                                               , iRadius );

        mCubicPath->AppendVertex( cubicVertex, false, false );

        Sample ( point, iRadius, iEnforce );
    }
    else
    {
        UOdysseyVectorLink* link = UOdysseyVectorLink::New( lastPoint, point );
        UOdysseyVectorPoint* lastSamplePoint = GetLastSamplePoint();
        ::ULIS::FVec2D dif = { iX - lastSamplePoint->GetX()
                             , iY - lastSamplePoint->GetY() };
        double length = dif.Distance();

        if ( length >= 12.0f || iEnforce == true )
        {
            cubicSegment = Sample ( point, iRadius, iEnforce );

            if ( cubicSegment )
            {
                /*FitSegment( *cubicSegment, mSampleLinkList );*/

                Adjust( *cubicSegment );
                ClearUntil ( lastSamplePoint );
            }
        }

        mLinkList.push_back ( link );
    }

    mCubicPath->Invalidate();

    return cubicSegment;
}

UOdysseyVectorSegment*
UOdysseyVectorPathBuilder::AppendPoint( double iX
                                      , double iY
                                      , double iRadius )
{
    return AppendPoint ( iX, iY, iRadius, false );
}

UOdysseyVectorPathCubic* 
UOdysseyVectorPathBuilder::GetCubicPath()
{
    return mCubicPath;
}

UOdysseyVectorSegment*
UOdysseyVectorPathBuilder::End( double iX
                              , double iY
                              , double iRadius
                              , bool iClose )
{
    UOdysseyVectorVertex* lastVertex = mCubicPath->GetLastVertex();
    UOdysseyVectorVertex* firstVertex = mCubicPath->GetFirstVertex();
    UOdysseyVectorSegment* firstSegment = mCubicPath->GetFirstSegment();
    UOdysseyVectorSegmentCubic* newSegment = nullptr;

    if ( lastVertex )
    {
        if ( lastVertex->GetX() == iX && lastVertex->GetY() == iY )
        {
            // We delete the last point to prevent both points being at the same location
            // which would fake the result of AppendPoint() with iEnforce = true
            mPointList.pop_back();
            mLinkList.pop_back();
        }

        AppendPoint ( iX, iY, iRadius, true );

        newSegment = static_cast<UOdysseyVectorSegmentCubic*>(mCubicPath->GetLastSegment());

        if ( iClose == true )
        {
            if ( firstSegment && newSegment )
            {
                UOdysseyVectorVertex* newVertex = mCubicPath->GetLastVertex();

                if ( firstVertex->GetSegmentCount() == 1 )
                {
                    UOdysseyVectorSegmentCubic* loopSegment = UOdysseyVectorSegmentCubic::New( mCubicPath
                                                                                             , static_cast<UOdysseyVectorVertexCubic*>(newVertex)
                                                                                             , static_cast<UOdysseyVectorVertexCubic*>(firstVertex) );

                    mCubicPath->AddSegment( loopSegment );
                }
            }
        }
    }

    return nullptr;
}

void
UOdysseyVectorPathBuilder::FitSegment( UOdysseyVectorSegmentCubic& iSegment
                                     , std::list<UOdysseyVectorLink*>& iLinkList )
{
    ::ULIS::FVec2D& point0 = iSegment.GetPoint(0)->GetCoords();
    ::ULIS::FVec2D& point1 = iSegment.GetPoint(1)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = iSegment.GetControlPoint(0)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = iSegment.GetControlPoint(1)->GetCoords();
    double sampleLength = 0.0f;
    double currentT = 0.0f;

    for( std::list<UOdysseyVectorLink*>::iterator it = iLinkList.begin(); it != iLinkList.end(); ++it )
    {
        UOdysseyVectorLink *link = (*it);

        sampleLength += link->GetStraightDistance();
    }

    if ( sampleLength )
    {
        double A1 = 0.0f, A2 = 0.0f, A12 = 0.0f;
        ::ULIS::FVec2D C1 = { 0.0f, 0.0f };
        ::ULIS::FVec2D C2 = { 0.0f, 0.0f };

        for( std::list<UOdysseyVectorLink*>::iterator it = iLinkList.begin(); it != iLinkList.end(); ++it )
        {
            UOdysseyVectorLink *link = (*it);
            double t = currentT + ( link->GetStraightDistance() / sampleLength );
            double ct = 1.0f - t;
            double t3 = pow ( t, 3 );
            double ct3 = pow ( ct, 3 );
            ::ULIS::FVec2D PC = { link->GetPoint(1)->GetX() - ( ct3 * point0.x ) - ( t3 * point1.x )
                        , link->GetPoint(1)->GetY() - ( ct3 * point0.y ) - ( t3 * point1.y ) };

            A1  += ( pow ( t, 2 ) * pow ( ( ct ), 4 ) );
            A2  += ( pow ( t, 4 ) * pow ( ( ct ), 2 ) );
            A12 += ( pow ( t, 3 ) * pow ( ( ct ), 3 ) );

            C1.x += ( 3 * t * pow ( ct, 2 ) * PC.x );
            C1.y += ( 3 * t * pow ( ct, 2 ) * PC.y );

            C2.x += ( 3 * pow ( t, 2 ) * ct * PC.x );
            C2.y += ( 3 * pow ( t, 2 ) * ct * PC.y );

            currentT = t;
        }

        A1  *= 9.0f;
        A2  *= 9.0f;
        A12 *= 9.0f;

        ctrlPoint0.x = ( A2 * C1.x - A12 * C2.x ) / ( A1 * A2 - A12 * A12 );
        ctrlPoint0.y = ( A2 * C1.y - A12 * C2.y ) / ( A1 * A2 - A12 * A12 );

        ctrlPoint1.x = ( A1 * C2.x - A12 * C1.x ) / ( A1 * A2 - A12 * A12 );
        ctrlPoint1.y = ( A1 * C2.y - A12 * C1.y ) / ( A1 * A2 - A12 * A12 );
    }
}

void
UOdysseyVectorPathBuilder::DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext* blctx = GetRoot()->GetEngine()->GetBLContext();
    BLPath path;
    BLRgba32 strokeColor;

    strokeColor.r = Foreground.R;
    strokeColor.g = Foreground.G;
    strokeColor.b = Foreground.B;
    strokeColor.a = Foreground.A;

    blctx->setCompOp(BL_COMP_OP_SRC_COPY);
    /*iBLContext.setFillStyle(BLRgba32(0xFFFFFFFF));
    iBLContext.setStrokeStyle(BLRgba32(0xFF000000));*/

    blctx->setStrokeStyle( strokeColor );

    for( std::list<UOdysseyVectorLink*>::iterator it = mLinkList.begin(); it != mLinkList.end(); ++it )
    {
        UOdysseyVectorLink *link = (*it);
        BLPoint point0;
        BLPoint point1;

        point0.x = link->GetPoint(0)->GetX();
        point0.y = link->GetPoint(0)->GetY();

        point1.x = link->GetPoint(1)->GetX();
        point1.y = link->GetPoint(1)->GetY();

        if ( it == mLinkList.begin() )
        {
            blctx->setStrokeWidth( link->GetPoint(0)->GetRadius() * 2.0f );
            path.moveTo( point0.x,point0.y );
        }

        path.lineTo( point1.x,point1.y );
    }

    blctx->strokePath( path );

    blctx->setFillStyle(BLRgba32(0xFFFF00FF));

    for( std::list<UOdysseyVectorPoint*>::iterator it = mSamplePointList.begin(); it != mSamplePointList.end(); ++it )
    {
        UOdysseyVectorPoint *samplePoint = (*it);

        blctx->fillRect( samplePoint->GetX() - 3, samplePoint->GetY() - 3, 6, 6  );
    }
}
