#include "OdysseyVectorPathBuilder.h"

FOdysseyVectorPathBuilder::~FOdysseyVectorPathBuilder()
{

}

FOdysseyVectorPathBuilder::FOdysseyVectorPathBuilder()
    : mCubicPath ( nullptr )
    , mCumulAngle ( 0.0f )
    , mPointID( 0 )
    , mCumulAngleLimit ( 1.5708f ) // 90 degrees
    , mLastCubicAngleLimit ( 1.0472f ) // 60 deg
{
    mObjectParam.Foreground.R = 128;
    mObjectParam.Foreground.G = 128;
    mObjectParam.Foreground.B = 128;
    mObjectParam.Foreground.A = 255;

    mLinkBuffer.reserve( 50 );
    mPointBuffer.reserve( 50 );
    mSampleBuffer.reserve( 50 );

    mPointArray.reserve(50);
    mSampleArray.reserve(50);
    mVertexArray.reserve(50);
}

void 
FOdysseyVectorPathBuilder::Attach( FOdysseyVectorPathCubic* iCubicPath )
{
    mCubicPath = iCubicPath;
}

static void
Shape( FOdysseyVectorSegmentCubic& iCubicSegment, ::ULIS::FVec2D iEntryVector, ::ULIS::FVec2D iExitVector )
{
    double length = iCubicSegment.GetVector( false ).Distance();

    iCubicSegment.GetHandle(0)->Set( iCubicSegment.GetPoint(0)->GetX() + iEntryVector.x * length * 0.35f
                                   , iCubicSegment.GetPoint(0)->GetY() + iEntryVector.y * length * 0.35f );

    iCubicSegment.GetHandle(1)->Set( iCubicSegment.GetPoint(1)->GetX() - iExitVector.x * length * 0.35f
                                   , iCubicSegment.GetPoint(1)->GetY() - iExitVector.y * length * 0.35f );

    iCubicSegment.Update();
}

double
FOdysseyVectorPathBuilder::GetSampleAngle()
{
    if( mLinkBuffer.size() >= 2 )
    {
        uint32 idx0 = mLinkBuffer.size() - 1;
        uint32 idx1 = idx0 - 1;

        ::ULIS::FVec2D link0 = mLinkBuffer[idx0].GetVector( true );
        ::ULIS::FVec2D link1 = mLinkBuffer[idx1].GetVector( true );

        if( link0.DistanceSquared() && link1.DistanceSquared() )
        {
            return acos( ULIS::FMath::Clamp<double>( link0.DotProduct( link1 ), -1.0f, 1.0f ) );
        }
    }

    return 0.0f;
}

/*
            if( iEnforce )
            {
                mFinalVertex = static_cast<FOdysseyVectorVertexCubic*>(iPoint);

                finalSampleID = mSampleArray.size() - 1;
                finalLinkID = mLinkArray.size() - 1;
            }
*/

uint32
FOdysseyVectorPathBuilder::RecordVertex( double iX, double iY, double iRadius, uint32 iID )
{
    double angle = GetSampleAngle();
    uint32 ret = 0;

    mCumulAngle += angle;

    if ( ( angle >= mLastCubicAngleLimit ) || ( mCumulAngle >= mCumulAngleLimit ) )
    {
        FOdysseyVectorVertexCubic* cubicVertex = FOdysseyVectorVertexCubic::New( iX, iY, iRadius );
        FOdysseyVectorVertexCubic* previousCubicVertex = mVertexArray.back();

        cubicVertex->SetID( iID );

        mVertexArray.push_back( cubicVertex );

        ret |= FOdysseyVectorPathBuilder::NEWVERTEX;

        if( mLinkBuffer.size() >= 2 )
        {
            uint32 linkID0 =  0;
            uint32 linkID1 =  mLinkBuffer.size() - 2; // penultimate link

            mCubicSegment = FOdysseyVectorSegmentCubic::New( mCubicPath, previousCubicVertex, cubicVertex );

            Shape ( *mCubicSegment
                   , mLinkBuffer[linkID0].GetVector( true )
                   , mLinkBuffer[linkID1].GetVector( true ) );

            Adjust( *mCubicSegment );

            ret |= FOdysseyVectorPathBuilder::NEWSEGMENT;
        }

        mCumulAngle = 0.0f;
    }

    return ret;
}

