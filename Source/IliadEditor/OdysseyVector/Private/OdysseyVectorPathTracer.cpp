// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyVectorPathTracer.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVector.h"

FOdysseyVectorPathTracer::~FOdysseyVectorPathTracer()
{
}

FOdysseyVectorPathTracer::FOdysseyVectorPathTracer()
    : mDotLimit ( 0.0 ) // 90 deg
    //, mDotLimit ( 0.7071f ) // cos 45deg
    , mPointID( 0 )
    , mSampleDistance( 3.0f )
    , mTracingWidth( 6.0f )
    , mCubicPath(nullptr)
{
    mPointBuffer.reserve(100);
    mRecordBuffer.reserve(100);
    mEdgeBuffer.reserve(100);
}

void
FOdysseyVectorPathTracer::SetTracingWidth( double iTracingWidth )
{
    mTracingWidth = iTracingWidth;
}

void
FOdysseyVectorPathTracer::SetDotLimit( double iDotLimit )
{
    mDotLimit = iDotLimit;
}

void
FOdysseyVectorPathTracer::Init( FOdysseyVectorGroupPaint* iScene )
{
    Reset();
}

void
FOdysseyVectorPathTracer::Reset()
{
    mBestBezier.inited = false;
    mPointBuffer.clear();
    mRecordBuffer.clear();
    mEdgeBuffer.clear();
    mCubicPath = nullptr;
    mPointID = 0;
}

FOdysseyVectorSegment*
FOdysseyVectorPathTracer::Flush( FOdysseyVectorVertex* iPreviousVertex
                               , FOdysseyVectorVertex* iEndVertex )
{
    FOdysseyVectorSegment* newSegment = nullptr;

    // forbid path with a signle vertex that has a segment that loops on itself
    if ( ( mCubicPath->GetVertexList().size() == 1 )
      && ( mCubicPath->GetVertexList().front() == iEndVertex ) )
    {
        Reset();

        return nullptr;
    }

    if( mEdgeBuffer.size() )
    {
        MakeBezier( true );
        // newSegment will be nullptr if iEndVertex == mPreviousVertex
        newSegment = CommitSegment( iPreviousVertex
                                  , iEndVertex ? iEndVertex : CommitVertex( false ) );

        // relocate the last vertex at the last entry
        if( ( iEndVertex == nullptr ) && mPointBuffer.size() )
        {
            BLMatrix2D& cubicPathInverseWorldMatrix = mCubicPath->GetInverseWorldMatrix();
            ::ULIS::FVec2D lastPointCoords = mPointBuffer.back().coords;
            BLPoint localPoint = { cubicPathInverseWorldMatrix.mapPoint( lastPointCoords.x
                                                                       , lastPointCoords.y ) };

            newSegment->GetVertex(1)->Set( localPoint.x, localPoint.y );
        }
    }

    Reset();

    return newSegment;
}

std::vector<FTracerPoint>&
FOdysseyVectorPathTracer::GetPointBuffer()
{
    return mPointBuffer;
}

std::vector<FTracerRecord>&
FOdysseyVectorPathTracer::GetRecordBuffer()
{
    return mRecordBuffer;
}

std::vector<FTracerEdge>&
FOdysseyVectorPathTracer::GetEdgeBuffer()
{
    return mEdgeBuffer;
}

double
FOdysseyVectorPathTracer::GetEdgeChainLength()
{
    double length = 0.0f;

    for( int i = 0; i < mEdgeBuffer.size(); i++ )
    {
        length += mEdgeBuffer[i].length;
    }

    return length;
}

