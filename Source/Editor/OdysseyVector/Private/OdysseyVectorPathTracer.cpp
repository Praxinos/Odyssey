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
    , mAngleLimit ( 1.0472f ) // 60 deg
    //, mAngleLimit ( 0.7071f ) // 45 deg
    , mPointID( 0 )
    , mSampleDistance( 12.0f )
    , mCubicPath ( nullptr )
    , mLastVertex ( nullptr )
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

    mPointArray.clear();
    mRecordArray.clear();
    mEdgeArray.clear();
    mCubicPath = nullptr;
    mLastVertex = nullptr;
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

bool
FOdysseyVectorPathTracer::TestBezier( FTracerBezierPoint iBezier[4] )
{
    if( ( fabs( iBezier[0].coords.x - iBezier[1].coords.x ) < 1.0f )
     && ( fabs( iBezier[0].coords.x - iBezier[2].coords.x ) < 1.0f )
     && ( fabs( iBezier[0].coords.x - iBezier[3].coords.x ) < 1.0f )
     && ( fabs( iBezier[0].coords.y - iBezier[1].coords.y ) < 1.0f )
     && ( fabs( iBezier[0].coords.y - iBezier[2].coords.y ) < 1.0f )
     && ( fabs( iBezier[0].coords.y - iBezier[3].coords.y ) < 1.0f ) )
    {
        int32 x = (int)iBezier[0].coords.x;
        int32 y = (int)iBezier[0].coords.y;

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
        FTracerBezierPoint childBezier[2][4];

        memcpy( childBezier[0], iBezier, sizeof( childBezier[0] ) );
        memcpy( childBezier[1], iBezier, sizeof( childBezier[1] ) );

        ::ULIS::CubicBezierSplitAtParameter       <::ULIS::FVec2D>( &childBezier[0][0].coords
                                                                  , &childBezier[0][1].coords
                                                                  , &childBezier[0][2].coords
                                                                  , &childBezier[0][3].coords
                                                                  , 0.5f );
        if( TestBezier( childBezier[0] ) == false )
        {
            return false;
        }

        ::ULIS::CubicBezierInverseSplitAtParameter<::ULIS::FVec2D>( &childBezier[1][0].coords
                                                                  , &childBezier[1][1].coords
                                                                  , &childBezier[1][2].coords
                                                                  , &childBezier[1][3].coords
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
    ::ULIS::FVec2D firstEdgeVector = firstEdge->vector * linkChainLength * 0.35f;
    ::ULIS::FVec2D lastEdgeVector = lastEdge->vector * linkChainLength * 0.35f;
    FTracerBezierPoint currentBezier[4];

    currentBezier[0].id = firstRecord->id;
    currentBezier[3].id = lastRecord->id;

    currentBezier[0].radius = firstRecord->radius;
    currentBezier[3].radius = lastRecord->radius;

    currentBezier[0].coords = firstRecordCoords;
    currentBezier[1].coords = firstRecordCoords + firstEdgeVector;
    currentBezier[2].coords = lastRecordCoords - lastEdgeVector;
    currentBezier[3].coords = lastRecordCoords;

    if( ( iForce == true ) || ( TestBezier( currentBezier ) == true ) )
    {
        mCandidateBezier[0] = currentBezier[0];
        mCandidateBezier[1] = currentBezier[1];
        mCandidateBezier[2] = currentBezier[2];
        mCandidateBezier[3] = currentBezier[3];

        return true;
    }

    return false;
}

void
FOdysseyVectorPathTracer::ClearRecordsUntil( std::vector<FTracerRecord>& iRecordArray, uint32 iID )
{
    int i, j, rank;

    for( i = 0, rank = 0; i < iRecordArray.size(); i++, rank++ )
    {
        if( iRecordArray[i].id == iID ) break;
    }

    for( i = rank, j = 0; i < iRecordArray.size(); i++, j++ )
    {
        iRecordArray[j] = iRecordArray[i];
    }

    iRecordArray.resize( iRecordArray.size() - rank );
}

void
FOdysseyVectorPathTracer::ClearEdgesUntil( std::vector<FTracerEdge>& iEdgeArray, uint32 iID )
{
    int i, j, rank;

    for( i = 0, rank = 0; i < iEdgeArray.size(); i++, rank++ )
    {
        if( iEdgeArray[i].id == iID ) break;
    }

    for( i = rank, j = 0; i < iEdgeArray.size(); i++, j++ )
    {
        iEdgeArray[j] = iEdgeArray[i];
    }

    iEdgeArray.resize( iEdgeArray.size() - rank );

    UE_LOG(LogTemp, Warning, TEXT("iEdgeArray: %d"), iEdgeArray.size() );
}

void
FOdysseyVectorPathTracer::CommitSegment()
{
    BLMatrix2D& cubicPathInverseWorldMatrix = mCubicPath->GetInverseWorldMatrix();
    BLPoint localHandlePoint[2] = { cubicPathInverseWorldMatrix.mapPoint( mCandidateBezier[1].coords.x
                                                                        , mCandidateBezier[1].coords.y )
                                  , cubicPathInverseWorldMatrix.mapPoint( mCandidateBezier[2].coords.x
                                                                        , mCandidateBezier[2].coords.y ) };
    BLPoint localPoint = { cubicPathInverseWorldMatrix.mapPoint( mCandidateBezier[3].coords.x
                                                               , mCandidateBezier[3].coords.y ) };
    BLPoint localVector = cubicPathInverseWorldMatrix.mapVector( mCandidateBezier[3].radius * 0.7071f
                                                               , mCandidateBezier[3].radius * 0.7071f );
    double localRadius = ::ULIS::FVec2D( localVector.x, localVector.y ).Distance();
    FOdysseyVectorVertex* newVertex = new FOdysseyVectorVertex( mCubicPath
                                                              , localPoint.x
                                                              , localPoint.y
                                                              , localRadius );
    FOdysseyVectorSegmentCubic* newCubicSegment = new FOdysseyVectorSegmentCubic( mCubicPath
                                                                                , mLastVertex
                                                                                , localHandlePoint[0].x
                                                                                , localHandlePoint[0].y
                                                                                , localHandlePoint[1].x
                                                                                , localHandlePoint[1].y
                                                                                , newVertex
                                                                                , true );
    mCubicPath->AddVertex( newVertex );
    mCubicPath->AddSegment( newCubicSegment );

    ClearRecordsUntil( mRecordArray, mCandidateBezier[3].id );
    ClearEdgesUntil( mEdgeArray, mCandidateBezier[3].id );

    mLastVertex = newVertex;
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

        mLastVertex = newVertex;
    }
    else
    {
        FTracerRecord& lastRecord = mRecordArray.back();
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

        if( ::ULIS::FVec2D( lastRecord.coords.x - iWorldX
                          , lastRecord.coords.y - iWorldY ).Distance() > mSampleDistance )
        {
            // points are no more needed, clear them
            //ClearPointsUntil( mPointArray, mPointID );

            mRecordArray.emplace_back( mPointID, iWorldX, iWorldY, iRadius );

            mEdgeArray.emplace_back( lastRecord.id, lastRecord.coords.x, lastRecord.coords.y, iWorldX, iWorldY );

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
