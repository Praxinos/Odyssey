#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVector.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorAnimationCell.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

FInterpolatedPoint::~FInterpolatedPoint()
{
}

FInterpolatedPoint::FInterpolatedPoint( FOdysseyVectorPoint* iPoint
                                      , uint32 iIndex
                                      , double iU
                                      , double iV  )
    : mOriginalPoint( iPoint )
    , mIndex ( iIndex )
    , mU( iU )
    , mV( iV )
{
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
        mInterpolatedPointBuffer.emplace_back( vertex, mInterpolatedPointBuffer.size(), u, v );

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
            mInterpolatedPointBuffer.emplace_back( handle0, mInterpolatedPointBuffer.size(), u0, v0 );
            handle0->SetID( pointID++ );
            // Note: we add +1 for the target position
            mInterpolatedPointBuffer.emplace_back( handle1, mInterpolatedPointBuffer.size(), u1, v1 );
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

std::vector<::ULIS::FVec2D>&
FInterpolatedPath::GetInterpolatedPointPositionBuffer()
{
    return mInterpolatedPointPositionBuffer;
}

std::vector<FInterpolatedPoint>&
FInterpolatedPath::GetInterpolatedPointBuffer()
{
    return mInterpolatedPointBuffer;
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
        FInbetweenerGridPoint gridPoint = { iObject, iPoint, s, t };

        mCellArray[offset].mPointArray.push_back( gridPoint );

        return 1;
    }

    return 0;
}
*/

FInbetweenerGridPoint::FInbetweenerGridPoint()
{
}

void
FInbetweenerGridPoint::Init( FInbetweenerGrid* iGrid )
{
    mGrid = iGrid;
}

void
FInbetweenerGridPoint::SetSourcePosition( double iX, double iY )
{
    mSourcePosition.x = iX;
    mSourcePosition.y = iY;

    mGrid->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_BBOX );
}

void
FInbetweenerGridPoint::SetTargetPosition( double iX, double iY )
{
    mTargetPosition.x = iX;
    mTargetPosition.y = iY;

    mGrid->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_BBOX );
}

const ::ULIS::FVec2D&
FInbetweenerGridPoint::GetSourcePosition()
{
    return mSourcePosition;
}

const ::ULIS::FVec2D&
FInbetweenerGridPoint::GetTargetPosition()
{
    return mTargetPosition;
}

void
FInbetweenerGridPoint::AddQuad( FInbetweenerGridQuad* iQuad )
{
    mQuadList.push_back( iQuad );
}

void
FInbetweenerGridPoint::RemoveQuad( FInbetweenerGridQuad* iQuad )
{
    mQuadList.remove( iQuad );
}

FInbetweenerGridQuad::FInbetweenerGridQuad( )
    : mFlags ( 0 )
{
}

FInbetweenerGridPoint** 
FInbetweenerGridQuad::GetPoints()
{
    return mPoint;
}

bool
FInbetweenerGridQuad::IsLinked()
{
    return ( mFlags & LINKED ) ? true : false; 
}

void
FInbetweenerGridQuad::Link()
{
    mFlags |= LINKED;

    for( uint32 i = 0; i < 4; i++ )
    {
        mPoint[i]->AddQuad( this );
    }
}

void
FInbetweenerGridQuad::Unlink()
{
    for( uint32 i = 0; i < 4; i++ )
    {
        mPoint[i]->RemoveQuad( this );
    }

    mFlags &= (~LINKED);
}

FInbetweenerGrid::FInbetweenerGrid( FOdysseyVectorTagInbetweener* iInbetweenerTag )
    : mNumQuadX( 0 )
    , mNumQuadY( 0 )
    , mInbetweenerTag( iInbetweenerTag )
{
}

FOdysseyVectorTagInbetweener*
FInbetweenerGrid::GetInbetweenerTag()
{
    return mInbetweenerTag;
}

uint32
FInbetweenerGrid::GetNumQuadX()
{
    return mNumQuadY;
}

uint32
FInbetweenerGrid::GetNumQuadY()
{
    return mNumQuadY;
}

