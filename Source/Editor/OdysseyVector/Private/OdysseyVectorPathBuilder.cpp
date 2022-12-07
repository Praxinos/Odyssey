#include "OdysseyVectorPathBuilder.h"

FOdysseyVectorPathBuilder::~FOdysseyVectorPathBuilder()
{

}

FOdysseyVectorPathBuilder::FOdysseyVectorPathBuilder( FOdysseyVectorPathCubic* iCubicPath )
    : mCubicPath ( iCubicPath )
    , mCumulAngle ( 0.0f )
    , mCumulAngleLimit ( 1.5708f ) // 90 degrees
    , mLastCubicAngleLimit ( 1.0472f ) // 60 deg 
    /*, mLastCubicAngleLimit ( 2.35619f )*/ // 135 degrees
{
    mStrokeColor = 0xFF808080;
}

FOdysseyVectorLink*
FOdysseyVectorPathBuilder::GetLastSampleLink()
{
    if( mSampleLinkList.size() == 0 ) return nullptr;

    return mSampleLinkList.back();
}

FOdysseyVectorPoint*
FOdysseyVectorPathBuilder::GetLastSamplePoint()
{
    if( mSamplePointList.size() == 0 ) return nullptr;

    return mSamplePointList.back();
}

FOdysseyVectorObject*
FOdysseyVectorPathBuilder::CopyShape()
{
    return nullptr;
}

void
FOdysseyVectorPathBuilder::Sharp( FOdysseyVectorSegmentCubic& iCubicSegment
                         , ::ULIS::FVec2D iEntryVector
                         , ::ULIS::FVec2D iExitVector )
{
    double length = iCubicSegment.GetVector( false ).Distance();

    iCubicSegment.GetControlPoint(0).Set( iCubicSegment.GetPoint(0)->GetX() + iEntryVector.x * length * 0.35f
                                        , iCubicSegment.GetPoint(0)->GetY() + iEntryVector.y * length * 0.35f );

    iCubicSegment.GetControlPoint(1).Set( iCubicSegment.GetPoint(1)->GetX() - iExitVector.x * length * 0.35f
                                        , iCubicSegment.GetPoint(1)->GetY() - iExitVector.y * length * 0.35f );

    iCubicSegment.Update();
}

double
FOdysseyVectorPathBuilder::GetTotalSampleLinkLength()
{
    double length = 0.0f;

    for( std::list<FOdysseyVectorLink*>::iterator it = mSampleLinkList.begin(); it != mSampleLinkList.end(); ++it )
    {
        FOdysseyVectorLink *link = (*it);

        length += link->GetStraightDistance();
    }

    return length;
}

::ULIS::FVec2D
FOdysseyVectorPathBuilder::GetSamplePointAtParameter( double iToTalLinkLength, double iT )
{
    ::ULIS::FVec2D point = { 0.0f, 0.0f };
    double currentT = 0.0f;

    for( std::list<FOdysseyVectorLink*>::iterator it = mSampleLinkList.begin(); it != mSampleLinkList.end(); ++it )
    {
        FOdysseyVectorLink *link = (*it);
        double t = currentT + ( link->GetStraightDistance() / iToTalLinkLength );

        if ( ( iT >= currentT ) && ( iT <= t ) )
        {
            double deltaT = iT - currentT;
            ::ULIS::FVec2D linkVec = link->GetVector( false );

            point.x = link->GetPoint(0)->GetX() + ( deltaT * linkVec.x );
            point.y = link->GetPoint(0)->GetY() + ( deltaT * linkVec.y );
printf("iToTalLinkLength:%f - deltaT:%f - x:%f, y:%f\n", iToTalLinkLength, deltaT, point.x, point.y );
            return point;
        }

        currentT = t;
    }

    return point;
}

