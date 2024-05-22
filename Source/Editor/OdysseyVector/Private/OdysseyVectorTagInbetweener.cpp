#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVector.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"

FInterpolatedPoint::~FInterpolatedPoint()
{
}

FInterpolatedPoint::FInterpolatedPoint( FOdysseyVectorPoint* iPoint
                                      , double iU
                                      , double iV
                                      , uint32 iPositionCount  )
    : mOriginalPoint( iPoint )
    , mU( iU )
    , mV( iV )
{
    positionBuffer.resize(iPositionCount);
}

FInterpolatedSegment::~FInterpolatedSegment()
{
}

FInterpolatedSegment::FInterpolatedSegment( FOdysseyVectorSegment* iSegment
                                          , FInterpolatedPoint* iInterpolatedPoint0
                                          , FInterpolatedPoint* iInterpolatedPoint1 )
    : mOriginalSegment( iSegment )
    , mInterpolatedVertex{ iInterpolatedPoint0, iInterpolatedPoint1 }
{
}

FInterpolatedSegmentCubic::~FInterpolatedSegmentCubic()
{
}

FInterpolatedSegmentCubic::FInterpolatedSegmentCubic( FOdysseyVectorSegmentCubic* iCubicSegment
                                                    , FInterpolatedPoint* iInterpolatedPoint0
                                                    , FInterpolatedPoint* iInterpolatedHandle0
                                                    , FInterpolatedPoint* iInterpolatedHandle1
                                                    , FInterpolatedPoint* iInterpolatedPoint1 )
    : FInterpolatedSegment( iCubicSegment, iInterpolatedPoint0, iInterpolatedPoint1 )
    , mInterpolatedHandle{ iInterpolatedHandle0, iInterpolatedHandle1 }
{
}

FInterpolatedPath::~FInterpolatedPath()
{
}

FInterpolatedPath::FInterpolatedPath( FOdysseyVectorPath* iPath
                                    , const ::ULIS::FRectD& iSpaceBBox
                                    , const BLMatrix2D& iSpaceInverseMatrix 
                                    , uint32 iInbetweenCount )
    : mOriginalPath( iPath )
{
    BLMatrix2D conversionMatrix;
    uint32 pointID = 0;
    uint32 segmentID = 0;

    FOdysseyVector::MatrixMultiply( iSpaceInverseMatrix
                                  , iPath->GetWorldMatrix()
                                  , conversionMatrix );

                                       // alloc 1 point per vertex
    mInterpolatedPointBuffer.reserve(  iPath->GetVertexList().size()
                                       // alloc 2 handles per segment
                                   + ( iPath->GetSegmentList().size() * 2 ) );

    for( FOdysseyVectorVertex* vertex : iPath->GetVertexList() )
    {
        BLPoint pt = conversionMatrix.mapPoint( vertex->GetX(), vertex->GetY() );
        double spaceX = pt.x - iSpaceBBox.x;
        double spaceY = pt.y - iSpaceBBox.y;
        //double u = std::clamp<double>( spaceX / iSpaceBBox.w, 0.0f, 1.0f );
        //double v = std::clamp<double>( spaceY / iSpaceBBox.h, 0.0f, 1.0f );
        double u = std::clamp<double>( spaceX / iSpaceBBox.w, 0.0f, 1.0f );
        double v = std::clamp<double>( spaceY / iSpaceBBox.h, 0.0f, 1.0f );

            // Note: we add +1 for the target position
        mInterpolatedPointBuffer.emplace_back( vertex, u, v, iInbetweenCount + 1 );

        vertex->SetID( pointID++ );
    }

    for( FOdysseyVectorSegment* segment : iPath->GetSegmentList() )
    {
        FOdysseyVectorVertex* vertex0 = segment->GetVertex(0);
        FOdysseyVectorVertex* vertex1 = segment->GetVertex(1);

        if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
            FOdysseyVectorHandleSegment* handle0 = cubicSegment->GetHandle(0);
            FOdysseyVectorHandleSegment* handle1 = cubicSegment->GetHandle(1);
            BLPoint pt0 = conversionMatrix.mapPoint( handle0->GetX(), handle0->GetY() );
            BLPoint pt1 = conversionMatrix.mapPoint( handle1->GetX(), handle1->GetY() );
            // compute UV for first handle
            double space0X = pt0.x - iSpaceBBox.x;
            double space0Y = pt0.y - iSpaceBBox.y;
            double u0 = std::clamp<double>( space0X / iSpaceBBox.w, 0.0f, 1.0f );
            double v0 = std::clamp<double>( space0Y / iSpaceBBox.h, 0.0f, 1.0f );
            // compute UV for second handle
            double space1X = pt1.x - iSpaceBBox.x;
            double space1Y = pt1.y - iSpaceBBox.y;
            double u1 = std::clamp<double>( space1X / iSpaceBBox.w, 0.0f, 1.0f );
            double v1 = std::clamp<double>( space1Y / iSpaceBBox.h, 0.0f, 1.0f );

            // Note: we add +1 for the target position
            mInterpolatedPointBuffer.emplace_back( handle0, u0, v0, iInbetweenCount + 1 );
            handle0->SetID( pointID++ );
            // Note: we add +1 for the target position
            mInterpolatedPointBuffer.emplace_back( handle1, u1, v1, iInbetweenCount + 1 );
            handle1->SetID( pointID++ );

            mInterpolatedSegmentCubicBuffer.emplace_back( cubicSegment
                                                        , &mInterpolatedPointBuffer[vertex0->GetID()]
                                                        , &mInterpolatedPointBuffer[handle0->GetID()]
                                                        , &mInterpolatedPointBuffer[handle1->GetID()]
                                                        , &mInterpolatedPointBuffer[vertex1->GetID()] );

            cubicSegment->SetID( segmentID++ );
        }
    }
}

