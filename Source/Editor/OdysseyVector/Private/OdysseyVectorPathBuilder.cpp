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

    mPointBuffer.reserve( 100 );
}

void 
UOdysseyVectorPathBuilder::Attach( UOdysseyVectorPathCubic* iCubicPath )
{
    mCubicPath = iCubicPath;
}

void
UOdysseyVectorPathBuilder::Reset()
{
    mSamplePointArray.clear();
    mPointArray.clear();
    mCumulAngle = 0.0f;
}

static void
Shape( FOdysseyVectorSegmentCubic& iCubicSegment, ::ULIS::FVec2D iEntryVector, ::ULIS::FVec2D iExitVector )
{
    double length = iCubicSegment.GetVector( false ).Distance();

    iCubicSegment.GetHandle(0)->Set( iCubicSegment.GetPoint(0)->GetX() + iEntryVector.x * length * 0.35f
                                   , iCubicSegment.GetPoint(0)->GetY() + iEntryVector.y * length * 0.35f );

    iCubicSegment.GetHandle(1)->Set( iCubicSegment.GetPoint(1)->GetX() + iExitVector.x * length * 0.35f
                                   , iCubicSegment.GetPoint(1)->GetY() + iExitVector.y * length * 0.35f );

    iCubicSegment.Update();
}

FOdysseyVectorSegmentCubic*
UOdysseyVectorPathBuilder::MakeSegment( FOdysseyVectorVertexCubic* iVertex0, FOdysseyVectorVertexCubic* iVertex1 )
{
    FOdysseyVectorSegmentCubic* cubicSegment = FOdysseyVectorSegmentCubic::New( mCubicPath, iVertex0, iVertex1 );

    Shape ( *cubicSegment
           , GetFirstVectorFromSamples()
           , GetLastVectorFromSamples() );

    Adjust( *cubicSegment );

    Reset();
    RecordSample( iVertex1 );
    RecordPoint( iVertex1 );

    return cubicSegment;
}

void
UOdysseyVectorPathBuilder::RecordPoint( FOdysseyVectorPoint* iPoint )
{
    mPointArray.push_back( iPoint );
}

::ULIS::FVec2D
UOdysseyVectorPathBuilder::GetFirstVectorFromSamples()
{
    ::ULIS::FVec2D vec = ::ULIS::FVec2D( 0.0f, 0.0f );

    if( mSamplePointArray.size() > 1 )
    {
        uint32 idx0 = 0;
        uint32 idx1 = 1;

        vec = mSamplePointArray[idx1]->GetCoords() - mSamplePointArray[idx0]->GetCoords();
    }

    if( vec.DistanceSquared() )
    {
        vec.Normalize();
    }

    return vec;
}

::ULIS::FVec2D
UOdysseyVectorPathBuilder::GetLastVectorFromSamples()
{
    ::ULIS::FVec2D vec = ::ULIS::FVec2D( 0.0f, 0.0f );

    if( mSamplePointArray.size() > 1 )
    {
        uint32 idx0 = mSamplePointArray.size() - 1;
        uint32 idx1 = idx0 - 1;

        vec = mSamplePointArray[idx1]->GetCoords() - mSamplePointArray[idx0]->GetCoords();
    }

    if( vec.DistanceSquared() )
    {
        vec.Normalize();
    }

    return vec;
}

double
UOdysseyVectorPathBuilder::RecordSample( FOdysseyVectorPoint* iPoint )
{
    mSamplePointArray.push_back( iPoint );

    if( mSamplePointArray.size() > 2 )
    {
        uint32 idx0 = mSamplePointArray.size() - 1;
        uint32 idx1 = idx0 - 1;
        uint32 idx2 = idx1 - 1;
        ::ULIS::FVec2D p0p1 = ::ULIS::FVec2D( mSamplePointArray[idx1]->GetX() - mSamplePointArray[idx0]->GetX()
                                            , mSamplePointArray[idx1]->GetY() - mSamplePointArray[idx0]->GetY() );
        ::ULIS::FVec2D p1p2 = ::ULIS::FVec2D( mSamplePointArray[idx2]->GetX() - mSamplePointArray[idx1]->GetX()
                                            , mSamplePointArray[idx2]->GetY() - mSamplePointArray[idx1]->GetY() );

        if( p0p1.DistanceSquared() && p1p2.DistanceSquared() )
        {
            p0p1.Normalize();
            p1p2.Normalize();

            return acos( ULIS::FMath::Clamp<double>( p0p1.DotProduct( p1p2 ), -1.0f, 1.0f ) );
        }
    }

    return 0.0f;
}