uint32
FOdysseyVectorPathBuilder::RecordSample( double iX, double iY, double iRadius, uint32 iID )
{

    FOdysseyVectorPoint* previousSample = mSampleArray.back();
    ::ULIS::FVec2D dif = { iX - previousSample->GetX(), iY - previousSample->GetY() };
    double length = dif.Distance();
    uint32 ret = 0;

    if( length >= 12.0f )
    {
        FOdysseyVectorPoint sample = FOdysseyVectorPoint( iX, iY, iRadius );
        uint32 sampleIndex = mSampleBuffer.size();

        sample.SetID( iID );

        mSampleBuffer.push_back( sample );

        mSampleArray.push_back( &mSampleBuffer[sampleIndex] );
        mLinkBuffer.push_back( FOdysseyVectorLink( previousSample, &mSampleBuffer[sampleIndex] ) );

        ret |= FOdysseyVectorPathBuilder::NEWSAMPLE;
    }

    return ret | RecordVertex( iX, iY, iRadius, iID );
}

uint32
FOdysseyVectorPathBuilder::RecordPoint( double iX, double iY, double iRadius, uint32 iID )
{
    FOdysseyVectorPoint point = FOdysseyVectorPoint( iX, iY, iRadius );
    uint32 pointIndex = mPointBuffer.size();
    uint32 ret = 0;

    point.SetID( iID );

    mPointBuffer.push_back( point );
    mPointArray.push_back( &mPointBuffer[pointIndex] );

    return ret | RecordSample( iX, iY, iRadius, iID );
}

void
FOdysseyVectorPathBuilder::ClearPointsUntil( uint32 iID )
{
    std::vector<FOdysseyVectorPoint> cleanedBuffer;
    int rank = -1;

cleanedBuffer.reserve(50);

    mPointArray.clear();

    while( mPointBuffer[++rank].GetID() != iID );

    for( int i = rank; i < mPointBuffer.size(); i++ )
    {
        cleanedBuffer.push_back( mPointBuffer[i] );
    }

    mPointBuffer = cleanedBuffer;

    for( int i = rank; i < mPointBuffer.size(); i++ )
    {
        mPointArray.push_back( &mPointBuffer[i] );
    }
}

void
FOdysseyVectorPathBuilder::ClearSamplesUntil( uint32 iID )
{
    std::vector<FOdysseyVectorPoint> cleanedBuffer;
    int rank = -1;

cleanedBuffer.reserve(50);

    mSampleArray.clear();

    while( mSampleBuffer[++rank].GetID() != iID );
//UE_LOG(LogTemp, Warning, TEXT("Some warning message %d/%d"), rank, mSampleBuffer.size() );
    for( int i = rank; i < mSampleBuffer.size(); i++ )
    {
        cleanedBuffer.push_back( mSampleBuffer[i] );
    }

    mSampleBuffer = cleanedBuffer;

    for( int i = 0; i < mSampleBuffer.size(); i++ )
    {
        mSampleArray.push_back( &mSampleBuffer[i] );
    }

    // recreate links
    mLinkBuffer.clear();

    for( int i = 0; i < mSampleArray.size(); i++ )
    {
       int n = i + 1;
       FOdysseyVectorLink link = FOdysseyVectorLink( mSampleArray[i], mSampleArray[n] );

        mLinkBuffer.push_back( link );
    }
}

void
FOdysseyVectorPathBuilder::RecordStart( FOdysseyVectorVertexCubic *iVertex )
{
    mPointBuffer.push_back( FOdysseyVectorPoint( iVertex->GetX(), iVertex->GetY(), iVertex->GetRadius() ) );
    mPointBuffer[0].SetID( mPointID );
    mPointArray.push_back( &mPointBuffer[0] );

    mSampleBuffer.push_back( FOdysseyVectorPoint( iVertex->GetX(), iVertex->GetY(), iVertex->GetRadius() ) );
    mSampleBuffer[0].SetID( mPointID );
    mSampleArray.push_back( &mSampleBuffer[0] );

    iVertex->SetID( mPointID );
    mVertexArray.push_back( iVertex );

    mPointID++;
}

void
FOdysseyVectorPathBuilder::RecordIntermediate( double iX, double iY, double iRadius )
{
    uint32 ret = RecordPoint( iX, iY, iRadius, mPointID++ );

    if( ret & FOdysseyVectorPathBuilder::NEWSAMPLE )
    {
        ClearPointsUntil( mSampleArray.back()->GetID() );
    }

    if( ret & FOdysseyVectorPathBuilder::NEWVERTEX )
    {
        mCubicPath->AddVertex( mVertexArray.back() );

        ClearSamplesUntil( mVertexArray.back()->GetID() );
    }

    if( ret & FOdysseyVectorPathBuilder::NEWSEGMENT )
    {
        mCubicPath->AddSegment( mCubicSegment );
        mCubicSegment->Invalidate();
    }
}

void
FOdysseyVectorPathBuilder::RecordEnd( FOdysseyVectorVertexCubic *iVertex )
{

}

FOdysseyVectorObject*
FOdysseyVectorPathBuilder::CopyShape()
{
    return nullptr;
}

