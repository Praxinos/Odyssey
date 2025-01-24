// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyVectorPathTracer.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorCell.h"

FOdysseyVectorPathTracer::~FOdysseyVectorPathTracer()
{
    mBLContext.end();

    if( mBLImage )
    {
        delete mBLImage;
    }
}

FOdysseyVectorPathTracer::FOdysseyVectorPathTracer()
    : mDotLimit ( 0.0 ) // 90 deg
    //, mDotLimit ( 0.7071f ) // cos 45deg
    , mPointID( 0 )
    , mSampleDistance( 3.0f )
    , mTracingWidth( 6.0f )
    , mCubicPath(nullptr)
{
    mPointArray.reserve(100);
    mRecordArray.reserve(100);
    mEdgeArray.reserve(100);

    mBLContext.setCompOp( BL_COMP_OP_SRC_COPY );
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
    BLImageData imageData;

    mWidth = iScene->GetLayer()->GetWidth();
    mHeight = iScene->GetLayer()->GetHeight();

    if( mBLImage )
    {
        delete mBLImage;
    }

    mBLImage = new BLImage( mWidth, mHeight, BL_FORMAT_A8 );

    mBLImage->getData( &imageData );

    mPixelData = (uint8*)imageData.pixelData;

    mBLContext.begin( *mBLImage );
    mBLContext.clearAll();

    Reset();
}

void
FOdysseyVectorPathTracer::Reset()
{
    mBestBezier.inited = false;
    mPointArray.clear();
    mRecordArray.clear();
    mEdgeArray.clear();
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

    if( mEdgeArray.size() )
    {
        MakeBezier( true );
        // newSegment will be nullptr if iEndVertex == mPreviousVertex
        newSegment = CommitSegment( iPreviousVertex
                                  , iEndVertex ? iEndVertex : CommitVertex( false ) );

        // relocate the last vertex at the last entry
        if( ( iEndVertex == nullptr ) && mPointArray.size() )
        {
            BLMatrix2D& cubicPathInverseWorldMatrix = mCubicPath->GetInverseWorldMatrix();
            ::ULIS::FVec2D lastPointCoords = mPointArray.back().coords;
            BLPoint localPoint = { cubicPathInverseWorldMatrix.mapPoint( lastPointCoords.x
                                                                       , lastPointCoords.y ) };

            newSegment->GetVertex(1)->Set( localPoint.x, localPoint.y );
        }
    }

    Reset();

    return newSegment;
}

std::vector<FTracerPoint>&
FOdysseyVectorPathTracer::GetPointArray()
{
    return mPointArray;
}

std::vector<FTracerRecord>&
FOdysseyVectorPathTracer::GetRecordArray()
{
    return mRecordArray;
}

std::vector<FTracerEdge>&
FOdysseyVectorPathTracer::GetEdgeArray()
{
    return mEdgeArray;
}