FOdysseyVectorSegmentCubic*
UOdysseyVectorPathBuilder::RecordVertex( FOdysseyVectorVertexCubic* iRecordedVertex
                                       , FOdysseyVectorVertexCubic* iPreviousVertex
                                       , double iX
                                       , double iY
                                       , double iRadius )
{
    FOdysseyVectorSegmentCubic* newCubicSegment = nullptr;
    FOdysseyVectorVertexCubic* vertex0 = iPreviousVertex;
    FOdysseyVectorVertexCubic* vertex1 = nullptr;

    if( iRecordedVertex == nullptr )
    {
        // Note: this will crash if no previous vertex was recorded. This is INTENTIONAL. A first vertex must always be recorded.
        FOdysseyVectorPoint* lastSamplePoint = mSamplePointArray.back();
        ::ULIS::FVec2D dif = { iX - lastSamplePoint->GetX()
                             , iY - lastSamplePoint->GetY() };
        double length = dif.Distance();
        FOdysseyVectorPoint point = FOdysseyVectorPoint( iX, iY, iRadius );
        uint32 pointCount = mPointBuffer.size();

        mPointBuffer.push_back( point );

        RecordPoint( &mPointBuffer[pointCount] );

        if( length >= 12.0f )
        {
            double angle = RecordSample( &mPointBuffer[pointCount] );

            mCumulAngle += angle;

            vertex1 = ( (       angle >= mLastCubicAngleLimit ) ||
                        ( mCumulAngle >= mCumulAngleLimit     ) ) ? FOdysseyVectorVertexCubic::New( iX, iY, iRadius ) : nullptr;

            mPointArray.clear();
        }
    }
    else
    {
        RecordPoint( iRecordedVertex );
        RecordSample( iRecordedVertex );

        vertex1 = iRecordedVertex;
    }

    return ( vertex1 && vertex0 ) ? MakeSegment( vertex0, vertex1 ) : nullptr;;
}

UOdysseyVectorObject*
UOdysseyVectorPathBuilder::CopyShape()
{
    return nullptr;
}

