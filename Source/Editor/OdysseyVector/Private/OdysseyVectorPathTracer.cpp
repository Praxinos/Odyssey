#include "OdysseyVectorPathTracer.h"

FOdysseyVectorPathTracer::~FOdysseyVectorPathTracer()
{
    mBLContext->end();

    if( mBLImage )
    {
        delete mBLImage;
    }
}

FOdysseyVectorPathTracer::FOdysseyVectorPathTracer()
    : mCumulAngle ( 0.0f )
    , mCumulAngleLimit ( 1.5708f ) // 90 degrees
    //, mAngleLimit ( 0.86 ) // 60 deg
    , mAngleLimit ( 0.7071f ) // cos 45deg
    , mPointID( 0 )
    , mSampleDistance( 12.0f )
    , mCubicPath ( nullptr )
    , mPreviousVertex ( nullptr )
{
    mPointArray.reserve(100);
    mRecordArray.reserve(100);
    mEdgeArray.reserve(100);

    mBLContext = new BLContext();
}

void
FOdysseyVectorPathTracer::Init( FOdysseyVectorScene* iScene )
{
    BLImageData imageData;

    iScene->GetEngine()->GetColorImageSize( &mWidth, &mHeight );

    if( mBLImage )
    {
        delete mBLImage;
    }

    mBLImage = new BLImage( mWidth, mHeight, BL_FORMAT_A8 );

    mBLImage->getData( &imageData );

    mPixelData = (uint8*)imageData.pixelData;

    mBLContext->begin( *mBLImage );
    mBLContext->clearAll();

    Flush();
}