/* AdjustBezier()
 *
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

::ULIS::FVec2D
FOdysseyVectorPathTracer::GetSamplePointAtParameter( double iEdgeChainLength, double iAt )
{
    double cumulEdgeLength = 0.0f;
    double ti, tf = 0.0f;
    ::ULIS::FVec2D samplePoint;

    for( int i = 0; i < mEdgeBuffer.size(); i++ )
    {
        cumulEdgeLength += mEdgeBuffer[i].length;

        ti = tf;
        tf = cumulEdgeLength / iEdgeChainLength;

        if( ( iAt >= ti ) && ( iAt <= tf ) )
        {
            double t = iAt - ti;

            samplePoint = mEdgeBuffer[i].p0 + ( mEdgeBuffer[i].p1 - mEdgeBuffer[i].p0 ) * t;

            break;
        }
    }

    return samplePoint;
}

void
FOdysseyVectorPathTracer::AdjustBezier( ::ULIS::FVec2D iBezier[4], double iEdgeChainLength )
{
    ::ULIS::FVec2D expectedPoint = CubicBezierPointAtParameter( iBezier[0]
                                                              , iBezier[1]
                                                              , iBezier[2]
                                                              , iBezier[3]
                                                              , 0.5f );
    ::ULIS::FVec2D sampledPoint = GetSamplePointAtParameter( iEdgeChainLength, 0.5f );

    AdjustBezierHandle( iBezier, expectedPoint, sampledPoint, 0 );
    AdjustBezierHandle( iBezier, expectedPoint, sampledPoint, 3 );
}

void
FOdysseyVectorPathTracer::AdjustBezierHandle( ::ULIS::FVec2D iBezier[4]
                                            , ::ULIS::FVec2D& iExpectedPoint
                                            , ::ULIS::FVec2D& iSampledPoint
                                            , uint32 iAt )
{
    uint32 pointID = iAt;
    uint32 handleID = ( iAt == 0 ) ? 1 : 2;
//    double totalLinkLength = GetTotalSampleLinkLength();

//     ::ULIS::FVec2D expectedPoint = CubicBezierPointAtParameter( iBezier[0]
//                                                               , iBezier[1]
//                                                               , iBezier[2]
//                                                               , iBezier[3]
//                                                               , 0.5f );
    //::ULIS::FVec2D sampledPoint = GetSamplePointAtParameter( totalLinkLength, 0.5f );
    ::ULIS::FVec2D& endPoint = iBezier[pointID];
    ::ULIS::FVec2D& handlePoint = iBezier[handleID];
    ::ULIS::FVec2D endPointToExpectedPoint = iExpectedPoint - endPoint;
    ::ULIS::FVec2D endPointToSampledPoint = iSampledPoint - endPoint;
    ::ULIS::FVec2D endPointToHandlePoint = handlePoint - endPoint;

    if ( endPointToExpectedPoint.DistanceSquared()
      && endPointToSampledPoint.DistanceSquared()
      && endPointToHandlePoint.DistanceSquared() )
    {
        ::ULIS::FVec2D direction = endPointToHandlePoint;
        endPointToExpectedPoint.Normalize();
        endPointToSampledPoint.Normalize();
        endPointToHandlePoint.Normalize();

        double dot0 = endPointToHandlePoint.DotProduct( endPointToExpectedPoint );
        double angle0 = acos( ULIS::FMath::Clamp<double>( dot0, -1.0f, 1.0f ) );
        double dot1 = endPointToHandlePoint.DotProduct( endPointToSampledPoint );
        double angle1 = acos( ULIS::FMath::Clamp<double>( dot1, -1.0f, 1.0f ) );

        if ( angle1 > 0.1f )
        {
            double ratio = angle0 / angle1;

            handlePoint.x = endPoint.x + ( direction.x * ratio );
            handlePoint.y = endPoint.y + ( direction.y * ratio );
        }
    }
}

bool
FOdysseyVectorPathTracer::TestBezier( ::ULIS::FVec2D iBezier[4] )
{
    // take 3 sample points are check how far they are from the edges
    ::ULIS::FVec2D samples[9] = { ::ULIS::CubicBezierPointAtParameter( iBezier[0]
                                                                     , iBezier[1]
                                                                     , iBezier[2]
                                                                     , iBezier[3]
                                                                     , 0.10f )
                                , ::ULIS::CubicBezierPointAtParameter( iBezier[0]
                                                                     , iBezier[1]
                                                                     , iBezier[2]
                                                                     , iBezier[3]
                                                                     , 0.20f )
                                , ::ULIS::CubicBezierPointAtParameter( iBezier[0]
                                                                     , iBezier[1]
                                                                     , iBezier[2]
                                                                     , iBezier[3]
                                                                     , 0.30f )
                                , ::ULIS::CubicBezierPointAtParameter( iBezier[0]
                                                                     , iBezier[1]
                                                                     , iBezier[2]
                                                                     , iBezier[3]
                                                                     , 0.40f )
                                , ::ULIS::CubicBezierPointAtParameter( iBezier[0]
                                                                     , iBezier[1]
                                                                     , iBezier[2]
                                                                     , iBezier[3]
                                                                     , 0.50f )
                                , ::ULIS::CubicBezierPointAtParameter( iBezier[0]
                                                                     , iBezier[1]
                                                                     , iBezier[2]
                                                                     , iBezier[3]
                                                                     , 0.60f )
                                , ::ULIS::CubicBezierPointAtParameter( iBezier[0]
                                                                     , iBezier[1]
                                                                     , iBezier[2]
                                                                     , iBezier[3]
                                                                     , 0.70f )
                                , ::ULIS::CubicBezierPointAtParameter( iBezier[0]
                                                                     , iBezier[1]
                                                                     , iBezier[2]
                                                                     , iBezier[3]
                                                                     , 0.80f )
                                , ::ULIS::CubicBezierPointAtParameter( iBezier[0]
                                                                     , iBezier[1]
                                                                     , iBezier[2]
                                                                     , iBezier[3]
                                                                     , 0.90f ) };
    double tolerance = mTracingWidth * 0.5f;

    for( uint32 i = 0; i < 9; i++ )
    {
        double minDistance = DBL_MAX;

        for( FTracerEdge& edge : mEdgeBuffer )
        {
            double dist;
            double t = FOdysseyVector::DistanceToSegmentConstrained( samples[i], edge.p0, edge.p1, dist );

            if( dist < minDistance )
            {
                minDistance = dist;
            }
        }

        if( minDistance > tolerance )
        {
            return false;
        }
    }

    return true;
}

FTracerBezier&
FOdysseyVectorPathTracer::GetBestBezier()
{
    return mBestBezier;
}

FTracerBezier&
FOdysseyVectorPathTracer::GetRawBezier()
{
    return mRawBezier;
}

// is there a bezier that matches ? return true if yes, false otherwise.
bool
FOdysseyVectorPathTracer::MakeBezier( bool iForce )
{
    FTracerRecord* firstRecord = &mRecordBuffer.front();
    FTracerRecord* lastRecord = &mRecordBuffer.back();
    ::ULIS::FVec2D& firstRecordCoords = firstRecord->coords;
    ::ULIS::FVec2D& lastRecordCoords = lastRecord->coords;
    FTracerEdge* firstEdge = &mEdgeBuffer.front();
    FTracerEdge* lastEdge = &mEdgeBuffer.back();
    double edgeChainLength = GetEdgeChainLength();
    ::ULIS::FVec2D firstEdgeVector = firstRecord->smooth ? mSmoothVector * edgeChainLength * 0.33f
                                                            : firstEdge->vector * edgeChainLength * 0.33f;
    ::ULIS::FVec2D lastEdgeVector = lastEdge->vector * edgeChainLength * 0.33f;

    //UE_LOG(LogTemp,Warning,TEXT("mRecordBuffer:%d mEdgeBuffer:%d %f"),mRecordBuffer.size(),mEdgeBuffer.size(),edgeChainLength);

    mCandidateBezier.inited = true;
    mCandidateBezier.firstRecordRadius = firstRecord->radius;
    mCandidateBezier.lastRecordRadius = lastRecord->radius;

    mCandidateBezier.firstRecordID = firstRecord->id;
    mCandidateBezier.lastRecordID = lastRecord->id;
    mCandidateBezier.firstEdgeID = firstEdge->id;
    mCandidateBezier.lastEdgeID = lastEdge->id;

    mCandidateBezier.pt[0] = firstRecordCoords;
    mCandidateBezier.pt[1] = firstRecordCoords + firstEdgeVector;
    mCandidateBezier.pt[2] = lastRecordCoords - lastEdgeVector;
    mCandidateBezier.pt[3] = lastRecordCoords;

    // raw bezier is the bezier before adjustement. For debugging purposes only
    mRawBezier = mCandidateBezier;

    AdjustBezier( mCandidateBezier.pt, edgeChainLength );

    if( 1 ) // the else statement is disabled for now
    {
        if( ( iForce == true ) || ( TestBezier( mCandidateBezier.pt ) == true ) )
        {
            mBestBezier = mCandidateBezier;

            return true;
        }
    }
    else // disabled for now. Testing some "perfect mode" thats generates 1 bezier everytime the direction changes
         // or everytime we can't find a better bezier
    {
        mBestBezier = mCandidateBezier;

        if( mEdgeBuffer.size() > 1 )
        {
            FTracerEdge* anteEdge = &mEdgeBuffer[mEdgeBuffer.size()-2];
            static double limit = 0.99939082701f; // cos( 2deg );

            if( ( anteEdge->vector.DotProduct( lastEdge->vector ) < limit ) || ( TestBezier( mCandidateBezier.pt ) == false ) )
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        else
        {
            return true;
        }
    }

    // if we never found any best bezier, then we use the last candidate
    if( mBestBezier.inited == false )
    {
        mBestBezier = mCandidateBezier;
    }

    return false;
}

void
FOdysseyVectorPathTracer::ClearPointsTo( uint32 iPointID )
{
    std::vector<FTracerPoint> newPointArray;
    int pointRank = 0;

    // clear points until the one passed as parameter (but keep it)
    newPointArray.reserve( mPointBuffer.size() );

    while( mPointBuffer[pointRank++].id != iPointID );

    for( int i = --pointRank, j = 0; i < mPointBuffer.size(); i++, j++ )
    {
        newPointArray.push_back( mPointBuffer[i] );
    }

    mPointBuffer = newPointArray;
}

void
FOdysseyVectorPathTracer::ClearTo( uint32 iRecordID, uint32 iEdgeID )
{
    std::vector<FTracerRecord> newRecordArray;
    std::vector<FTracerEdge> newEdgeArray;
    int recordRank = 0;
    int edgeRank = 0;

    // clear records until the one passed as parameter (but keep it)
    newRecordArray.reserve( mRecordBuffer.size() );

    while( mRecordBuffer[recordRank++].id != iRecordID );

    for( int i = --recordRank, j = 0; i < mRecordBuffer.size(); i++, j++ )
    {
        newRecordArray.push_back( mRecordBuffer[i] );
    }

    mRecordBuffer = newRecordArray;

    newEdgeArray.reserve( mEdgeBuffer.size() );

    while( mEdgeBuffer[edgeRank++].id != iEdgeID );

    for( int i = edgeRank, j = 0; i < mEdgeBuffer.size(); i++, j++ )
    {
        newEdgeArray.push_back( mEdgeBuffer[i] );
    }

    mEdgeBuffer = newEdgeArray;
}

FOdysseyVectorVertex*
FOdysseyVectorPathTracer::CommitVertex( bool iIsHandleAligned )
{
    BLMatrix2D& cubicPathInverseWorldMatrix = mCubicPath->GetInverseWorldMatrix();
    BLPoint localPoint = { cubicPathInverseWorldMatrix.mapPoint( mBestBezier.pt[3].x
                                                               , mBestBezier.pt[3].y ) };
    BLPoint localVector = cubicPathInverseWorldMatrix.mapVector( mBestBezier.lastRecordRadius * 0.7071f
                                                               , mBestBezier.lastRecordRadius * 0.7071f );
    double localRadius = ::ULIS::FVec2D( localVector.x, localVector.y ).Distance();
    FOdysseyVectorVertex* newVertex = new FOdysseyVectorVertex( localPoint.x
                                                              , localPoint.y
                                                              , localRadius );

    mCubicPath->AddVertex( newVertex );

    newVertex->SetHandleAligned( iIsHandleAligned );

    return newVertex;
}

FOdysseyVectorSegment*
FOdysseyVectorPathTracer::CommitSegment( FOdysseyVectorVertex* iPreviousVertex
                                       , FOdysseyVectorVertex* iEndVertex )
{
    if( iEndVertex != iPreviousVertex )
    {
        BLMatrix2D& cubicPathInverseWorldMatrix = mCubicPath->GetInverseWorldMatrix();
        BLPoint localHandlePoint[2] = { cubicPathInverseWorldMatrix.mapPoint( mBestBezier.pt[1].x
                                                                            , mBestBezier.pt[1].y )
                                      , cubicPathInverseWorldMatrix.mapPoint( mBestBezier.pt[2].x
                                                                            , mBestBezier.pt[2].y ) };
        FOdysseyVectorSegmentCubic* newCubicSegment = new FOdysseyVectorSegmentCubic( mCubicPath
                                                                                    , iPreviousVertex
                                                                                    , localHandlePoint[0].x
                                                                                    , localHandlePoint[0].y
                                                                                    , localHandlePoint[1].x
                                                                                    , localHandlePoint[1].y
                                                                                    , iEndVertex
                                                                                    , true );
        std::vector<double> pressureProfile;

        mCubicPath->AddSegment( newCubicSegment );

        newCubicSegment->Update( 0 );

        // build pressure profile
        /* unused - commented-out for now.
        {
            pressureProfile.reserve( mRecordBuffer.size() );

            for( FTracerRecord& record : mRecordBuffer )
            {
                pressureProfile.push_back( record.radius );
            }

            newCubicSegment->SetPressureProfile( pressureProfile );
        }
        */

        ClearTo( mBestBezier.lastRecordID, mBestBezier.lastEdgeID );

        // must be done after segments are added to the path
        // so that the topology exists
        if( iPreviousVertex->IsHandleAligned() )
        {
            iPreviousVertex->SetHandleAligned( true );
        }

        mSmoothVector = mBestBezier.pt[3] - mBestBezier.pt[2];

        if( mSmoothVector.Distance() )
        {
            mSmoothVector.Normalize();
        }

        // very important. there is no best bezier anymore.
        mBestBezier.inited = false;

        return newCubicSegment;
    }

    return  nullptr;
}