void
FOdysseyVectorPathBuilder::Adjust( FOdysseyVectorSegmentCubic& iCubicSegment )
{
    double totalLinkLength = GetTotalSampleLinkLength();
    ::ULIS::FVec2D& point0 = iCubicSegment.GetPoint(0)->GetCoords();
    ::ULIS::FVec2D& point1 = iCubicSegment.GetPoint(1)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = iCubicSegment.GetControlPoint(0).GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = iCubicSegment.GetControlPoint(1).GetCoords();
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

printf("refAngle %f angle %f\n", refAngle, angle );
        if ( fabs ( refAngle ) > 0.0001f )
        {
            double ratio = angle / refAngle;

            if( ratio < 1.0f )
            {
                ::ULIS::FVec2D ctrlVec = ctrlPoint0 - point0;

    printf("ratio %f\n", ratio );
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

printf("refAngle %f angle %f\n", refAngle, angle );
        if ( fabs ( refAngle ) > 0.0001f )
        {
            double ratio = angle / refAngle;

            if ( ratio < 1.0f )
            {
                ::ULIS::FVec2D ctrlVec = ctrlPoint1 - point1;

                printf("ratio1 %f\n", ratio );
                ctrlPoint1.x = point1.x + ( ctrlVec.x * ratio );
                ctrlPoint1.y = point1.y + ( ctrlVec.y * ratio );
            }
        }
    }
}

FOdysseyVectorSegmentCubic*
FOdysseyVectorPathBuilder::Sample( FOdysseyVectorPoint* iSamplePoint, double iRadius, bool iEnforce )
{
    FOdysseyVectorPoint* lastSamplePoint = GetLastSamplePoint();
    FOdysseyVectorSegmentCubic* lastCubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(mCubicPath->GetLastSegment());
    FOdysseyVectorSegmentCubic* cubicSegment = nullptr;

    mSamplePointList.push_back( iSamplePoint );

    if ( lastSamplePoint )
    {
        FOdysseyVectorLink* lastSampleLink = GetLastSampleLink();
        FOdysseyVectorLink* sampleLink = new FOdysseyVectorLink ( lastSamplePoint, iSamplePoint );

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
                FOdysseyVectorPointCubic* cubicPoint = new FOdysseyVectorPointCubic( lastSamplePoint->GetX()
                                                                     , lastSamplePoint->GetY()
                                                                     , iRadius );
                ::ULIS::FVec2D entryVector = mSampleLinkList.front()->GetVector( true );
                ::ULIS::FVec2D exitVector =  lastSampleSegmentVector;

                cubicSegment = mCubicPath->AppendPoint( cubicPoint, true, true );

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

                        cubicSegment->GetControlPoint(0).Set ( cubicSegment->GetPoint(0)->GetX() + ( lastCubicSegmentVector.x * length * 0.35f )
                                                             , cubicSegment->GetPoint(0)->GetY() + ( lastCubicSegmentVector.y * length * 0.35f ) );
                    }
                }

                mCumulAngle = 0.0f;
            }
        }
    }

    return cubicSegment;
}

void
FOdysseyVectorPathBuilder::ClearUntil( FOdysseyVectorPoint* iPoint )
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