void
FInbetweenerGrid::Make( uint32 iNumQuadX
                      , uint32 iNumQuadY )
{
    ::ULIS::FRectD bbox = mInbetweenerTag->GetOwner()->GetBBox( false );
    double x = bbox.x;
    double y = bbox.y;
    double stepx = bbox.w / iNumQuadX;
    double stepy = bbox.h / iNumQuadY;
    uint32 numVertexX = iNumQuadX + 1;
    uint32 numVertexY = iNumQuadY + 1;

    mNumQuadX = iNumQuadX;
    mNumQuadY = iNumQuadY;

    mPointBuffer.resize( numVertexX * numVertexY );
    mQuadBuffer.resize( mNumQuadX * mNumQuadY );

    // position vertices
    for( uint32 i = 0; i < numVertexY; i++ )
    {
        for( uint32 j = 0; j < numVertexX; j++ )
        {
            uint32 offset = ( i * numVertexX ) + j;

            mPointBuffer[offset].Init( this );
            mPointBuffer[offset].SetSourcePosition( x, y );
            mPointBuffer[offset].mTargetPosition = mPointBuffer[offset].mSourcePosition;

            mPointBuffer[offset].u = std::clamp<double>( ( x - bbox.x ) / bbox.w, 0.0f, 1.0f );
            mPointBuffer[offset].v = std::clamp<double>( ( y - bbox.y ) / bbox.h, 0.0f, 1.0f );

            x += stepx;
        }

        y += stepy;
        x = bbox.x;
    }

    // design cells
    for( uint32 i = 0; i < mNumQuadY; i++ )
    {
        for( uint32 j = 0; j < mNumQuadX; j++ )
        {
            uint32 vertexOffset = ( i * numVertexX ) + j;
            uint32 quadOffset   = ( i * mNumQuadX  ) + j;
            FInbetweenerGridQuad* quad = &mQuadBuffer[quadOffset];
            FInbetweenerGridPoint** gridPoint = quad->GetPoints();

            gridPoint[0] = &mPointBuffer[vertexOffset];
            gridPoint[1] = &mPointBuffer[vertexOffset+1];
            gridPoint[2] = &mPointBuffer[vertexOffset+1+numVertexX];
            gridPoint[3] = &mPointBuffer[vertexOffset+numVertexX];

            quad->Link();
        }
    }
}

FInbetweenerGridFFD::FInbetweenerGridFFD( FOdysseyVectorTagInbetweener* iInbetweenerTag )
    : FInbetweenerGrid( iInbetweenerTag )
{
}

void
FInbetweenerGrid::DeformPaths( std::vector<FInterpolatedPath>& iInterpolatedPathBuffer
                             , uint32 iInbetweenIndex )
{
}

::ULIS::FVec2D
FInbetweenerGridFFD::DeformPoint( FInterpolatedPoint* iInterpolatedPoint )
{
    ::ULIS::FRectD bbox = mInbetweenerTag->GetOwner()->GetBBox( false );
    ::ULIS::FVec2D vi = ::ULIS::FVec2D( 0.0f, 0.0f );
    uint32 numVertexX = mNumQuadX + 1;
    uint32 numVertexY = mNumQuadY + 1;

    for ( uint32 i = 0; i < numVertexY; i++ )
    {
        ::ULIS::FVec2D vj = ::ULIS::FVec2D( 0.0f, 0.0f );
        double bcv = mVBinomialCoefficientBuffer[i];

        for ( uint32 j = 0; j < numVertexX; j++ )
        {
            double bcu = mUBinomialCoefficientBuffer[j];
            uint32 offset = ( i * numVertexX ) + j;

            vj.x += ( bcu * pow ( ( 1 - iInterpolatedPoint->mU ), (mNumQuadX) - j ) * pow ( iInterpolatedPoint->mU, j ) * mPointBuffer[offset].u );
            vj.y += ( bcu * pow ( ( 1 - iInterpolatedPoint->mU ), (mNumQuadX) - j ) * pow ( iInterpolatedPoint->mU, j ) * mPointBuffer[offset].v );
        }

        vi.x += ( bcv * pow ( ( 1 - iInterpolatedPoint->mV ), (mNumQuadY) - i ) * pow ( iInterpolatedPoint->mV, i ) * vj.x );
        vi.y += ( bcv * pow ( ( 1 - iInterpolatedPoint->mV ), (mNumQuadY) - i ) * pow ( iInterpolatedPoint->mV, i ) * vj.y );
    }

    return ::ULIS::FVec2D( ( bbox.x + ( bbox.w * vi.x ) )
                         , ( bbox.y + ( bbox.h * vi.y ) ) );
}