::ULIS::FRectD
FOdysseyVectorPathTracer::GetRedrawRect()
{
    if( mPointBuffer.size() )
    {
        double xmin = mPointBuffer[0].coords.x
             , ymin = mPointBuffer[0].coords.y
             , xmax = xmin
             , ymax = ymin;
        double maxRadius = mPointBuffer[0].radius;

        for( int i = 1; i < mPointBuffer.size(); i++ )
        {
            if( mPointBuffer[i].coords.x < xmin ) xmin = mPointBuffer[i].coords.x;
            if( mPointBuffer[i].coords.x > xmax ) xmax = mPointBuffer[i].coords.x;
            if( mPointBuffer[i].coords.y < ymin ) ymin = mPointBuffer[i].coords.y;
            if( mPointBuffer[i].coords.y > ymax ) ymax = mPointBuffer[i].coords.y;

            if( mPointBuffer[i].radius > maxRadius ) maxRadius = mPointBuffer[i].radius;
        }

        for( int i = 0; i < mRecordBuffer.size(); i++ )
        {
            if( mRecordBuffer[i].coords.x < xmin ) xmin = mRecordBuffer[i].coords.x;
            if( mRecordBuffer[i].coords.x > xmax ) xmax = mRecordBuffer[i].coords.x;
            if( mRecordBuffer[i].coords.y < ymin ) ymin = mRecordBuffer[i].coords.y;
            if( mRecordBuffer[i].coords.y > ymax ) ymax = mRecordBuffer[i].coords.y;

            if( mRecordBuffer[i].radius > maxRadius ) maxRadius = mRecordBuffer[i].radius;
        }

        return ::ULIS::FRectD::FromMinMax( xmin - maxRadius
                                         , ymin - maxRadius
                                         , xmax + maxRadius
                                         , ymax + maxRadius );
    }

    return ::ULIS::FRectD( 0, 0, 0, 0 );
}