/*
uint32
FOdysseyVectorTagInbetweener::MapPoint( FOdysseyVectorObject* iObject
                                      , FOdysseyVectorPoint* iPoint
                                      , double iSpaceX
                                      , double iSpaceY )
{
    double paramX = iSpaceX / mSelectionBox.rect.w;
    double paramY = iSpaceY / mSelectionBox.rect.h;

    if( ( paramX >= 0.0f ) && ( paramX < 1.0f ) && ( paramY >= 0.0f ) && ( paramY < 1.0f ) )
    {
        uint32 rowid = paramX * mGridTool->DivisionsX;
        uint32 colid = paramY * mGridTool->DivisionsY;
        uint32 offset = ( colid * mGridTool->DivisionsX ) + rowid;
        double s = ( iSpaceX - (double) rowid * mCellSizeX ) / mCellSizeX;
        double t = ( iSpaceY - (double) colid * mCellSizeY ) / mCellSizeY;
        FGridPoint gridPoint = { iObject, iPoint, s, t };

        mCellArray[offset].mPointArray.push_back( gridPoint );

        return 1;
    }

    return 0;
}
*/

void
FOdysseyVectorTagInbetweener::Map()
{
    FOdysseyVectorEngine* vectorEngine = mOwner->GetEngine();
    uint32 pathCount = 0;

    mInterpolatedPathBuffer.clear();

    vectorEngine->Traverse
    ( mOwner
    , 0
    , [ this
      , &pathCount ]( FOdysseyVectorObject* object, uint64 travesalFlags ) -> uint64
      {
          FOdysseyVectorTag* objectInbetweenerTag = object->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

          if( ( objectInbetweenerTag == nullptr ) || ( objectInbetweenerTag == this ) )
          {
              if( object->GetClass() == FOdysseyVectorPath::StaticClass() )
              {
                  pathCount++;

                  return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
              }
          }
          else
          {
              return FOdysseyVectorEngine::TRAVERSE_OBJECT_IGNORE_CHILDREN;
          }

          return 0;
      } );

    mInterpolatedPathBuffer.reserve( pathCount );

    vectorEngine->Traverse
    ( mOwner
    , 0
    , [ this ]( FOdysseyVectorObject* object, uint64 travesalFlags ) -> uint64
      {
          BLMatrix2D& inverseSpaceMatrix = mOwner->GetInverseWorldMatrix();
          FOdysseyVectorTag* objectInbetweenerTag = object->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

          if( ( objectInbetweenerTag == nullptr ) || ( objectInbetweenerTag == this ) )
          {
              if( object->GetClass() == FOdysseyVectorPath::StaticClass() )
              {
                  FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

                  mInterpolatedPathBuffer.emplace_back( path
                                                      , mOwner->GetBBox( false )
                                                      , inverseSpaceMatrix
                                                      , mInbetweenCount );

                  return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
              }
          }
          else
          {
              return FOdysseyVectorEngine::TRAVERSE_OBJECT_IGNORE_CHILDREN;
          }

          return 0;
      } );
}