double
FOdysseyVectorPathBuilder::GetTotalSampleLinkLength()
{
    double length = 0.0f;

    for( uint32 i = 0; i < mLinkBuffer.size(); i++ )
    {

        length += mLinkBuffer[i].GetStraightDistance();
    }

    return length;
}

::ULIS::FVec2D
FOdysseyVectorPathBuilder::GetSamplePointAtParameter( double iToTalLinkLength, double iT )
{
    ::ULIS::FVec2D point = { 0.0f, 0.0f };
    double currentT = 0.0f;

    for( uint32 i = 0; i < mLinkBuffer.size(); i++ )
    {
        double t = currentT + ( mLinkBuffer[i].GetStraightDistance() / iToTalLinkLength );

        if ( ( iT >= currentT ) && ( iT <= t ) )
        {
            double deltaT = iT - currentT;
            ::ULIS::FVec2D linkVec = mLinkBuffer[i].GetVector( false );

            point.x = mLinkBuffer[i].GetPoint(0)->GetX() + ( deltaT * linkVec.x );
            point.y = mLinkBuffer[i].GetPoint(0)->GetY() + ( deltaT * linkVec.y );

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
FOdysseyVectorPathBuilder::Adjust( FOdysseyVectorSegmentCubic& iCubicSegment )
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

/*
FOdysseyVectorPathBuilder::SmoothSegment( FOdysseyVectorSegmentCubic* iCubicSegment )
{
    FOdysseyVectorVertex vertex0 = iCubicSegment->GetVertex( 0 );
    FOdysseyVectorVertex vertex1 = iCubicSegment->GetVertex( 1 );
    FOdysseyVectorSegmentCubic* prevCubicSegment = vertex0->GetOtherSegment( iCubicSegment );
    FOdysseyVectorSegmentCubic* nextCubicSegment = vertex1->GetOtherSegment( iCubicSegment );

    if ( prevCubicSegment )
    {
        ::ULIS::FVec2D prevCubicSegmentVector = prevCubicSegment->GetVectorAtEnd( true );
        double smoothDot = entryVector.DotProduct( prevCubicSegmentVector );
        double smoothAngle = acos( ULIS::FMath::Clamp<double>( smoothDot, -1.0f, 1.0f ) );

        if ( fabs(smoothAngle) < mLastCubicAngleLimit )
        {
            double length = iCubicSegment->GetStraightDistance();

            iCubicSegment->GetHandle(0)->Set ( vertex0->GetX() + ( prevCubicSegmentVector.x * length * 0.35f )
                                             , vertex0->GetY() + ( prevCubicSegmentVector.y * length * 0.35f ) );
        }
    }

    if ( nextCubicSegment )
    {
        ::ULIS::FVec2D nextCubicSegmentVector = nextCubicSegment->GetVectorAtStart( true );
        double smoothDot = entryVector.DotProduct( nextCubicSegmentVector );
        double smoothAngle = acos( ULIS::FMath::Clamp<double>( smoothDot, -1.0f, 1.0f ) );

        if ( fabs(smoothAngle) < mLastCubicAngleLimit )
        {
            double length = iCubicSegment->GetStraightDistance();

            iCubicSegment->GetHandle(1)->Set ( vertex1->GetX() + ( nextCubicSegmentVector.x * length * 0.35f )
                                             , vertex1->GetY() + ( nextCubicSegmentVector.y * length * 0.35f ) );
        }
    }
}
*/

FOdysseyVectorPathCubic* 
FOdysseyVectorPathBuilder::GetCubicPath()
{
    return mCubicPath;
}

void
FOdysseyVectorPathBuilder::FitSegment( FOdysseyVectorSegmentCubic& iSegment
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
FOdysseyVectorPathBuilder::DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext* blctx = GetScene()->GetEngine()->GetBLContext();
    BLPath path;
    BLRgba32 strokeColor;

    strokeColor.r = mObjectParam.Foreground.R;
    strokeColor.g = mObjectParam.Foreground.G;
    strokeColor.b = mObjectParam.Foreground.B;
    strokeColor.a = mObjectParam.Foreground.A;

    blctx->setCompOp(BL_COMP_OP_SRC_COPY);
    /*iBLContext.setFillStyle(BLRgba32(0xFFFFFFFF));
    iBLContext.setStrokeStyle(BLRgba32(0xFF000000));*/

    blctx->setStrokeStyle( strokeColor );

    if( mSampleArray.size() > 1 )
    {
        for( uint32 i = 0; i < mSampleArray.size() - 1; i++ )
        {
            uint32 n = i + 1;
            ::ULIS::FVec2D& point0 = mSampleArray[i]->GetCoords();
            ::ULIS::FVec2D& point1 = mSampleArray[n]->GetCoords();

            blctx->setStrokeWidth( mSampleArray[i]->GetRadius() * 2.0f );

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
