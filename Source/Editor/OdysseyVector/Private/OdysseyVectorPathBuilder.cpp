#include "OdysseyVectorPathBuilder.h"

FOdysseyVectorPointSample::~FOdysseyVectorPointSample()
{
}

FOdysseyVectorPointSample::FOdysseyVectorPointSample( double iX, double iY, double iRadius )
    : FOdysseyVectorPoint( iX, iY, iRadius )
    , mIsSharp( false )
{
}

FOdysseyVectorPointSample::FOdysseyVectorPointSample( std::vector<FOdysseyVectorPoint*> iPointArray )
{
    double averageX = 0.0f;
    double averageY = 0.0f;
    double averageRadius = 0.0f;
    uint32 pointCount = iPointArray.size();

    if( pointCount )
    {
        for( uint32 i = 0; i < pointCount; i++ )
        {
            averageX += iPointArray[i]->GetX();
            averageY += iPointArray[i]->GetY();
            averageRadius += iPointArray[i]->GetRadius();
        }

        averageX /= pointCount;
        averageY /= pointCount;
        averageRadius /= pointCount;
    }

    Set( averageX, averageY );
    SetRadius( averageRadius );
}

void
FOdysseyVectorPointSample::SetSharp( bool iIsSharp )
{
    mIsSharp = iIsSharp;
}

bool
FOdysseyVectorPointSample::IsSharp()
{
    return mIsSharp;
}

FOdysseyVectorPathBuilder::~FOdysseyVectorPathBuilder()
{
}

FOdysseyVectorPathBuilder::FOdysseyVectorPathBuilder()
    : mCumulAngle ( 0.0f )
    , mCubicPath ( nullptr )
    , mCumulAngleLimit ( 1.5708f ) // 90 degrees
    , mPointID( 0 )
    , mAngleLimit ( 1.0472f ) // 60 deg
    //, mAngleLimit ( 0.7071f ) // 45 deg
{
    mObjectParam.Foreground.R = 128;
    mObjectParam.Foreground.G = 128;
    mObjectParam.Foreground.B = 128;
    mObjectParam.Foreground.A = 255;

    mLinkBuffer.reserve(50);
    mPointBuffer.reserve(50);
    mSampleBuffer.reserve(50);

    mPointArray.reserve(50);
    mSampleArray.reserve(50);
    mVertexArray.reserve(50);
}

std::vector<FOdysseyVectorPoint*>&
FOdysseyVectorPathBuilder::GetPointArray()
{
    return mPointArray;
}

bool
FOdysseyVectorPathBuilder::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorObject::HasBaseClass( iBaseClassID );
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
                mFinalVertex = static_cast<FOdysseyVectorVertex*>(iPoint);

                finalSampleID = mSampleArray.size() - 1;
                finalLinkID = mLinkArray.size() - 1;
            }
*/

static void
Smooth( FOdysseyVectorSegmentCubic* iNewSegment, bool iSharp )
{
    FOdysseyVectorVertex* vertex = iNewSegment->GetVertex( 0 );
    FOdysseyVectorHandleSegment* handle = iNewSegment->GetHandle( 0 );
    FOdysseyVectorSegmentCubic* prevSegment = static_cast<FOdysseyVectorSegmentCubic*>(vertex->GetOtherSegment( iNewSegment ));

    if( prevSegment )
    {
        uint32_t prevVertexIndex = ( prevSegment->GetVertex(0) == vertex ) ? 0 : 1;
        ::ULIS::FVec2D prevVector = prevSegment->GetHandleVector( prevVertexIndex, false );
        ::ULIS::FVec2D nsegVector = iNewSegment->GetHandleVector( 0, false );

        if( iSharp == false )
        {
            if( prevVector.DistanceSquared() )
            {
                double distance = nsegVector.Distance();

                prevVector.Normalize();

                handle->Set( vertex->GetX() - ( prevVector.x * distance )
                           , vertex->GetY() - ( prevVector.y * distance ) );
            }
        }
    }
}