void
FInbetweenerGridFFD::DeformPaths( std::vector<FInterpolatedPath>& iInterpolatedPathBuffer
                                , uint32 iInbetweenIndex )
{

    for( FInterpolatedPath& interpolatedPath : iInterpolatedPathBuffer )
    {
        std::vector<::ULIS::FVec2D>& interpolatedPointPositionBuffer = interpolatedPath.GetInterpolatedPointPositionBuffer();
        uint32 pointCount = interpolatedPath.GetInterpolatedPointBuffer().size();
        uint32 skippedOffset = ( iInbetweenIndex * pointCount );

        for( uint32 i = 0; i < pointCount; i++ )
        {
            FInterpolatedPoint* interpolatedPoint = &interpolatedPath.GetInterpolatedPointBuffer()[i];

            interpolatedPointPositionBuffer[skippedOffset + i] = DeformPoint( interpolatedPoint );
        }
    }
}

std::vector<FInbetweenerGridQuad>&
FInbetweenerGrid::GetQuadBuffer()
{
    return mQuadBuffer;
}

std::vector<FInbetweenerGridPoint>&
FInbetweenerGrid::GetPointBuffer()
{
    return mPointBuffer;
}

void
FInbetweenerGridFFD::Make( uint32 iNumQuadX, uint32 iNumQuadY )
{
    FInbetweenerGrid::Make( iNumQuadX, iNumQuadY );

    mUBinomialCoefficientBuffer.resize( mPointBuffer.size() );
    mVBinomialCoefficientBuffer.resize( mPointBuffer.size() );

    ComputeBinomialCoefficients();
}

FInbetweenerGridARAP::FInbetweenerGridARAP( FOdysseyVectorTagInbetweener* iInbetweenerTag )
    : FInbetweenerGrid( iInbetweenerTag )
{
}

void
FInbetweenerGridARAP::Make( uint32 iNumQuadX, uint32 iNumQuadY )
{
    FInbetweenerGrid::Make( iNumQuadX, iNumQuadY );
}

void
FOdysseyVectorTagInbetweener::UpdateGridBBox()
{
    double xmin = DBL_MAX, ymin = DBL_MAX, xmax = -DBL_MAX, ymax = -DBL_MAX;
    bool hasBBox = false;

    for( FInbetweenerGridPoint& gridPoint : mGrid->GetPointBuffer() )
    {
        const ::ULIS::FVec2D& targetPosition = gridPoint.GetTargetPosition();

        hasBBox = true;

        if ( targetPosition.x < xmin ) xmin = targetPosition.x;
        if ( targetPosition.y < ymin ) ymin = targetPosition.y;
        if ( targetPosition.x > xmax ) xmax = targetPosition.x;
        if ( targetPosition.y > ymax ) ymax = targetPosition.y;
    }

    mTargetGridBBox = ( hasBBox ) ? ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax ) 
                                  : ::ULIS::FRectD( 0.0f, 0.0f, 0.0f, 0.0f );
}

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
    delete mGrid;
}

