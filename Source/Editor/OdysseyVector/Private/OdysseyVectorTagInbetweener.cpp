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
#include "InbetweenerTag/InbetweenerPoint.h"
#include "InbetweenerTag/InbetweenerQuad.h"
#include "InbetweenerTag/InbetweenerGrid.h"
#include "InbetweenerTag/InbetweenerGridFFD.h"
#include "InbetweenerTag/InbetweenerGridARAP.h"
#include "InbetweenerTag/InterpolatedSegmentCubic.h"
#include "InbetweenerTag/InterpolatedPoint.h"
#include "InbetweenerTag/InterpolatedPath.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

void
FOdysseyVectorTagInbetweener::UpdateBBox( ::ULIS::FRectD& iBBox
                                        , eInbetweenerPointPositionType iPositionType )
{
    double xmin = DBL_MAX, ymin = DBL_MAX, xmax = -DBL_MAX, ymax = -DBL_MAX;
    bool hasBBox = false;

    for( FInbetweenerPoint& gridPoint : mGrid->GetPointBuffer() )
    {
        const ::ULIS::FVec2D& position = gridPoint.GetPosition( iPositionType );

        hasBBox = true;

        if ( position.x < xmin ) xmin = position.x;
        if ( position.y < ymin ) ymin = position.y;
        if ( position.x > xmax ) xmax = position.x;
        if ( position.y > ymax ) ymax = position.y;
    }

    iBBox = ( hasBBox ) ? ::ULIS::FRectD::FromMinMax( xmin - 0.001f, ymin - 0.001f
                                                    , xmax + 0.001f, ymax + 0.001f ) 
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
          FOdysseyVectorTag* objectInbetweenerTag = object->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

          if( ( objectInbetweenerTag == nullptr ) || ( objectInbetweenerTag == this ) )
          {
              if( object->GetClass() == FOdysseyVectorPath::StaticClass() )
              {
                  FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

                  mInterpolatedPathBuffer.emplace_back( path, mInbetweenCount );

                  return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
              }
          }
          else
          {
              return FOdysseyVectorEngine::TRAVERSE_OBJECT_IGNORE_CHILDREN;
          }

          return 0;
      } );

    mGrid->MapInterpolatedPaths( mInterpolatedPathBuffer
                               , mOwner->GetInverseWorldMatrix() );
}

FOdysseyVectorTagInbetweener::~FOdysseyVectorTagInbetweener()
{
    delete mGrid;
}