double
UOdysseyVectorPathBuilder::GetTotalSampleLinkLength()
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
UOdysseyVectorPathBuilder::GetSamplePointAtParameter( double iToTalLinkLength, double iT )
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
UOdysseyVectorPathBuilder::Adjust( FOdysseyVectorSegmentCubic& iCubicSegment )
{
    double totalLinkLength = GetTotalSampleLinkLength();
    ::ULIS::FVec2D& point0 = iCubicSegment.GetVertex(0)->GetCoords( nullptr );
    ::ULIS::FVec2D& point1 = iCubicSegment.GetVertex(1)->GetCoords( nullptr );
    ::ULIS::FVec2D& ctrlPoint0 = iCubicSegment.GetHandle(0)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = iCubicSegment.GetHandle(1)->GetCoords();
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

#ifdef UNUSED
FOdysseyVectorSegmentCubic*
UOdysseyVectorPathBuilder::Sample( FOdysseyVectorVertexCubic* iLastVertex
                                 , FOdysseyVectorVertexCubic* iNextVertex
                                 , FOdysseyVectorPoint* iSamplePoint
                                 , double iRadius )
{
    FOdysseyVectorPoint* lastSamplePoint = GetLastSamplePoint();
    FOdysseyVectorSegmentCubic* lastCubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(mCubicPath->GetLastSegment());
    FOdysseyVectorSegmentCubic* cubicSegment = nullptr;

    mSamplePointList.push_back( iSamplePoint );

    if ( lastSamplePoint )
    {
        FOdysseyVectorLink* lastSampleLink = GetLastSampleLink();
        FOdysseyVectorLink* sampleLink = FOdysseyVectorLink::New( lastSamplePoint, iSamplePoint );

        mSampleLinkList.push_back( sampleLink );

        if ( lastSampleLink )
        {
            ::ULIS::FVec2D lastSampleSegmentVector = lastSampleLink->GetVector( true );
            ::ULIS::FVec2D     sampleSegmentVector =     sampleLink->GetVector( true );
            double dot = lastSampleSegmentVector.DotProduct( sampleSegmentVector );
            double angle = acos( ULIS::FMath::Clamp<double>( dot, -1.0f, 1.0f ) );

            mCumulAngle += angle;

            if ( ( angle > mLastCubicAngleLimit ) || ( mCumulAngle >= mCumulAngleLimit ) || iNextVertex )
            {
                ::ULIS::FVec2D entryVector = mSampleLinkList.front()->GetVector( true );
                ::ULIS::FVec2D exitVector =  lastSampleSegmentVector;
                FOdysseyVectorVertexCubic* cubicVertex;

                if( iNextVertex == nullptr )
                {
                    cubicVertex = FOdysseyVectorVertexCubic::New( lastSamplePoint->GetX(), lastSamplePoint->GetY(), iRadius );

                    mCubicPath->AddVertex( cubicVertex );
                }
                else
                {
                    cubicVertex = iNextVertex;
                }

                if( iLastVertex )
                {
                    cubicSegment = FOdysseyVectorSegmentCubic::New( mCubicPath, iLastVertex, cubicVertex );

                    mCubicPath->AddSegment( cubicSegment );

                    cubicSegment->Invalidate();

                    Sharp ( *cubicSegment
                            , entryVector
                            , exitVector );
                }

                if ( lastCubicSegment )
                {
                    ::ULIS::FVec2D lastCubicSegmentVector = lastCubicSegment->GetVectorAtEnd( true );
                    double smoothDot = entryVector.DotProduct( lastCubicSegmentVector );
                    double smoothAngle = acos( ULIS::FMath::Clamp<double>( smoothDot, -1.0f, 1.0f ) );

                    if ( fabs(smoothAngle) < mLastCubicAngleLimit )
                    {
                        double length = cubicSegment->GetStraightDistance();

                        cubicSegment->GetHandle(0)->Set ( cubicSegment->GetPoint(0)->GetX() + ( lastCubicSegmentVector.x * length * 0.35f )
                                                        , cubicSegment->GetPoint(0)->GetY() + ( lastCubicSegmentVector.y * length * 0.35f ) );
                    }
                }

                mCumulAngle = 0.0f;
            }
        }
    }

    return cubicSegment;
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! TODO Free memory or use arrays !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

FOdysseyVectorSegmentCubic*
UOdysseyVectorPathBuilder::AppendPoint( FOdysseyVectorVertexCubic* iOnLastVertex
                                      , FOdysseyVectorVertexCubic* iOnNextVertex
                                      , double iX
                                      , double iY
                                      , double iRadius
                                      , bool   iEnforce )
{
    FOdysseyVectorPoint* point = FOdysseyVectorPoint::New( iX, iY, iRadius );
    FOdysseyVectorPoint* lastPoint = ( mPointList.size() ) ? mPointList.back() : nullptr;
    FOdysseyVectorSegmentCubic* cubicSegment = nullptr;

    mPointList.push_back( point );

    if ( lastPoint == nullptr )
    {
        FOdysseyVectorVertexCubic* lastVertex;
        FOdysseyVectorVertexCubic* nextVertex;

        if( iOnLastVertex == nullptr )
        {
            FOdysseyVectorVertexCubic* cubicVertex = FOdysseyVectorVertexCubic::New( point->GetX()
                                                                                   , point->GetY()
                                                                                   , iRadius );

            mCubicPath->AppendVertex( cubicVertex, false, false );

            lastVertex = cubicVertex;
            nextVertex = iOnNextVertex;
        }
        else
        {
            // interpolate radii
            iOnLastVertex->SetRadius( ( iOnLastVertex->GetRadius() + iRadius ) * 0.5f );

            lastVertex = iOnLastVertex;
            nextVertex = iOnNextVertex;
        }

        Sample ( lastVertex, nextVertex, point, iRadius );
    }
    else
    {
        FOdysseyVectorLink* link = FOdysseyVectorLink::New( lastPoint, point );
        FOdysseyVectorPoint* lastSamplePoint = GetLastSamplePoint();
        ::ULIS::FVec2D dif = { iX - lastSamplePoint->GetX()
                             , iY - lastSamplePoint->GetY() };
        double length = dif.Distance();

        if ( length >= 12.0f || iEnforce == true )
        {
            cubicSegment = Sample ( iOnLastVertex, iOnNextVertex, point, iRadius );

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

#endif

UOdysseyVectorPathCubic* 
UOdysseyVectorPathBuilder::GetCubicPath()
{
    return mCubicPath;
}

void
UOdysseyVectorPathBuilder::FitSegment( FOdysseyVectorSegmentCubic& iSegment
                                     , std::list<FOdysseyVectorLink*>& iLinkList )
{
    ::ULIS::FVec2D& point0 = iSegment.GetVertex(0)->GetCoords( nullptr );
    ::ULIS::FVec2D& point1 = iSegment.GetVertex(1)->GetCoords( nullptr );
    ::ULIS::FVec2D& ctrlPoint0 = iSegment.GetHandle(0)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = iSegment.GetHandle(1)->GetCoords();
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
UOdysseyVectorPathBuilder::DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext* blctx = GetScene()->GetEngine()->GetBLContext();
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

    if( mSamplePointArray.size() > 1 )
    {
        for( uint32 i = 0; i < mSamplePointArray.size() - 1; i++ )
        {
            uint32 n = i + 1;
            ::ULIS::FVec2D& point0 = mSamplePointArray[i]->GetCoords();
            ::ULIS::FVec2D& point1 = mSamplePointArray[n]->GetCoords();

            blctx->setStrokeWidth( mSamplePointArray[i]->GetRadius() * 2.0f );

            blctx->strokeLine( point0.x, point0.y, point1.x, point1.y );
        }
    }

    if( mPointArray.size() > 1 )
    {
        for( uint32 i = 0; i < mPointArray.size() - 1; i++ )
        {
            uint32 n = i + 1;
            ::ULIS::FVec2D& point0 = mPointArray[i]->GetCoords();
            ::ULIS::FVec2D& point1 = mPointArray[n]->GetCoords();

            blctx->setStrokeWidth( mPointArray[i]->GetRadius() * 2.0f );

            blctx->strokeLine( point0.x, point0.y, point1.x, point1.y );
        }
    }

/*
    blctx->setFillStyle(BLRgba32(0xFFFF00FF));


    for( std::list<FOdysseyVectorPoint*>::iterator it = mSamplePointList.begin(); it != mSamplePointList.end(); ++it )
    {
        FOdysseyVectorPoint *samplePoint = (*it);

        blctx->fillRect( samplePoint->GetX() - 3, samplePoint->GetY() - 3, 6, 6  );
    }
*/
}