void
FOdysseyVectorPathTracer::Flush()
{
    if( mEdgeArray.size() )
    {
        MakeBezier( true );
        CommitSegment();
    }

    mBestBezier.inited = false;

    mPointArray.clear();
    mRecordArray.clear();
    mEdgeArray.clear();
    mCubicPath = nullptr;
    mPreviousVertex = nullptr;
    mPointID = 0;
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


void
FOdysseyVectorPathTracer::AdjustBezier( ::ULIS::FVec2D iBezier[4] )
{
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

bool
FOdysseyVectorPathTracer::MakeBezier( bool iForce )
{
    FTracerRecord* firstRecord = &mRecordArray.front();
    FTracerRecord* lastRecord = &mRecordArray.back();
    ::ULIS::FVec2D& firstRecordCoords = firstRecord->coords;
    ::ULIS::FVec2D& lastRecordCoords = lastRecord->coords;
    FTracerEdge* firstEdge = &mEdgeArray.front();
    FTracerEdge* lastEdge = &mEdgeArray.back();
    double linkChainLength = GetEdgeChainLength();
    ::ULIS::FVec2D firstEdgeVector = firstRecord->smooth ? mSmoothVector * linkChainLength * 0.35f 
                                                         : firstEdge->vector * linkChainLength * 0.35f;
    ::ULIS::FVec2D lastEdgeVector = lastEdge->vector * linkChainLength * 0.35f;

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
    newEdgeArray.reserve( mEdgeArray.size() );

    while( mEdgeArray[edgeRank++].id != iEdgeID );

    for( int i = edgeRank, j = 0; i < mEdgeArray.size(); i++, j++ )
    {
        newEdgeArray.push_back( mEdgeArray[i] );
    }

    mEdgeArray = newEdgeArray;
}

void
FOdysseyVectorPathTracer::CommitSegment()
{
    BLMatrix2D& cubicPathInverseWorldMatrix = mCubicPath->GetInverseWorldMatrix();
    BLPoint localHandlePoint[2] = { cubicPathInverseWorldMatrix.mapPoint( mBestBezier.pt[1].x
                                                                        , mBestBezier.pt[1].y )
                                  , cubicPathInverseWorldMatrix.mapPoint( mBestBezier.pt[2].x
                                                                        , mBestBezier.pt[2].y ) };
    BLPoint localPoint = { cubicPathInverseWorldMatrix.mapPoint( mBestBezier.pt[3].x
                                                               , mBestBezier.pt[3].y ) };
    BLPoint localVector = cubicPathInverseWorldMatrix.mapVector( mBestBezier.lastRecordRadius * 0.7071f
                                                               , mBestBezier.lastRecordRadius * 0.7071f );
    double localRadius = ::ULIS::FVec2D( localVector.x, localVector.y ).Distance();
    FOdysseyVectorVertex* newVertex = new FOdysseyVectorVertex( mCubicPath
                                                              , localPoint.x
                                                              , localPoint.y
                                                              , localRadius );
    FOdysseyVectorSegmentCubic* newCubicSegment = new FOdysseyVectorSegmentCubic( mCubicPath
                                                                                , mPreviousVertex
                                                                                , localHandlePoint[0].x
                                                                                , localHandlePoint[0].y
                                                                                , localHandlePoint[1].x
                                                                                , localHandlePoint[1].y
                                                                                , newVertex
                                                                                , true );
    mCubicPath->AddVertex( newVertex );
    mCubicPath->AddSegment( newCubicSegment );

    newCubicSegment->Update();

    ClearTo( mBestBezier.lastRecordID, mBestBezier.lastEdgeID );

    mPreviousVertex = newVertex;
    mSmoothVector = mBestBezier.pt[3] - mBestBezier.pt[2];

    if( mSmoothVector.Distance() )
    {
        mSmoothVector.Normalize();
    }
}

void
FOdysseyVectorPathTracer::Trace( double iWorldX, double iWorldY, double iRadius )
{
    BLMatrix2D& cubicPathInverseWorldMatrix = mCubicPath->GetInverseWorldMatrix();
    uint32 indexn = mPointArray.size();

    mPointArray.emplace_back( mPointID, iWorldX, iWorldY, iRadius );

    if( indexn == 0 )
    {
        BLPoint localPoint = cubicPathInverseWorldMatrix.mapPoint( iWorldX, iWorldY );
        BLPoint localVector = cubicPathInverseWorldMatrix.mapVector( iRadius * 0.7071f
                                                                   , iRadius * 0.7071f );
        double localRadius = ::ULIS::FVec2D( localVector.x, localVector.y ).Distance();
        FOdysseyVectorVertex* newVertex = new FOdysseyVectorVertex( mCubicPath
                                                                  , localPoint.x
                                                                  , localPoint.y
                                                                  , localRadius );
        mCubicPath->AddVertex( newVertex );

        mRecordArray.emplace_back( mPointID, iWorldX, iWorldY, iRadius );

        mPreviousVertex = newVertex;
    }
    else
    {
        FTracerRecord* lastRecord = mRecordArray.size() ? &mRecordArray.back() : nullptr;
        uint32 indexi = indexn - 1;

        mBLContext->setFillAlpha( 1.0f );
        mBLContext->fillCircle( mPointArray[indexi].coords.x
                              , mPointArray[indexi].coords.y
                              , 2.5f );
        mBLContext->setStrokeAlpha( 1.0f );
        mBLContext->setStrokeWidth( 5.0f );
        mBLContext->strokeLine( mPointArray[indexi].coords.x, mPointArray[indexi].coords.y
                              , mPointArray[indexn].coords.x, mPointArray[indexn].coords.y );
        mBLContext->setFillAlpha( 1.0f );
        mBLContext->fillCircle( mPointArray[indexn].coords.x
                              , mPointArray[indexn].coords.y
                              , 2.5f );

        mBLContext->flush(BL_CONTEXT_FLUSH_SYNC);

        if( ::ULIS::FVec2D( lastRecord->coords.x - iWorldX
                          , lastRecord->coords.y - iWorldY ).Distance() > mSampleDistance )
        {
            uint32 edgeCount = mEdgeArray.size();
            FTracerEdge* lastEdge = edgeCount ? &mEdgeArray.back() : nullptr;

            ClearPointsTo( mPointID );

            mRecordArray.emplace_back( mPointID, iWorldX, iWorldY, iRadius );

            mEdgeArray.emplace_back( lastRecord->id
                                   , lastRecord->coords.x
                                   , lastRecord->coords.y
                                   , iWorldX
                                   , iWorldY );

            // detect if smooth or not
            if( lastEdge )
            {
                if ( lastEdge->vector.DotProduct( mEdgeArray[edgeCount].vector ) > mAngleLimit )
                {
                    lastRecord->smooth = true;
                }
            }

            if( MakeBezier( false ) == false )
            {
                CommitSegment();
            }
        }
    }

    mPointID++;
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