uint32
FOdysseyVectorPathBuilder::RecordVertex()
{
    uint32 ret = 0;
    double angle = GetSampleAngle();

    mCumulAngle += angle;

    if ( ( angle >= mAngleLimit ) || ( mCumulAngle >= mCumulAngleLimit ) )
    {
        FOdysseyVectorVertex* previousCubicVertex = mVertexArray.back();
        // use the penultimate sample
        uint32 penultimateSampleIndex = mSampleArray.size() - 2;
        uint32 penultimateSampleID = mSampleArray[penultimateSampleIndex]->GetID();
        FOdysseyVectorVertex* cubicVertex = new FOdysseyVectorVertex( mCubicPath
                                                                    , mSampleArray[penultimateSampleIndex]->GetX()
                                                                    , mSampleArray[penultimateSampleIndex]->GetY()
                                                                    , mSampleArray[penultimateSampleIndex]->GetRadius() );

        mSampleArray[penultimateSampleIndex]->SetSharp( angle >= mAngleLimit );

        cubicVertex->SetID( penultimateSampleID );

        mVertexArray.push_back( cubicVertex );

        mCubicPath->AddVertex( mVertexArray.back() );

        ret |= FOdysseyVectorPathBuilder::NEWVERTEX;

        if( mLinkBuffer.size() >= 2 )
        {
            uint32 linkID0 =  0;
            uint32 linkID1 =  mLinkBuffer.size() - 2; // penultimate link
/// TODO: Factorize that part
            mCubicSegment = new FOdysseyVectorSegmentCubic( mCubicPath, previousCubicVertex, cubicVertex );

            Shape ( *mCubicSegment
                   , mLinkBuffer[linkID0].GetVector( true )
                   , mLinkBuffer[linkID1].GetVector( true ) );

            mCubicPath->AddSegment( mCubicSegment );

            Smooth( mCubicSegment, mSampleArray[0]->IsSharp() );

            AdjustHandle( mCubicSegment, 0, 0.5f, 1 );
            AdjustHandle( mCubicSegment, 1, 0.5f, 1 );

            mCubicSegment->Invalidate();

            mCubicPath->Update( 0 );
/// end TODO

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
    BLPoint worldVector = mWorldMatrix.mapVector( dif.x, dif.y );
    uint32 ret = 0;

    dif.x = worldVector.x;
    dif.y = worldVector.y;

    if( dif.Distance() >= 8.0f )
    {
        FOdysseyVectorPointSample sample = FOdysseyVectorPointSample( /*iX, iY, iRadius*/mPointArray ); // average
        uint32 sampleIndex = mSampleBuffer.size();

        sample.SetID( iID );

        mSampleBuffer.push_back( sample );

        mSampleArray.push_back( &mSampleBuffer[sampleIndex] );
        mLinkBuffer.push_back( FOdysseyVectorLink( previousSample, &mSampleBuffer[sampleIndex] ) );

        ret |= ( FOdysseyVectorPathBuilder::NEWSAMPLE | RecordVertex() );
    }

    return ret;
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
    int bufferSize = mPointBuffer.size();
    int rank = -1;
    int i, j;

    while( mPointBuffer[++rank].GetID() != iID );

    for( i = rank, j = 0; i < bufferSize; i++, j++ )
    {
        mPointBuffer[j] = mPointBuffer[i];
    }

    mPointBuffer.resize( j );

    // recreate pointers
    mPointArray.clear();

    for( i = rank; i < mPointBuffer.size(); i++ )
    {
        mPointArray.push_back( &mPointBuffer[i] );
    }
}

void
FOdysseyVectorPathBuilder::ClearSamplesUntil( uint32 iID )
{
    int bufferSize = mSampleBuffer.size();
    int rank = -1;
    int i, j;

    while( mSampleBuffer[++rank].GetID() != iID );

    for( i = rank, j = 0; i < bufferSize; i++, j++ )
    {
        mSampleBuffer[j] = mSampleBuffer[i];
    }

    mSampleBuffer.resize( j );

    // recreate pointers
    mSampleArray.clear();

    for( i = 0; i < mSampleBuffer.size(); i++ )
    {
        mSampleArray.push_back( &mSampleBuffer[i] );
    }

    // recreate links
    mLinkBuffer.clear();

    for( i = 0; i < mSampleArray.size() - 1; i++ )
    {
       int n = i + 1;
       FOdysseyVectorLink link = FOdysseyVectorLink( mSampleArray[i], mSampleArray[n] );

        mLinkBuffer.push_back( link );
    }
}

void
FOdysseyVectorPathBuilder::RecordStart( FOdysseyVectorVertex *iVertex )
{
    mPointBuffer.push_back( FOdysseyVectorPoint( iVertex->GetX(), iVertex->GetY(), iVertex->GetRadius() ) );
    mPointBuffer[0].SetID( mPointID );
    mPointArray.push_back( &mPointBuffer[0] );

    mSampleBuffer.push_back( FOdysseyVectorPointSample( iVertex->GetX(), iVertex->GetY(), iVertex->GetRadius() ) );
    mSampleBuffer[0].SetID( mPointID );
    mSampleBuffer[0].SetSharp( true );
    mSampleArray.push_back( &mSampleBuffer[0] );

    iVertex->SetID( mPointID );
    mVertexArray.push_back( iVertex );

    mPointID++;
}

FOdysseyVectorVertex*
FOdysseyVectorPathBuilder::RecordIntermediate( double iX
                                             , double iY
                                             , double iRadius
                                             , std::vector<FOdysseyVectorVertex*>& oNewVertexArray
                                             , std::vector<FOdysseyVectorSegment*>& oNewSegmentArray )
{
    uint32 ret = RecordPoint( iX, iY, iRadius, mPointID++ );

    if( ret & FOdysseyVectorPathBuilder::NEWSAMPLE )
    {
        ClearPointsUntil( mSampleArray.back()->GetID() );
    }

    if( ret & FOdysseyVectorPathBuilder::NEWVERTEX )
    {
        //FitSegment( *mCubicSegment, mLinkBuffer );

        ClearSamplesUntil( mVertexArray.back()->GetID() );

        oNewVertexArray.push_back( mVertexArray.back() );
    }

    if( ret & FOdysseyVectorPathBuilder::NEWSEGMENT )
    {
        //mCubicSegment->Invalidate();

        oNewSegmentArray.push_back( mCubicSegment );
    }

    return mVertexArray.back();
}

FOdysseyVectorSegmentCubic*
FOdysseyVectorPathBuilder::RecordEnd( FOdysseyVectorVertex *iVertex )
{
    if( iVertex != mVertexArray.back() )
    {
        uint32 linkID0 =  0;

        if( mLinkBuffer.size() )
        {
            uint32 linkID1 =  mLinkBuffer.size() - 1; // last link

        /// TODO: Factorize that part
            mCubicSegment = new FOdysseyVectorSegmentCubic( mCubicPath, mVertexArray.back(), iVertex );

            Shape ( *mCubicSegment
                   , mLinkBuffer[linkID0].GetVector( true )
                   , mLinkBuffer[linkID1].GetVector( true ) );

            mCubicPath->AddSegment( mCubicSegment );

            Smooth( mCubicSegment, mSampleArray[0]->IsSharp() );

            AdjustHandle( mCubicSegment, 0, 0.5f, 1 );
            AdjustHandle( mCubicSegment, 1, 0.5f, 1 );

            mCubicSegment->Invalidate();

            mCubicPath->Update( 0 );
        /// end TODO
            return mCubicSegment;
        }
    }

    return nullptr;
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
FOdysseyVectorPathBuilder::AdjustHandle( FOdysseyVectorSegmentCubic* iCubicSegment, uint32 iHandleID, double iCheckAt, int iDepth )
{
    double totalLinkLength = GetTotalSampleLinkLength();
    FOdysseyVectorHandleSegment* segmentHandle0 = iCubicSegment->GetHandle( 0 );
    FOdysseyVectorHandleSegment* segmentHandle1 = iCubicSegment->GetHandle( 1 );
    FOdysseyVectorHandleSegment* segmentHandle = iCubicSegment->GetHandle( iHandleID );
    FOdysseyVectorVertex* cubicVertex = static_cast<FOdysseyVectorVertex*>( iCubicSegment->GetVertex( iHandleID ) );
    FOdysseyVectorVertex* cubicVertex0 = static_cast<FOdysseyVectorVertex*>( iCubicSegment->GetVertex(0) );
    FOdysseyVectorVertex* cubicVertex1 = static_cast<FOdysseyVectorVertex*>( iCubicSegment->GetVertex(1) );
    ::ULIS::FVec2D& point0 = cubicVertex0->GetCoords();
    ::ULIS::FVec2D& point1 = cubicVertex1->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = segmentHandle0->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = segmentHandle1->GetCoords();
    ::ULIS::FVec2D& handlePoint = segmentHandle->GetCoords();
    ::ULIS::FVec2D expectedPoint = CubicBezierPointAtParameter( point0, ctrlPoint0, ctrlPoint1, point1, iCheckAt );
    ::ULIS::FVec2D sampledPoint = GetSamplePointAtParameter( totalLinkLength, iCheckAt );
    ::ULIS::FVec2D& vertexPoint = cubicVertex->GetCoords();
    ::ULIS::FVec2D vertexPointToExpectedPoint = expectedPoint - vertexPoint;
    ::ULIS::FVec2D vertexPointToSampledPoint = sampledPoint - vertexPoint;
    ::ULIS::FVec2D vertexPointToHandlePoint = handlePoint - vertexPoint;

    if ( vertexPointToExpectedPoint.DistanceSquared() && vertexPointToSampledPoint.DistanceSquared() && vertexPointToHandlePoint.DistanceSquared() )
    {
        ::ULIS::FVec2D direction = vertexPointToHandlePoint;

        vertexPointToExpectedPoint.Normalize();
        vertexPointToSampledPoint.Normalize();
        vertexPointToHandlePoint.Normalize();

        double dot0 = vertexPointToHandlePoint.DotProduct( vertexPointToExpectedPoint );
        double angle0 = acos( ULIS::FMath::Clamp<double>( dot0, -1.0f, 1.0f ) );
        double dot1 = vertexPointToHandlePoint.DotProduct( vertexPointToSampledPoint );
        double angle1 = acos( ULIS::FMath::Clamp<double>( dot1, -1.0f, 1.0f ) );

        if ( dot0 )
        {
            double ratio = fabs( dot1 / dot0 );
//UE_LOG(LogTemp, Warning, TEXT("Sample: %f %f"), sampledPoint.x, sampledPoint.y );
//UE_LOG(LogTemp, Warning, TEXT("Some warning message %f %f %f %f %f"), ratio );

            segmentHandle->Set( vertexPoint.x + ( direction.x * ratio ),
                                vertexPoint.y + ( direction.y * ratio ) );

            if( iDepth > 0 )
                AdjustHandle( iCubicSegment, iHandleID, ( cubicVertex->GetT( iCubicSegment ) + iCheckAt) * 0.5f, iDepth - 1 );
        }
    }
}

FOdysseyVectorPathCubic* 
FOdysseyVectorPathBuilder::GetCubicPath()
{
    return mCubicPath;
}

void
FOdysseyVectorPathBuilder::FitSegment( FOdysseyVectorSegmentCubic& iSegment
                                     , std::vector<FOdysseyVectorLink>& iLinkArray )
{
    ::ULIS::FVec2D& point0 = iSegment.GetVertex(0)->GetCoords();
    ::ULIS::FVec2D& point1 = iSegment.GetVertex(1)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = iSegment.GetHandle(0)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = iSegment.GetHandle(1)->GetCoords();
    double sampleLength = 0.0f;
    double currentT = 0.0f;

    for( int i = 0; i < iLinkArray.size(); i++ )
    {
        FOdysseyVectorLink *link = &iLinkArray[i];

        sampleLength += link->GetStraightDistance();
    }

    if ( sampleLength )
    {
        double A1 = 0.0f, A2 = 0.0f, A12 = 0.0f;
        ::ULIS::FVec2D C1 = { 0.0f, 0.0f };
        ::ULIS::FVec2D C2 = { 0.0f, 0.0f };

        for( int i = 0; i < iLinkArray.size(); i++)
        {
            FOdysseyVectorLink *link = &iLinkArray[i];
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

    strokeColor.setR( mObjectParam.Foreground.R );
    strokeColor.setG( mObjectParam.Foreground.G );
    strokeColor.setB( mObjectParam.Foreground.B );
    strokeColor.setA( mObjectParam.Foreground.A );

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

    blctx->save();
    blctx->resetMatrix();
    blctx->setFillStyle(BLRgba32(0xFFFF00FF));
    for( uint32 i = 0; i < mSampleArray.size() - 1; i++ )
    {
        BLPoint pt = mWorldMatrix.mapPoint( mSampleArray[i]->GetX(), mSampleArray[i]->GetY() );

        blctx->fillRect( pt.x - 3, pt.y - 3, 6, 6  );
    }
    blctx->restore();
}