FOdysseyVectorSegment*
FOdysseyVectorPathTracer::Trace( FOdysseyVectorVertex* iPreviousVertex
                               , double iWorldX
                               , double iWorldY
                               , double iRadius )
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyVectorPathTracer::Trace);
    uint32 indexn = mPointBuffer.size();
    FOdysseyVectorSegment* newSegment = nullptr;

    mPointBuffer.emplace_back( mPointID, iWorldX, iWorldY, iRadius );

    if( indexn == 0 )
    {
        mRecordBuffer.emplace_back( mPointID, iWorldX, iWorldY, iRadius );
    }
    else
    {
        FTracerRecord* lastRecord = mRecordBuffer.size() ? &mRecordBuffer.back() : nullptr;
        uint32 indexi = indexn - 1;

        if( ::ULIS::FVec2D( lastRecord->coords.x - iWorldX
                          , lastRecord->coords.y - iWorldY ).Distance() > mSampleDistance )
        {
            uint32 edgeCount = mEdgeBuffer.size();
            FTracerEdge* lastEdge = edgeCount ? &mEdgeBuffer.back() : nullptr;
            FTracerRecord newRecord = FTracerRecord( mPointID, iWorldX, iWorldY, iRadius );
            FTracerEdge newEdge = FTracerEdge( lastRecord->id
                                             , lastRecord->coords.x
                                             , lastRecord->coords.y
                                             , iWorldX
                                             , iWorldY
                                             , lastRecord->radius
                                             , iRadius );

            ClearPointsTo( mPointID );

            // detect if smooth or not
            if( lastEdge )
            {
                if ( lastEdge->vector.DotProduct( newEdge.vector ) > mDotLimit )
                {
                    lastRecord->smooth = true;
                }
            }

            if( ( lastRecord->smooth == false ) && ( lastEdge != nullptr ) )
            {
                FOdysseyVectorVertex* newVertex;

                MakeBezier( true );

                newVertex = CommitVertex( lastRecord->smooth );
                newSegment = CommitSegment( iPreviousVertex, newVertex );

                mRecordBuffer.push_back( newRecord );
                mEdgeBuffer.push_back( newEdge );
            }
            else
            {
                mRecordBuffer.push_back( newRecord );
                mEdgeBuffer.push_back( newEdge );

                if( MakeBezier( false ) == false )
                {
                    FOdysseyVectorVertex* newVertex;

                    newVertex = CommitVertex( lastRecord->smooth );
                    newSegment = CommitSegment( iPreviousVertex, newVertex );
                }
            }
        }
    }

    mPointID++;

    return newSegment;
}

void
FOdysseyVectorPathTracer::AttachPath( FOdysseyVectorPath* iCubicPath )
{
    mCubicPath = iCubicPath;
}

FOdysseyVectorPath*
FOdysseyVectorPathTracer::GetPath()
{
    return mCubicPath;
}