FOdysseyVectorTagInbetweener::~FOdysseyVectorTagInbetweener()
{
    mOwner->GetEngine()->GetSharedEnv()->RemoveTag( this );
}

FOdysseyVectorTagInbetweener::FOdysseyVectorTagInbetweener( FOdysseyVectorObject* iOwnerObject
                                                          , uint32 iNumCellX
                                                          , uint32 iNumCellY
                                                          , uint32 iInbetweenCount )
    : FOdysseyVectorTag( iOwnerObject )
    , mGridType( eInbetweenerGridType::FFD )
    , mNumCellX( iNumCellX )
    , mNumCellY( iNumCellY )
    , mInbetweenCount( iInbetweenCount )
{
    Reset();

    mOwner->GetEngine()->GetSharedEnv()->AddTag( this );
}

void FOdysseyVectorTagInbetweener::Update( uint32 iUpdateFlags )
{
}

void
FOdysseyVectorTagInbetweener::ResetChart()
{
    float step = 1.0f / ( mInbetweenCount + 1 );
    float spacing = step;

    mChart.inbetweenBuffer.clear();
    // Note: +1 for target position
    mChart.inbetweenBuffer.resize( mInbetweenCount + 1 );

    for( uint32 i = 0; i <= mInbetweenCount; i++ )
    {
        mChart.inbetweenBuffer[i].spacing = spacing;

        spacing += step;
    }
}

FInbetweenerChart&
FOdysseyVectorTagInbetweener::GetChart()
{
    return mChart;
}

void
FOdysseyVectorTagInbetweener::Reset()
{
    ::ULIS::FRectD bbox = mOwner->GetBBox( false );
    double x = bbox.x;
    double y = bbox.y;
    double stepx = bbox.w / mNumCellX;
    double stepy = bbox.h / mNumCellY;
    uint32 numVertexX = mNumCellX + 1;
    uint32 numVertexY = mNumCellY + 1;

    mGridPointBuffer.resize( numVertexX * numVertexY );
    mUBinomialCoefficientBuffer.resize( mGridPointBuffer.size() );
    mVBinomialCoefficientBuffer.resize( mGridPointBuffer.size() );

    mGridCellBuffer.resize( mNumCellX * mNumCellY );

    // position vertices
    for( uint32 i = 0; i < numVertexY; i++ )
    {
        for( uint32 j = 0; j < numVertexX; j++ )
        {
            uint32 offset = ( i * numVertexX ) + j;

            mGridPointBuffer[offset].sourcePosition = ::ULIS::FVec2D( x, y );
            mGridPointBuffer[offset].targetPosition = mGridPointBuffer[offset].sourcePosition;

            mGridPointBuffer[offset].u = std::clamp<double>( ( x - bbox.x ) / bbox.w, 0.0f, 1.0f );
            mGridPointBuffer[offset].v = std::clamp<double>( ( y - bbox.y ) / bbox.h, 0.0f, 1.0f );

            x += stepx;
        }

        y += stepy;
        x = bbox.x;
    }

    // design cells
    for( uint32 i = 0; i < mNumCellY; i++ )
    {
        for( uint32 j = 0; j < mNumCellX; j++ )
        {
            uint32 vertexOffset = ( i * numVertexX ) + j;
            uint32 cellOffset   = ( i * mNumCellX  ) + j;

            mGridCellBuffer[cellOffset].point[0] = &mGridPointBuffer[vertexOffset];
            mGridCellBuffer[cellOffset].point[1] = &mGridPointBuffer[vertexOffset+1];
            mGridCellBuffer[cellOffset].point[2] = &mGridPointBuffer[vertexOffset+1+numVertexX];
            mGridCellBuffer[cellOffset].point[3] = &mGridPointBuffer[vertexOffset+numVertexX];
        }
    }

    FFDComputeBinomialCoefficients();

    Map();

    ResetChart();

    Interpolate();
}