FOdysseyVectorTagInbetweener::FOdysseyVectorTagInbetweener( FOdysseyVectorSharedEnv* iSharedEnv
                                                          , FOdysseyVectorObject* iOwnerObject
                                                          , uint32 iNumQuadX
                                                          , uint32 iNumQuadY
                                                          , uint32 iInbetweenCount )
    : FOdysseyVectorTag( iOwnerObject )
    // note: mSharedEnv is remebered as a member variable because GetEngine() calls
    // GetClass() and the latter is a virtual function. virtual function don't work
    // in destructors.
    , mSharedEnv ( iSharedEnv )
    , mGrid( nullptr )
    , mGridType( eInbetweenerGridType::FFD )
    , mInbetweenCount( iInbetweenCount )
    , mInvalidationFlags( 0 )
{
    mTargetTranslationX = 0.0f;
    mTargetTranslationY = 0.0f;
    mTargetScalingX     = 1.0f;
    mTargetScalingY     = 1.0f;
    mTargetRotation     = 0.0f;

    UpdateMatrix();

    SetGridType( mGridType );

    //mGrid->Make( iNumQuadX, iNumQuadY, iOwnerObject->GetBBox( false ), this );

    //Map();
    //ResetChart();
    //AllocBuffers();
    //Interpolate();
}

void
FOdysseyVectorTagInbetweener::Translate( double iX, double iY )
{
    mTargetTranslationX = iX;
    mTargetTranslationY = iY;
}

void
FOdysseyVectorTagInbetweener::Rotate( double iAngle )
{
    mTargetRotation = iAngle;
}

void
FOdysseyVectorTagInbetweener::Scale( double iX, double iY )
{
    mTargetScalingX = iX;
    mTargetScalingY = iY;
}

void FOdysseyVectorTagInbetweener::Added()
{
    mSharedEnv->AddTag( this );
}

void FOdysseyVectorTagInbetweener::Removed()
{
    mSharedEnv->RemoveTag( this );
}

void FOdysseyVectorTagInbetweener::Update( uint32 iUpdateFlags )
{
    if( ( iUpdateFlags & FOdysseyVectorObject::INVALIDATE_HIERARCHY )

     || ( iUpdateFlags & FOdysseyVectorObject::INVALIDATE_SHAPE    )
     || ( iUpdateFlags & FOdysseyVectorObject::INVALIDATE_TOPOLOGY )
     || ( iUpdateFlags & FOdysseyVectorObject::INVALIDATE_TAGS     )
     || ( iUpdateFlags & FOdysseyVectorObject::INVALIDATE_MATRIX   )

     || ( iUpdateFlags & FOdysseyVectorObject::INVALIDATE_CHILD_SHAPE    )
     || ( iUpdateFlags & FOdysseyVectorObject::INVALIDATE_CHILD_TOPOLOGY )
     || ( iUpdateFlags & FOdysseyVectorObject::INVALIDATE_CHILD_TAGS     )
     || ( iUpdateFlags & FOdysseyVectorObject::INVALIDATE_CHILD_MATRIX   ) )
    {
        mInvalidationFlags |= ( INVALIDATE_MAP | INVALIDATE_BUFFERS | INVALIDATE_SPACING );
    }

    if( mInvalidationFlags & INVALIDATE_MAP )
    {
        Map();
    }

    if( mInvalidationFlags & INVALIDATE_BUFFERS )
    {
        AllocBuffers();
    }

    if( mInvalidationFlags & INVALIDATE_BBOX )
    {
        UpdateGridBBox();
    }

    if( mInvalidationFlags & INVALIDATE_SPACING )
    {
        Interpolate();
    }

    if( mInvalidationFlags & INVALIDATE_CELLS )
    {
        UpdateAnimationCells();
    }

    // reset tag's invalidation flags (do not confuse with object's invalidation flags)
    mInvalidationFlags = 0;
}

void
FOdysseyVectorTagInbetweener::Invalidate( uint64 iInvalidationFlags )
{
    mOwner->Invalidate( FOdysseyVectorObject::INVALIDATE_TAGS );

    mInvalidationFlags |= iInvalidationFlags;
}

BLMatrix2D&
FOdysseyVectorTagInbetweener::GetTargetWorldMatrix()
{
    return mTargetWorldMatrix;
}

BLMatrix2D&
FOdysseyVectorTagInbetweener::GetTargetInverseWorldMatrix()
{
    return mTargetInverseWorldMatrix;
}