BLMatrix2D&
FOdysseyVectorTagInbetweener::GetTargetLocalMatrix()
{
    return mTargetLocalMatrix;
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
    , mInterpolationType( eInbetweenerInterpolationType::ARAP )
    , mInbetweenCount( iInbetweenCount )
    , mInvalidationFlags( INVALIDATE_MAP
                        | INVALIDATE_BUFFERS
                        | INVALIDATE_SOURCEBBOX
                        | INVALIDATE_TARGETBBOX
                        | INVALIDATE_TRAJECTORIES
                        | INVALIDATE_SPACING
                        | INVALIDATE_CELLS )
    , mTargetTranslationX( 0.0f )
    , mTargetTranslationY( 0.0f )
    , mTargetScalingX    ( 1.0f )
    , mTargetScalingY    ( 1.0f )
    , mTargetRotation    ( 0.0f )
{
    ResetChart();
    // Note: Matrix needs chart to be allocated first.
    UpdateMatrix();

    mSourceBBox = iOwnerObject->GetBBox( false );

    // Note: Grid building needs the bbox to be set.
    SetGridType( mGridType );

    //mGrid->Make( iNumQuadX, iNumQuadY, iOwnerObject->GetBBox( false ), this );

    //Map();

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

void FOdysseyVectorTagInbetweener::Update( uint32 iUpdateFlags
                                         , uint64 iOwnerInvalidationFlags )
{
    if( ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_HIERARCHY      )
     || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_TOPOLOGY       )
     || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_TAGS     )
     || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_TOPOLOGY ) )
    {
        mInvalidationFlags |= INVALIDATE_MAP;
    }

    if( ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_SHAPE    )
     || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_TAGS     )
     || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_MATRIX   )

     || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_SHAPE    )
     || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_TAGS     )
     || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_MATRIX   ) )
    {
        mInvalidationFlags |= ( INVALIDATE_SPACING );
    }

    if( mInvalidationFlags & INVALIDATE_SOURCEBBOX )
    {
        UpdateBBox( mSourceBBox, eInbetweenerPointPositionType::SourcePosition );
    }

    if( mInvalidationFlags & INVALIDATE_TARGETBBOX )
    {
        UpdateBBox( mTargetBBox, eInbetweenerPointPositionType::TargetPosition );
    }

    mGrid->Update( iUpdateFlags, mInvalidationFlags );

    if( mInvalidationFlags & INVALIDATE_MAP )
    {
        Map();

        mInvalidationFlags |= INVALIDATE_BUFFERS;
    }

    if( mInvalidationFlags & INVALIDATE_BUFFERS )
    {
        AllocBuffers();
    }

    if( mInvalidationFlags & INVALIDATE_SPACING )
    {
        Interpolate();
    }

    if( mInvalidationFlags & INVALIDATE_TRAJECTORIES )
    {
        for( FInbetweenerTrajectory* trajectory : mGrid->GetTrajectoryList() )
        {
            trajectory->Update();
        }
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
FOdysseyVectorTagInbetweener::GetSourceBBox( bool iWorld )
{
    if ( iWorld == true )
    {
        BLMatrix2D worldMatrix = mOwner->GetWorldMatrix();
        BLPoint p0 = worldMatrix.mapPoint( mSourceBBox.x                , mSourceBBox.y                 );
        BLPoint p1 = worldMatrix.mapPoint( mSourceBBox.x + mSourceBBox.w, mSourceBBox.y                 );
        BLPoint p2 = worldMatrix.mapPoint( mSourceBBox.x + mSourceBBox.w, mSourceBBox.y + mSourceBBox.h );
        BLPoint p3 = worldMatrix.mapPoint( mSourceBBox.x                , mSourceBBox.y + mSourceBBox.h );
        ::ULIS::FRectD worldBBox = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                                             , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                                             , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                                             , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );

        return worldBBox;
    }

    return mSourceBBox;
}

::ULIS::FRectD
FOdysseyVectorTagInbetweener::GetTargetBBox( bool iWorld )
{
    if ( iWorld == true )
    {
        BLPoint p0 = mTargetWorldMatrix.mapPoint( mTargetBBox.x                , mTargetBBox.y                 );
        BLPoint p1 = mTargetWorldMatrix.mapPoint( mTargetBBox.x + mTargetBBox.w, mTargetBBox.y                 );
        BLPoint p2 = mTargetWorldMatrix.mapPoint( mTargetBBox.x + mTargetBBox.w, mTargetBBox.y + mTargetBBox.h );
        BLPoint p3 = mTargetWorldMatrix.mapPoint( mTargetBBox.x                , mTargetBBox.y + mTargetBBox.h );
        ::ULIS::FRectD worldBBox = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                                             , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                                             , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                                             , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );

        return worldBBox;
    }

    return mTargetBBox;
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

    Invalidate( INVALIDATE_CELLS | INVALIDATE_TRAJECTORIES );
}