static
int Factorial ( int n )
{
    return ( n <= 1 ) ? 1 : n * Factorial ( n - 1 ); 
}

static
double BinomialCoeff ( int n, int k )
{
    int div = Factorial ( k ) * Factorial ( n - k );

    if ( div == 0 ) return 0.0f;

    return ( double ) Factorial ( n ) / div;
}

// precompute binaomial coefficient
void
FOdysseyVectorTagInbetweener::FFDComputeBinomialCoefficients()
{
    uint32 numVertexX = mNumCellX + 1;
    uint32 numVertexY = mNumCellY + 1;

    for ( uint32 i = 0; i < numVertexX; i++ )
    {
        double coeffU = BinomialCoeff ( mNumCellX, i );

        mUBinomialCoefficientBuffer[i] = coeffU;
    }

    for ( uint32 i = 0; i < numVertexY; i++ )
    {
        double coeffV = BinomialCoeff ( mNumCellY, i );

        mVBinomialCoefficientBuffer[i] = coeffV;
    }
}

void
FOdysseyVectorTagInbetweener::FFDDeformPoint( FInterpolatedPoint* iInterpolatedPoint
                                            , uint32 iPositionIndex )
{
    ::ULIS::FRectD bbox = mOwner->GetBBox( false );
    ::ULIS::FVec2D vi = ::ULIS::FVec2D( 0.0f, 0.0f );
    uint32 numVertexX = mNumCellX + 1;
    uint32 numVertexY = mNumCellY + 1;

/*
    iInterpolatedPoint->positionBuffer[iPositionIndex] = ::ULIS::FVec2D( ( bbox.x + ( bbox.w * iInterpolatedPoint->mU ) )
                                                                       , ( bbox.y + ( bbox.h * iInterpolatedPoint->mV ) ) );
*/
    for ( uint32 i = 0; i < numVertexY; i++ )
    {
        ::ULIS::FVec2D vj = ::ULIS::FVec2D( 0.0f, 0.0f );
        double bcv = mVBinomialCoefficientBuffer[i];

        for ( uint32 j = 0; j < numVertexX; j++ )
        {
            double bcu = mUBinomialCoefficientBuffer[j];
            uint32 offset = ( i * numVertexX ) + j;

            vj.x += ( bcu * pow ( ( 1 - iInterpolatedPoint->mU ), (mNumCellX) - j ) * pow ( iInterpolatedPoint->mU, j ) * mGridPointBuffer[offset].u );
            vj.y += ( bcu * pow ( ( 1 - iInterpolatedPoint->mU ), (mNumCellX) - j ) * pow ( iInterpolatedPoint->mU, j ) * mGridPointBuffer[offset].v );
        }

        vi.x += ( bcv * pow ( ( 1 - iInterpolatedPoint->mV ), (mNumCellY) - i ) * pow ( iInterpolatedPoint->mV, i ) * vj.x );
        vi.y += ( bcv * pow ( ( 1 - iInterpolatedPoint->mV ), (mNumCellY) - i ) * pow ( iInterpolatedPoint->mV, i ) * vj.y );
    }

    iInterpolatedPoint->positionBuffer[iPositionIndex] = ::ULIS::FVec2D( ( bbox.x + ( bbox.w * vi.x ) )
                                                                       , ( bbox.y + ( bbox.h * vi.y ) ) );
}