::ULIS::FRectD
FOdysseyVectorTagInbetweener::GetTargetGridBBox( bool iWorld )
{
    if ( iWorld == true )
    {
        BLPoint p0 = mTargetWorldMatrix.mapPoint( mTargetGridBBox.x                    , mTargetGridBBox.y                     );
        BLPoint p1 = mTargetWorldMatrix.mapPoint( mTargetGridBBox.x + mTargetGridBBox.w, mTargetGridBBox.y                     );
        BLPoint p2 = mTargetWorldMatrix.mapPoint( mTargetGridBBox.x + mTargetGridBBox.w, mTargetGridBBox.y + mTargetGridBBox.h );
        BLPoint p3 = mTargetWorldMatrix.mapPoint( mTargetGridBBox.x                    , mTargetGridBBox.y + mTargetGridBBox.h );
        ::ULIS::FRectD worldBBox = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                                             , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                                             , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                                             , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );

        return worldBBox;
    }

    return mTargetGridBBox;
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
FOdysseyVectorTagInbetweener::UpdateMatrix()
{
    mTargetLocalMatrix.reset();
    mTargetLocalMatrix.translate( mTargetTranslationX, mTargetTranslationY );
    mTargetLocalMatrix.rotate( mTargetRotation * M_PI / 180.0f );
    mTargetLocalMatrix.scale( mTargetScalingX, mTargetScalingY );

    mTargetWorldMatrix = mOwner->GetWorldMatrix();
    mTargetWorldMatrix.transform( mTargetLocalMatrix );

    BLMatrix2D::invert( mTargetInverseWorldMatrix, mTargetWorldMatrix );

    for( uint32 inbetweenIndex = 0; inbetweenIndex < mInbetweenCount; inbetweenIndex++ )
    {
        InterpolateTransform( inbetweenIndex );
    }

    Invalidate( INVALIDATE_CELLS );
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
FInbetweenerGridFFD::ComputeBinomialCoefficients()
{
    uint32 numVertexX = mNumQuadX + 1;
    uint32 numVertexY = mNumQuadY + 1;

    for ( uint32 i = 0; i < numVertexX; i++ )
    {
        double coeffU = BinomialCoeff ( mNumQuadX, i );

        mUBinomialCoefficientBuffer[i] = coeffU;
    }

    for ( uint32 i = 0; i < numVertexY; i++ )
    {
        double coeffV = BinomialCoeff ( mNumQuadY, i );

        mVBinomialCoefficientBuffer[i] = coeffV;
    }
}

void
FOdysseyVectorTagInbetweener::InterpolateGeometry( uint32 iInbetweenIndex )
{
    ::ULIS::FRectD bbox = mOwner->GetBBox( false );

    for( FInbetweenerGridPoint& point : mGrid->GetPointBuffer() )
    {
        ::ULIS::FVec2D diff = ( point.mTargetPosition - point.mSourcePosition );
        ::ULIS::FVec2D step = diff * mChart.inbetweenBuffer[iInbetweenIndex].spacing;

        point.mMotionPosition = point.mSourcePosition + step;

        point.u = ( point.mMotionPosition.x - bbox.x ) / bbox.w;
        point.v = ( point.mMotionPosition.y - bbox.y ) / bbox.h;
    }

    // deform the path according to grid geometry
    mGrid->DeformPaths( mInterpolatedPathBuffer, iInbetweenIndex );
}

void
FOdysseyVectorTagInbetweener::UpdateAnimationCells()
{
    UpdateAnimationCells( mInbetweenCount );
}


void
FOdysseyVectorTagInbetweener::UpdateAnimationCells( uint32 iInbetweenCount )
{
    IOdysseyVectorAnimationCell* animationCell = mOwner->GetScene()->GetEngine()->GetAnimationCell();

    if( animationCell )
    {
        int32 animationCellIndex = animationCell->GetIndex();

        // Redraw impacted cells
        for( uint32 i = 0; ( i < iInbetweenCount ) && ( animationCell != nullptr ); i++ )
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
FOdysseyVectorTagInbetweener::InterpolateTransform( uint32 iInbetweenIndex )
{
    FInbetweenerInbetween* inbetween = &mChart.inbetweenBuffer[iInbetweenIndex];
    double translationX, translationY, rotation, scalingX, scalingY;
    double spacing = inbetween->spacing;

    translationX = mTargetTranslationX * spacing;
    translationY = mTargetTranslationY * spacing;
    rotation = mTargetRotation * spacing;
    scalingX = 1.0f + ( ( mTargetScalingX - 1.0f ) * spacing );
    scalingY = 1.0f + ( ( mTargetScalingY - 1.0f ) * spacing );

    inbetween->matrix.reset();
    inbetween->matrix.translate( translationX, translationY );
    inbetween->matrix.rotate( rotation );
    inbetween->matrix.scale( scalingX, scalingY );
}

void
FOdysseyVectorTagInbetweener::Interpolate()
{
    for( uint32 i = 0; i <= mInbetweenCount; i++ )
    {
        InterpolateGeometry( i );
        InterpolateTransform( i );
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

    // check the object is still displayed (it could have been removed but still in memory)
    if( mOwner->GetScene() )
    {
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
}

void
FOdysseyVectorTagInbetweener::DrawPathAt( FInterpolatedPath* iInterpolatedPath
                                        , ::ULIS::FVec2D* iPointPositionBuffer
                                        , const BLMatrix2D& iWorldMatrix
                                        , BLContext* iBLContext )
{
    for( FInterpolatedSegmentCubic& interpolatedCubicSegment : iInterpolatedPath->mInterpolatedSegmentCubicBuffer )
    {
        FInterpolatedPoint* interpolatedPoint[4] = { interpolatedCubicSegment.mInterpolatedVertex[0]
                                                   , interpolatedCubicSegment.mInterpolatedHandle[0]
                                                   , interpolatedCubicSegment.mInterpolatedHandle[1]
                                                   , interpolatedCubicSegment.mInterpolatedVertex[1] };
        BLPoint pt[4] = { iWorldMatrix.mapPoint( iPointPositionBuffer[interpolatedPoint[0]->mIndex].x
                                               , iPointPositionBuffer[interpolatedPoint[0]->mIndex].y )
                        , iWorldMatrix.mapPoint( iPointPositionBuffer[interpolatedPoint[1]->mIndex].x
                                               , iPointPositionBuffer[interpolatedPoint[1]->mIndex].y )
                        , iWorldMatrix.mapPoint( iPointPositionBuffer[interpolatedPoint[2]->mIndex].x
                                               , iPointPositionBuffer[interpolatedPoint[2]->mIndex].y )
                        , iWorldMatrix.mapPoint( iPointPositionBuffer[interpolatedPoint[3]->mIndex].x
                                               , iPointPositionBuffer[interpolatedPoint[3]->mIndex].y ) };
        BLPath path;

        path.moveTo ( pt[0].x, pt[0].y );
        path.cubicTo( pt[1].x, pt[1].y
                    , pt[2].x, pt[2].y
                    , pt[3].x, pt[3].y );

        iBLContext->strokePath( path );
    }
}

void
FOdysseyVectorTagInbetweener::DrawPathsInbetween( uint32 iInbetweenIndex
                                                , BLContext* iBLContext )
{
    BLMatrix2D worldMatrix = mOwner->GetWorldMatrix();

    worldMatrix.transform( mChart.inbetweenBuffer[iInbetweenIndex].matrix );

    for( FInterpolatedPath& interpolatedPath : mInterpolatedPathBuffer )
    {
        uint32 pointCount = interpolatedPath.mInterpolatedPointBuffer.size();
        ::ULIS::FVec2D* pointPositionBuffer = &interpolatedPath.mInterpolatedPointPositionBuffer[pointCount * iInbetweenIndex];

        DrawPathAt( &interpolatedPath
                  , pointPositionBuffer
                  , worldMatrix
                  , iBLContext );
    }
}

void
FOdysseyVectorTagInbetweener::DrawPathsTarget( BLContext* iBLContext )
{
    BLMatrix2D worldMatrix = mOwner->GetWorldMatrix();

    worldMatrix.transform( mTargetLocalMatrix );

    for( FInterpolatedPath& interpolatedPath : mInterpolatedPathBuffer )
    {
        uint32 pointCount = interpolatedPath.mInterpolatedPointBuffer.size();
        ::ULIS::FVec2D* pointPositionBuffer = &interpolatedPath.mInterpolatedPointPositionBuffer[pointCount * mInbetweenCount];

        DrawPathAt( &interpolatedPath
                  , pointPositionBuffer
                  , worldMatrix
                  , iBLContext );
    }
}

FInbetweenerGrid*
FOdysseyVectorTagInbetweener::GetGrid()
{
    return mGrid;
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
                InterpolateGeometry( inbetweenIndex );
                InterpolateTransform( inbetweenIndex );
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

double
FOdysseyVectorTagInbetweener::GetTargetTranslationX()
{
    return mTargetTranslationX;
}

double
FOdysseyVectorTagInbetweener::GetTargetTranslationY()
{
    return mTargetTranslationY;
}

double
FOdysseyVectorTagInbetweener::GetTargetRotation()
{
    return mTargetRotation;
}

double
FOdysseyVectorTagInbetweener::GetTargetScalingX()
{
    return mTargetScalingX;
}

double
FOdysseyVectorTagInbetweener::GetTargetScalingY()
{
    return mTargetScalingY;
}

void
FOdysseyVectorTagInbetweener::AllocBuffers()
{
    for( FInterpolatedPath& interpolatedPath : mInterpolatedPathBuffer )
    {
        uint32 pointCount = interpolatedPath.mInterpolatedPointBuffer.size();

        // Note: we add +1 to store the target too
        interpolatedPath.mInterpolatedPointPositionBuffer.resize( ( mInbetweenCount + 1 ) * pointCount );
    }
}

void
FOdysseyVectorTagInbetweener::SetInbetweenCount( uint32 iInbetweenCount )
{
    uint32 maxInbetweenCount = ::ULIS::FMath::Max( iInbetweenCount, mInbetweenCount );

    mInbetweenCount = iInbetweenCount;

    Invalidate( INVALIDATE_SPACING
              | INVALIDATE_BUFFERS
              | INVALIDATE_CELLS );

    ResetChart();

    UpdateAnimationCells( maxInbetweenCount );
}

eInbetweenerGridType
FOdysseyVectorTagInbetweener::GetGridType()
{
    return mGridType;
}

std::vector<FInbetweenerGridQuad>&
FOdysseyVectorTagInbetweener::GetGridQuadBuffer()
{
    return mGrid->GetQuadBuffer();
}

std::vector<FInbetweenerGridPoint>&
FOdysseyVectorTagInbetweener::GetGridPointBuffer()
{
    return mGrid->GetPointBuffer();
}

uint32
FOdysseyVectorTagInbetweener::GetGridNumQuadX()
{
    return mGrid->GetNumQuadX();
}

uint32
FOdysseyVectorTagInbetweener::GetGridNumQuadY()
{
    return mGrid->GetNumQuadY();
}

void
FOdysseyVectorTagInbetweener::SetGridType( eInbetweenerGridType iGridType )
{
    if( mGrid )
    {
        delete mGrid;

        mGrid = nullptr;
    }

    mGridType = iGridType;

    switch( iGridType )
    {
        case eInbetweenerGridType::ARAP :
            mGrid = new FInbetweenerGridARAP( this );
        break;

        default:
            mGrid = new FInbetweenerGridFFD( this );
        break;
    }

    Invalidate( INVALIDATE_MAP
              | INVALIDATE_SPACING
              | INVALIDATE_BUFFERS
              | INVALIDATE_CELLS );

    mGrid->Make( mGrid->GetNumQuadX(), mGrid->GetNumQuadY() );
}

void
FOdysseyVectorTagInbetweener::SetGridNumQuad( uint32 iNumQuadX, uint32 iNumQuadY )
{
    Invalidate( INVALIDATE_SPACING
              | INVALIDATE_CELLS );

    mGrid->Make( iNumQuadX, iNumQuadY );
}

void
FOdysseyVectorTagInbetweener::SetGridNumQuadX( uint32 iNumQuadX )
{
    Invalidate( INVALIDATE_SPACING
              | INVALIDATE_CELLS );

    mGrid->Make( iNumQuadX, mGrid->GetNumQuadY() );
}

void
FOdysseyVectorTagInbetweener::SetGridNumQuadY( uint32 iNumQuadY )
{
    Invalidate( INVALIDATE_SPACING
              | INVALIDATE_CELLS );

    mGrid->Make( mGrid->GetNumQuadX(), iNumQuadY );
}

void
FOdysseyVectorTagInbetweener::Commit()
{
    IOdysseyVectorAnimationCell* animationCell = mOwner->GetScene()->GetEngine()->GetAnimationCell();
    int32 animationCellIndex = animationCell->GetIndex();

    for( uint32 inbetweenIndex = 0; inbetweenIndex < mInbetweenCount; inbetweenIndex++ )
    {
        IOdysseyVectorAnimationCell* inbetweenAnimationCell = animationCell->GetCellByIndex( animationCellIndex + inbetweenIndex + 1 );

        if( inbetweenAnimationCell )
        {
            // change vertices coords before copying the object
            std::function<void(FOdysseyVectorObject*)> preProcess = [ inbetweenIndex ]( FOdysseyVectorObject* vectorObject )
            {
                FOdysseyVectorTag* tag = vectorObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

                if( tag )
                {
                    FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);
                    FInbetweenerInbetween* inbetween = &inbetweenerTag->mChart.inbetweenBuffer[inbetweenIndex];

                    for( FInterpolatedPath& interpolatedPath : inbetweenerTag->mInterpolatedPathBuffer )
                    {
                        uint32 pointCount = interpolatedPath.mInterpolatedPointBuffer.size();
                        uint32 skippedOffset = ( inbetweenIndex * pointCount );

                        for( uint32 i = 0; i < interpolatedPath.mInterpolatedPointBuffer.size(); i++ )
                        {
                            FInterpolatedPoint* interpolatedPoint = &interpolatedPath.mInterpolatedPointBuffer[i];
                            ::ULIS::FVec2D* commitPosition = &interpolatedPath.mInterpolatedPointPositionBuffer[skippedOffset + i];
                            ::ULIS::FVec2D swapPosition = interpolatedPoint->mOriginalPoint->GetCoords();
                            BLPoint transformedPosition;

                            transformedPosition = inbetween->matrix.mapPoint( commitPosition->x
                                                                            , commitPosition->y );

                            interpolatedPoint->mOriginalPoint->Set( transformedPosition.x
                                                                  , transformedPosition.y );

                            *commitPosition = swapPosition;
                        }
                    }

                    //tag->GetOwner()->RemoveTag( tag );
                }
            };

            std::function<void(FOdysseyVectorObject*,FOdysseyVectorObject*)> postProcess = [ inbetweenIndex
                                                                                           , preProcess ]( FOdysseyVectorObject* sourceObject
                                                                                                         , FOdysseyVectorObject* objectCopy )
            {
                FOdysseyVectorTag* tag = sourceObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

                if( tag )
                {
                    FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);
                    FInbetweenerInbetween* inbetween = &inbetweenerTag->mChart.inbetweenBuffer[inbetweenIndex];

                    inbetween->matrix.reset();
                }

                // revert vertices coords after having copied the object. It's actually the same thing.
                preProcess( sourceObject );
            };

            FOdysseyVectorGroupPaint* inbetweenScene = inbetweenAnimationCell->GetEngine()->GetScene();

            FOdysseyVectorObject* copiedObject = mOwner->Copy( preProcess, postProcess );

            inbetweenScene->AppendChild( copiedObject );

            inbetweenScene->UpdateMatrix();
            inbetweenScene->Update( 0 );
        }
    }

    mOwner->RecursiveRemoveTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

    UpdateAnimationCells();
}