void
FOdysseyVectorTagInbetweener::InterpolateGeometry( uint32 iInbetweenIndex )
{
    ::ULIS::FRectD bbox = mOwner->GetBBox( false );

    if( mInterpolationType == eInbetweenerInterpolationType::Linear )
    {
        for( FInbetweenerPoint& point : mGrid->GetPointBuffer() )
        {
            ::ULIS::FVec2D diff = ( point.mTargetPosition - point.mSourcePosition );
            double t = mChart.inbetweenBuffer[iInbetweenIndex].spacing;
            ::ULIS::FVec2D step = diff * t;

            point.mInterpPosition = point.mSourcePosition + step;
        }
    }

    if( mInterpolationType == eInbetweenerInterpolationType::ARAP )
    {
        double t = mChart.inbetweenBuffer[iInbetweenIndex].spacing;

        mGrid->ComputeARAPInterpolation( //t
                                       //, t
                                       // , const FInbetweenerPoint::Affine &globalRigidTransform
                                         &mChart.inbetweenBuffer[iInbetweenIndex]
                                       , false );
    }

    if( mGridType == eInbetweenerGridType::FFD )
    {
        for( FInbetweenerPoint& point : mGrid->GetPointBuffer() )
        {
            point.SetU( ( point.mInterpPosition.x - bbox.x ) / bbox.w );
            point.SetV( ( point.mInterpPosition.y - bbox.y ) / bbox.h );
        }
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

    BLMatrix2D::invert( inbetween->inverseMatrix, inbetween->matrix );
}

void
FOdysseyVectorTagInbetweener::Interpolate()
{
    if( mInterpolationType == eInbetweenerInterpolationType::ARAP )
    {
        if( mGrid->PrecomputeARAPInterpolation() == false )
        {
            mInterpolationType = eInbetweenerInterpolationType::Linear;
        }
    }

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
FOdysseyVectorTagInbetweener::DrawMotionGrid( uint32 iInbetweenIndex
                                            , BLContext* iBLContext
                                            , const ::ULIS::FRectD& iInvalidationArea
                                            , double iAncestorsOpacity
                                            , uint64 iDrawingFlags )
{
    BLMatrix2D worldMatrix = mOwner->GetWorldMatrix();

    worldMatrix.transform( mChart.inbetweenBuffer[iInbetweenIndex].matrix );

    iBLContext->save();
    iBLContext->resetMatrix();

    iBLContext->setStrokeStyle( BLRgba32( 255, 0, 255, 255 ) );
    iBLContext->setStrokeWidth( 1.0f );

    for( FInbetweenerQuad& quad : mGrid->GetQuadBuffer() )
    {
        FInbetweenerPoint** gridPoint = quad.GetPoints();
        BLPoint pt[4] = { worldMatrix.mapPoint( gridPoint[0]->GetPosition( eInbetweenerPointPositionType::InterpPosition ).x
                                              , gridPoint[0]->GetPosition( eInbetweenerPointPositionType::InterpPosition ).y )
                        , worldMatrix.mapPoint( gridPoint[1]->GetPosition( eInbetweenerPointPositionType::InterpPosition ).x
                                              , gridPoint[1]->GetPosition( eInbetweenerPointPositionType::InterpPosition ).y )
                        , worldMatrix.mapPoint( gridPoint[2]->GetPosition( eInbetweenerPointPositionType::InterpPosition ).x
                                              , gridPoint[2]->GetPosition( eInbetweenerPointPositionType::InterpPosition ).y )
                        , worldMatrix.mapPoint( gridPoint[3]->GetPosition( eInbetweenerPointPositionType::InterpPosition ).x
                                              , gridPoint[3]->GetPosition( eInbetweenerPointPositionType::InterpPosition ).y ) };

        iBLContext->strokeLine( pt[0], pt[1] );
        iBLContext->strokeLine( pt[1], pt[2] );
        iBLContext->strokeLine( pt[2], pt[3] );
        iBLContext->strokeLine( pt[3], pt[0] );
    }

    iBLContext->restore();
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

////////////////////////////////// TEMP /////////////////////////
/*
        if( mInterpolationType == eInbetweenerInterpolationType::ARAP )
        {
            uint32 tagCellIndex = tagCell->GetIndex();
            uint32 displayedCellIndex = displayedCell->GetIndex();
            uint32 iInbetweenIndex = ( displayedCellIndex - tagCellIndex - 1 );
            double t = mChart.inbetweenBuffer[iInbetweenIndex].spacing;

            mGrid->ComputeARAPInterpolation( t
                                           , t
                                           // , const FInbetweenerPoint::Affine &globalRigidTransform
                                           , false );

            DrawMotionGrid( iInbetweenIndex
                          , iBLContext
                          , iInvalidationArea
                          , iAncestorsOpacity
                          , iDrawingFlags );
        }
*/
////////////////////////////////////////////////////////////////
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

std::vector<FInterpolatedPath>&
FOdysseyVectorTagInbetweener::GetInterpolatedPathBuffer()
{
    return mInterpolatedPathBuffer;
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

std::vector<FInbetweenerQuad>&
FOdysseyVectorTagInbetweener::GetGridQuadBuffer()
{
    return mGrid->GetQuadBuffer();
}

std::vector<FInbetweenerPoint>&
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
    uint32 numQuadX = ( mGrid ) ? mGrid->GetNumQuadX() : 4;
    uint32 numQuadY = ( mGrid ) ? mGrid->GetNumQuadY() : 4;

    if( mGrid )
    {
        delete mGrid;

        mGrid = nullptr;
    }

    mGridType = iGridType;

    switch( iGridType )
    {
        case eInbetweenerGridType::ARAP :
            mGrid = new FInbetweenerGridARAP( this, numQuadX, numQuadY );
        break;

        default:
            mGrid = new FInbetweenerGridFFD( this, numQuadX, numQuadY );
        break;
    }

    Invalidate( INVALIDATE_MAP
              | INVALIDATE_SPACING
              | INVALIDATE_BUFFERS
              | INVALIDATE_CELLS );

    mGrid->Make( mGrid->GetNumQuadX(), mGrid->GetNumQuadY(), mSourceBBox );
}

void
FOdysseyVectorTagInbetweener::SetGridNumQuad( uint32 iNumQuadX
                                            , uint32 iNumQuadY
                                            , const std::vector<::ULIS::FVec2D>& iSourcePositionBuffer
                                            , const std::vector<::ULIS::FVec2D>& iTargetPositionBuffer )
{
    Invalidate( INVALIDATE_MAP
              | INVALIDATE_SPACING
              | INVALIDATE_CELLS );

    mGrid->Make( iNumQuadX
               , iNumQuadY
               , mSourceBBox
               , iSourcePositionBuffer
               , iTargetPositionBuffer );
}

void
FOdysseyVectorTagInbetweener::SetGridNumQuad( uint32 iNumQuadX, uint32 iNumQuadY )
{
    Invalidate( INVALIDATE_MAP
              | INVALIDATE_SPACING
              | INVALIDATE_CELLS );

    mGrid->Make( iNumQuadX, iNumQuadY, mSourceBBox );
}

void
FOdysseyVectorTagInbetweener::SetGridNumQuadX( uint32 iNumQuadX )
{
    Invalidate( INVALIDATE_MAP
              | INVALIDATE_SPACING
              | INVALIDATE_CELLS );

    mGrid->Make( iNumQuadX, mGrid->GetNumQuadY(), mSourceBBox );
}

void
FOdysseyVectorTagInbetweener::SetGridNumQuadY( uint32 iNumQuadY )
{
    Invalidate( INVALIDATE_MAP
              | INVALIDATE_SPACING
              | INVALIDATE_CELLS );

    mGrid->Make( mGrid->GetNumQuadX(), iNumQuadY, mSourceBBox );
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

eInbetweenerInterpolationType
FOdysseyVectorTagInbetweener::GetInterpolationType()
{
    return mInterpolationType;
}

void
FOdysseyVectorTagInbetweener::SetInterpolationType( eInbetweenerInterpolationType iInterpolationType )
{
    mInterpolationType = iInterpolationType;

    Invalidate( INVALIDATE_SPACING
              | INVALIDATE_CELLS );
}