void
FOdysseyVectorTagInbetweener::FFDDeformPaths( uint32 iPositionIndex )
{
    for( FInterpolatedPath& interpolatedPath : mInterpolatedPathBuffer )
    {
        for( FInterpolatedPoint& interpolatedPoint : interpolatedPath.mInterpolatedPointBuffer )
        {
            FFDDeformPoint( &interpolatedPoint, iPositionIndex );
        }
    }
}

void
FOdysseyVectorTagInbetweener::InterpolateInbetween( uint32 iInbetweenIndex )
{
    ::ULIS::FRectD bbox = mOwner->GetBBox( false );

    for( FInbetweenerPoint& point : mGridPointBuffer )
    {
        ::ULIS::FVec2D diff = ( point.targetPosition - point.sourcePosition );
        ::ULIS::FVec2D step = diff * mChart.inbetweenBuffer[iInbetweenIndex].spacing;

        point.motionPosition = point.sourcePosition + step;

        point.u = ( point.motionPosition.x - bbox.x ) / bbox.w;
        point.v = ( point.motionPosition.y - bbox.y ) / bbox.h;
    }

    // deform the path according to grid geometry
    FFDDeformPaths( iInbetweenIndex );
}

void
FOdysseyVectorTagInbetweener::UpdateAnimationCells()
{
    IOdysseyVectorAnimationCell* animationCell = mOwner->GetScene()->GetEngine()->GetAnimationCell();

    if( animationCell )
    {
        int32 animationCellIndex = animationCell->GetIndex();

        // Redraw impacted cells
        for( uint32 i = 0; ( i < mInbetweenCount ) && ( animationCell != nullptr ); i++ )
        {
            IOdysseyVectorAnimationCell* nextAnimationCell = animationCell->GetCellByIndex( animationCellIndex + i + 1 );

            if( nextAnimationCell )
            {
                //nextAnimationCell->GetEngine()->Invalidate();
                nextAnimationCell->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
            }

            animationCell = nextAnimationCell;
        }
    }
}

void
FOdysseyVectorTagInbetweener::Interpolate()
{
    for( uint32 i = 0; i <= mInbetweenCount; i++ )
    {
        InterpolateInbetween( i );
    }
}

uint32
FOdysseyVectorTagInbetweener::GetInbetweenCount()
{
    return mInbetweenCount;
}

void
FOdysseyVectorTagInbetweener::Draw( BLContext* iBLContext
                                  , const ::ULIS::FRectD& iInvalidationArea
                                  , double iAncestorsOpacity
                                  , uint64 iDrawingFlags )
{
    //DrawPaths( iBLContext, iInvalidationArea, iAncestorsOpacity, iDrawingFlags );

    //DrawGrid( iBLContext, iInvalidationArea, iAncestorsOpacity, iDrawingFlags );
}

// when drawn as a shared tag
void
FOdysseyVectorTagInbetweener::Draw( FOdysseyVectorGroupPaint* iDisplayedScene
                                  , BLContext* iBLContext
                                  , const ::ULIS::FRectD& iInvalidationArea
                                  , double iAncestorsOpacity
                                  , uint64 iDrawingFlags )
{
    IOdysseyVectorAnimationCell* displayedCell = iDisplayedScene->GetEngine()->GetAnimationCell();
    IOdysseyVectorAnimationCell* tagCell = mOwner->GetScene()->GetEngine()->GetAnimationCell();

    iBLContext->save();
    iBLContext->resetMatrix();

    iBLContext->setStrokeStyle( BLRgba32( 0, 0, 0, 255 ) );
    iBLContext->setStrokeWidth( 3.0f );

    // if th eobject hasn't been removed from the scene
    if( displayedCell && tagCell )
    {
        uint32 tagCellIndex = tagCell->GetIndex();
        uint32 displayedCellIndex = displayedCell->GetIndex();

        if ( ( displayedCellIndex >    tagCellIndex                     )
          && ( displayedCellIndex <= ( tagCellIndex + mInbetweenCount ) ) )
        {
            DrawPathsInbetween( displayedCellIndex - tagCellIndex - 1, iBLContext );
        }
    }

    iBLContext->restore();
}