FOdysseyVectorSegmentCubic*
FOdysseyVectorPathBuilder::AppendPoint( double iX
                               , double iY
                               , double iRadius
                               , bool   iEnforce )
{
    FOdysseyVectorPoint* point = new FOdysseyVectorPoint( iX, iY );
    FOdysseyVectorPoint* lastPoint = ( mPointList.size() ) ? mPointList.back() : nullptr;
    FOdysseyVectorSegmentCubic* cubicSegment = nullptr;

    mPointList.push_back( point );

    if ( lastPoint == nullptr )
    {
        FOdysseyVectorPointCubic* cubicPoint = new FOdysseyVectorPointCubic( point->GetX()
                                                             , point->GetY()
                                                             , iRadius );
        FOdysseyVectorPoint* samplePoint = new FOdysseyVectorPoint ( point->GetX(), point->GetY() );

        mCubicPath->AppendPoint( cubicPoint, false, false );

        Sample ( samplePoint, iRadius, iEnforce );
    }
    else
    {
        FOdysseyVectorLink* link = new FOdysseyVectorLink ( lastPoint, point );
        FOdysseyVectorPoint* lastSamplePoint = GetLastSamplePoint();
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

    mCubicPath->Update();

    return cubicSegment;
}

FOdysseyVectorSegment*
FOdysseyVectorPathBuilder::AppendPoint( double iX
                               , double iY
                               , double iRadius )
{
    return AppendPoint ( iX, iY, iRadius, false );
}

FOdysseyVectorPathCubic* 
FOdysseyVectorPathBuilder::GetCubicPath()
{
    return mCubicPath;
}

FOdysseyVectorSegment*
FOdysseyVectorPathBuilder::End( double iX
                       , double iY
                       , double iRadius
                       , bool iClose )
{
    FOdysseyVectorPoint* lastPoint = mCubicPath->GetLastPoint();
    FOdysseyVectorPoint* firstPoint = mCubicPath->GetFirstPoint();
    FOdysseyVectorSegment* firstSegment = mCubicPath->GetFirstSegment();
    FOdysseyVectorSegmentCubic* newSegment = nullptr;;

    if ( lastPoint )
    {
        if ( lastPoint->GetX() == iX && lastPoint->GetY() == iY )
        {
            // We delete the last point to prevent both points being at the same location
            // which would fake the result of AppendPoint() with iEnforce = true
            mPointList.remove( lastPoint );
        }

        AppendPoint ( iX, iY, iRadius, true );

        newSegment = static_cast<FOdysseyVectorSegmentCubic*>(mCubicPath->GetLastSegment());

        if ( iClose == true )
        {
            if ( firstSegment && newSegment )
            {
                FOdysseyVectorPoint* newPoint = mCubicPath->GetLastPoint();

                if ( firstPoint->GetSegmentCount() == 1 )
                {
                    FOdysseyVectorSegmentCubic* loopSegment = new FOdysseyVectorSegmentCubic( *mCubicPath
                                                                               , static_cast<FOdysseyVectorPointCubic*>(newPoint)
                                                                               , static_cast<FOdysseyVectorPointCubic*>(firstPoint) );


                    mCubicPath->AddSegment( loopSegment );

                    /*Round( *loopSegment );*/
                }
            }
        }
    }

    return nullptr;
}

void
FOdysseyVectorPathBuilder::FitSegment( FOdysseyVectorSegmentCubic& iSegment
                              , std::list<FOdysseyVectorLink*>& iLinkList )
{
    ::ULIS::FVec2D& point0 = iSegment.GetPoint(0)->GetCoords();
    ::ULIS::FVec2D& point1 = iSegment.GetPoint(1)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = iSegment.GetControlPoint(0).GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = iSegment.GetControlPoint(1).GetCoords();
    double sampleLength = 0.0f;
    double currentT = 0.0f;

    for( std::list<FOdysseyVectorLink*>::iterator it = iLinkList.begin(); it != iLinkList.end(); ++it )
    {
        FOdysseyVectorLink *link = (*it);

        sampleLength += link->GetStraightDistance();
    }

    if ( sampleLength )
    {
        double A1 = 0.0f, A2 = 0.0f, A12 = 0.0f;
        ::ULIS::FVec2D C1 = { 0.0f, 0.0f };
        ::ULIS::FVec2D C2 = { 0.0f, 0.0f };

        for( std::list<FOdysseyVectorLink*>::iterator it = iLinkList.begin(); it != iLinkList.end(); ++it )
        {
            FOdysseyVectorLink *link = (*it);
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
FOdysseyVectorPathBuilder::DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();

    BLPath path;

    blctx.setCompOp(BL_COMP_OP_SRC_COPY);
    /*iBLContext.setFillStyle(BLRgba32(0xFFFFFFFF));
    iBLContext.setStrokeStyle(BLRgba32(0xFF000000));*/

    blctx.setStrokeStyle( BLRgba32( mStrokeColor ) );
    blctx.setStrokeWidth( mStrokeWidth );

    for( std::list<FOdysseyVectorLink*>::iterator it = mLinkList.begin(); it != mLinkList.end(); ++it )
    {
        FOdysseyVectorLink *link = (*it);
        BLPoint point0;
        BLPoint point1;

        point0.x = link->GetPoint(0)->GetX();
        point0.y = link->GetPoint(0)->GetY();

        point1.x = link->GetPoint(1)->GetX();
        point1.y = link->GetPoint(1)->GetY();

        path.moveTo( point0.x,point0.y );
        path.lineTo( point1.x,point1.y );
    }

    blctx.strokePath( path );

    blctx.setFillStyle(BLRgba32(0xFFFF00FF));

    for( std::list<FOdysseyVectorPoint*>::iterator it = mSamplePointList.begin(); it != mSamplePointList.end(); ++it )
    {
        FOdysseyVectorPoint *samplePoint = (*it);

        blctx.fillRect( samplePoint->GetX() - 3, samplePoint->GetY() - 3, 6, 6  );
    }
}

bool
FOdysseyVectorPathBuilder::PickPoint( double iX
                             , double iY
                             , double iRadius
                             , uint64 iSelectionFlags )
{
    return false;
}