double
FOdysseyVectorPathTracer::GetEdgeChainLength()
{
    double length = 0.0f;

    for( int i = 0; i < mEdgeArray.size(); i++ )
    {
        length += mEdgeArray[i].length;
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

    for( int i = 0; i < mEdgeArray.size(); i++ )
    {
        cumulEdgeLength += mEdgeArray[i].length;

        ti = tf;
        tf = cumulEdgeLength / iEdgeChainLength;

        if( ( iAt >= ti ) && ( iAt <= tf ) )
        {
            double t = iAt - ti;

            samplePoint = mEdgeArray[i].p0 + ( mEdgeArray[i].p1 - mEdgeArray[i].p0 ) * t;

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
    if( ( fabs( iBezier[0].x - iBezier[1].x ) < 1.0f )
     && ( fabs( iBezier[0].x - iBezier[2].x ) < 1.0f )
     && ( fabs( iBezier[0].x - iBezier[3].x ) < 1.0f )
     && ( fabs( iBezier[0].y - iBezier[1].y ) < 1.0f )
     && ( fabs( iBezier[0].y - iBezier[2].y ) < 1.0f )
     && ( fabs( iBezier[0].y - iBezier[3].y ) < 1.0f ) )
    {
        int32 x = (int)iBezier[0].x;
        int32 y = (int)iBezier[0].y;

        if( ( x >= 0 ) && ( x < (int)mWidth ) && ( y >= 0 ) && ( y < (int)mHeight ) )
        {
            uint32 offset = ( y * mWidth ) + x;

            if ( mPixelData[offset] == 0 )
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else // refine
    {
        ::ULIS::FVec2D childBezier[2][4];

        memcpy( childBezier[0], iBezier, sizeof( childBezier[0] ) );
        memcpy( childBezier[1], iBezier, sizeof( childBezier[1] ) );

        ::ULIS::CubicBezierSplitAtParameter       <::ULIS::FVec2D>( &childBezier[0][0]
                                                                  , &childBezier[0][1]
                                                                  , &childBezier[0][2]
                                                                  , &childBezier[0][3]
                                                                  , 0.5f );
        if( TestBezier( childBezier[0] ) == false )
        {
            return false;
        }

        ::ULIS::CubicBezierInverseSplitAtParameter<::ULIS::FVec2D>( &childBezier[1][0]
                                                                  , &childBezier[1][1]
                                                                  , &childBezier[1][2]
                                                                  , &childBezier[1][3]
                                                                  , 0.5f );
        if( TestBezier( childBezier[1] ) == false )
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

void
FOdysseyVectorPathTracer::TraceEdge( FTracerEdge* iEdge, double iAlpha )
{
    double circleRadius = mTracingWidth * 0.5f;

    mBLContext.setFillAlpha( iAlpha );
    mBLContext.setStrokeAlpha( iAlpha );
    mBLContext.setStrokeWidth( mTracingWidth );

    mBLContext.fillCircle( iEdge->p0.x, iEdge->p0.y, circleRadius );
    mBLContext.strokeLine( iEdge->p0.x, iEdge->p0.y
                         , iEdge->p1.x, iEdge->p1.y );
    mBLContext.fillCircle( iEdge->p1.x, iEdge->p1.y, circleRadius );
}

void
FOdysseyVectorPathTracer::TraceEdges( double iAlpha )
{
    for( int i = 0; i < mEdgeArray.size(); i++ )
    {
        TraceEdge( &mEdgeArray[i], iAlpha );
    }
}

bool
FOdysseyVectorPathTracer::MakeBezier( bool iForce )
{
    FTracerRecord* firstRecord = &mRecordArray.front();
    FTracerRecord* lastRecord = &mRecordArray.back();
    ::ULIS::FVec2D& firstRecordCoords = firstRecord->coords;
    ::ULIS::FVec2D& lastRecordCoords = lastRecord->coords;
    FTracerEdge* firstEdge = &mEdgeArray.front();
    FTracerEdge* lastEdge = &mEdgeArray.back();
    double edgeChainLength = GetEdgeChainLength();
    ::ULIS::FVec2D firstEdgeVector = firstRecord->smooth ? mSmoothVector * edgeChainLength * 0.33f
                                                         : firstEdge->vector * edgeChainLength * 0.33f;
    ::ULIS::FVec2D lastEdgeVector = lastEdge->vector * edgeChainLength * 0.33f;

    //UE_LOG(LogTemp,Warning,TEXT("mRecordArray:%d mEdgeArray:%d %f"),mRecordArray.size(),mEdgeArray.size(),edgeChainLength);


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

    if( ( iForce == true ) || ( TestBezier( mCandidateBezier.pt ) == true ) )
    {
        mBestBezier = mCandidateBezier;

        return true;
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
    newPointArray.reserve( mPointArray.size() );

    while( mPointArray[pointRank++].id != iPointID );

    for( int i = --pointRank, j = 0; i < mPointArray.size(); i++, j++ )
    {
        newPointArray.push_back( mPointArray[i] );
    }

    mPointArray = newPointArray;
}

void
FOdysseyVectorPathTracer::ClearTo( uint32 iRecordID, uint32 iEdgeID )
{
    std::vector<FTracerRecord> newRecordArray;
    std::vector<FTracerEdge> newEdgeArray;
    int recordRank = 0;
    int edgeRank = 0;

    // clear records until the one passed as parameter (but keep it)
    newRecordArray.reserve( mRecordArray.size() );

    while( mRecordArray[recordRank++].id != iRecordID );

    for( int i = --recordRank, j = 0; i < mRecordArray.size(); i++, j++ )
    {
        newRecordArray.push_back( mRecordArray[i] );
    }

    mRecordArray = newRecordArray;

    // clear edges until the one passed as parameter
    //TraceEdges( 0.0f, 6.0f ); // erase // commented out. For some reason it does not work.
    mBLContext.clearAll();

    newEdgeArray.reserve( mEdgeArray.size() );

    while( mEdgeArray[edgeRank++].id != iEdgeID );

    for( int i = edgeRank, j = 0; i < mEdgeArray.size(); i++, j++ )
    {
        newEdgeArray.push_back( mEdgeArray[i] );
    }

    mEdgeArray = newEdgeArray;
    TraceEdges( 1.0f ); // trace again
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


        mCubicPath->AddSegment( newCubicSegment );

        newCubicSegment->Update( 0 );

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
    if( mPointArray.size() )
    {
        double xmin = mPointArray[0].coords.x
             , ymin = mPointArray[0].coords.y
             , xmax = xmin
             , ymax = ymin;
        double maxRadius = mPointArray[0].radius;

        for( int i = 1; i < mPointArray.size(); i++ )
        {
            if( mPointArray[i].coords.x < xmin ) xmin = mPointArray[i].coords.x;
            if( mPointArray[i].coords.x > xmax ) xmax = mPointArray[i].coords.x;
            if( mPointArray[i].coords.y < ymin ) ymin = mPointArray[i].coords.y;
            if( mPointArray[i].coords.y > ymax ) ymax = mPointArray[i].coords.y;

            if( mPointArray[i].radius > maxRadius ) maxRadius = mPointArray[i].radius;
        }

        for( int i = 0; i < mRecordArray.size(); i++ )
        {
            if( mRecordArray[i].coords.x < xmin ) xmin = mRecordArray[i].coords.x;
            if( mRecordArray[i].coords.x > xmax ) xmax = mRecordArray[i].coords.x;
            if( mRecordArray[i].coords.y < ymin ) ymin = mRecordArray[i].coords.y;
            if( mRecordArray[i].coords.y > ymax ) ymax = mRecordArray[i].coords.y;

            if( mRecordArray[i].radius > maxRadius ) maxRadius = mRecordArray[i].radius;
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
    uint32 indexn = mPointArray.size();
    FOdysseyVectorSegment* newSegment = nullptr;

    mPointArray.emplace_back( mPointID, iWorldX, iWorldY, iRadius );

    if( indexn == 0 )
    {
        mRecordArray.emplace_back( mPointID, iWorldX, iWorldY, iRadius );
    }
    else
    {
        FTracerRecord* lastRecord = mRecordArray.size() ? &mRecordArray.back() : nullptr;
        uint32 indexi = indexn - 1;

        if( ::ULIS::FVec2D( lastRecord->coords.x - iWorldX
                          , lastRecord->coords.y - iWorldY ).Distance() > mSampleDistance )
        {
            uint32 edgeCount = mEdgeArray.size();
            FTracerEdge* lastEdge = edgeCount ? &mEdgeArray.back() : nullptr;
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

            // draw alpha to pixel buffer
            TraceEdge( &newEdge, 1.0f );

            // unsure if useful
            mBLContext.flush(BL_CONTEXT_FLUSH_SYNC);

            if( ( lastRecord->smooth == false ) && ( lastEdge != nullptr ) )
            {
                FOdysseyVectorVertex* newVertex;

                MakeBezier( true );

                newVertex = CommitVertex( lastRecord->smooth );
                newSegment = CommitSegment( iPreviousVertex, newVertex );

                mRecordArray.push_back( newRecord );
                mEdgeArray.push_back( newEdge );
            }
            else
            {
                mRecordArray.push_back( newRecord );
                mEdgeArray.push_back( newEdge );

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

BLImage*
FOdysseyVectorPathTracer::GetBLImage()
{
    return mBLImage;
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