void
FOdysseyVectorTagInbetweener::DrawPathsInbetween( uint32 iInbetweenIndex
                                                , BLContext* iBLContext )
{
    BLMatrix2D worldMatrix = mOwner->GetWorldMatrix();

    for( FInterpolatedPath& interpolatedPath : mInterpolatedPathBuffer )
    {
        for( FInterpolatedSegmentCubic& interpolatedCubicSegment : interpolatedPath.mInterpolatedSegmentCubicBuffer )
        {
            FInterpolatedPoint* interpolatedPoint[4] = { interpolatedCubicSegment.mInterpolatedVertex[0]
                                                       , interpolatedCubicSegment.mInterpolatedHandle[0]
                                                       , interpolatedCubicSegment.mInterpolatedHandle[1]
                                                       , interpolatedCubicSegment.mInterpolatedVertex[1] };
            BLPoint pt[4] = { worldMatrix.mapPoint( interpolatedPoint[0]->positionBuffer[iInbetweenIndex].x
                                                  , interpolatedPoint[0]->positionBuffer[iInbetweenIndex].y )
                            , worldMatrix.mapPoint( interpolatedPoint[1]->positionBuffer[iInbetweenIndex].x
                                                  , interpolatedPoint[1]->positionBuffer[iInbetweenIndex].y )
                            , worldMatrix.mapPoint( interpolatedPoint[2]->positionBuffer[iInbetweenIndex].x
                                                  , interpolatedPoint[2]->positionBuffer[iInbetweenIndex].y )
                            , worldMatrix.mapPoint( interpolatedPoint[3]->positionBuffer[iInbetweenIndex].x
                                                  , interpolatedPoint[3]->positionBuffer[iInbetweenIndex].y ) };
            BLPath path;


            path.moveTo ( pt[0].x, pt[0].y );
            path.cubicTo( pt[1].x, pt[1].y
                        , pt[2].x, pt[2].y
                        , pt[3].x, pt[3].y );

            iBLContext->strokePath( path );
        }
    }
}

void
FOdysseyVectorTagInbetweener::MoveInbetween( FInbetweenerInbetween* iInbetween
                                           , float iNewSpacing
                                           , bool iRelative )
{
    if( ( iNewSpacing > 0.0f ) && ( iNewSpacing < 1.0f ) )
    {
        IOdysseyVectorAnimationCell* animationCell = mOwner->GetScene()->GetEngine()->GetAnimationCell();
        int32 inbetweenIndex = iInbetween - &mChart.inbetweenBuffer[0];
        int32 prevIndex  = inbetweenIndex - 1;
        uint32 nextIndex = inbetweenIndex + 1;
        float prevSpacing = prevIndex > -1 ? mChart.inbetweenBuffer[prevIndex].spacing
                                           : 0.0f;
        float nextSpacing = nextIndex < mInbetweenCount ? mChart.inbetweenBuffer[nextIndex].spacing
                                                        : 1.0f;

        if( iRelative == false )
        {
            if( ( iNewSpacing > prevSpacing )
             && ( iNewSpacing < nextSpacing ) )
            {
                iInbetween->spacing = iNewSpacing;

                // recompute single inbetweens
                InterpolateInbetween( inbetweenIndex );
            }
        }
        else
        {
            for( FInbetweenerInbetween& otherInbetween : mChart.inbetweenBuffer )
            {
                if( &otherInbetween != iInbetween )
                {
                    if( otherInbetween.spacing < iInbetween->spacing )
                    {
                        float length = iInbetween->spacing;
                        float ratio = iInbetween->spacing ? ( otherInbetween.spacing / length ) : 0.0f;
                        float newLength = iNewSpacing;

                        otherInbetween.spacing = newLength * ratio;
                    }
                    else
                    {
                        float length = 1.0f - iInbetween->spacing;
                        float ratio = iInbetween->spacing ? ( ( otherInbetween.spacing - iInbetween->spacing ) / length ) : 0.0f;
                        float newLength = 1.0f - iNewSpacing;

                        otherInbetween.spacing = iNewSpacing + ( newLength * ratio );
                    }
                }
            }

            iInbetween->spacing = iNewSpacing;

            // recompute all inbetweens
            Interpolate();
        }
    }
}

std::vector<FInbetweenerCell>&
FOdysseyVectorTagInbetweener::GetGridCellBuffer()
{
    return mGridCellBuffer;
}

std::vector<FInbetweenerPoint>&
FOdysseyVectorTagInbetweener::GetGridPointBuffer()
{
    return mGridPointBuffer;
}

void
FOdysseyVectorTagInbetweener::SetInbetweenCount( uint32 iInbetweenCount )
{
    mInbetweenCount = iInbetweenCount;
}

void
FOdysseyVectorTagInbetweener::SetGridType( eInbetweenerGridType iGridType )
{
    mGridType = iGridType;
}

void
FOdysseyVectorTagInbetweener::SetFFDNumCellX( uint32 iNumCellX )
{
    mNumCellX = iNumCellX;
}

void
FOdysseyVectorTagInbetweener::SetFFDNumCellY( uint32 iNumCellY )
{
    mNumCellY = iNumCellY;
}

eInbetweenerGridType
FOdysseyVectorTagInbetweener::GetGridType()
{
    return mGridType;
}

uint32
FOdysseyVectorTagInbetweener::GetFFDNumCellX()
{
    return mNumCellX;
}

uint32
FOdysseyVectorTagInbetweener::GetFFDNumCellY()
{
    return mNumCellY;
}

/*
void
FOdysseyVectorTagInbetweener::DrawPaths( BLContext* iBLContext
                                       , const ::ULIS::FRectD& iInvalidationArea
                                       , double iAncestorsOpacity
                                       , uint64 iDrawingFlags )
{
    BLMatrix2D worldMatrix = mOwner->GetWorldMatrix();

    iBLContext->save();
    iBLContext->resetMatrix();

    iBLContext->setStrokeStyle( BLRgba32( 255, 0, 255, 255 ) );
    iBLContext->setStrokeWidth( 1.0f );

    // note: mInbetweenCount+1 holds the target position
    for( uint32 i = 0; i < mInbetweenCount; i++ )
    {
        DrawPathsInbetween( i
                          , iBLContext
                          , iInvalidationArea
                          , iAncestorsOpacity
                          , iDrawingFlags );
    }

    iBLContext->setStrokeWidth( 3.0f );

    DrawPathsInbetween( mInbetweenCount
                      , iBLContext
                      , iInvalidationArea
                      , iAncestorsOpacity
                      , iDrawingFlags );

    iBLContext->restore();
}


void
FOdysseyVectorTagInbetweener::DrawGrid( BLContext* iBLContext
                                      , const ::ULIS::FRectD& iInvalidationArea
                                      , double iAncestorsOpacity
                                      , uint64 iDrawingFlags )
{
    BLMatrix2D worldMatrix = mOwner->GetWorldMatrix();

    iBLContext->save();
    iBLContext->resetMatrix();

    iBLContext->setStrokeStyle( BLRgba32( 255, 0, 255, 255 ) );
    iBLContext->setStrokeWidth( 1.0f );

    for( FInbetweenerCell& cell : mGridCellBuffer )
    {
        BLPoint pt[4] = { worldMatrix.mapPoint( cell.point[0]->targetPosition.x
                                              , cell.point[0]->targetPosition.y )
                        , worldMatrix.mapPoint( cell.point[1]->targetPosition.x
                                              , cell.point[1]->targetPosition.y )
                        , worldMatrix.mapPoint( cell.point[2]->targetPosition.x
                                              , cell.point[2]->targetPosition.y )
                        , worldMatrix.mapPoint( cell.point[3]->targetPosition.x
                                              , cell.point[3]->targetPosition.y ) };

        iBLContext->strokeLine( pt[0], pt[1] );
        iBLContext->strokeLine( pt[1], pt[2] );
        iBLContext->strokeLine( pt[2], pt[3] );
        iBLContext->strokeLine( pt[3], pt[0] );
    }

    iBLContext->restore();
}
*/

