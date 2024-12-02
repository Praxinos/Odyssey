// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

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
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"
#include "InbetweenerTag/InbetweenerPoint.h"
#include "InbetweenerTag/InbetweenerQuad.h"
#include "InbetweenerTag/InbetweenerGrid.h"
#include "InbetweenerTag/InbetweenerGridFFD.h"
#include "InbetweenerTag/InbetweenerGridARAP.h"
#include "InbetweenerTag/InbetweenerBreakdown.h"
#include "InbetweenerTag/InbetweenerRoute.h"
#include "InbetweenerTag/InterpolatedSegment.h"
#include "InbetweenerTag/InterpolatedPoint.h"
#include "InbetweenerTag/InterpolatedPath.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

FOdysseyVectorTagInbetweener::~FOdysseyVectorTagInbetweener()
{
    mRouteList.remove_if( []( FInbetweenerRoute* route )
                              {
                                  delete route;

                                  return true;
                              } );

    mBreakdownList.remove_if( []( FInbetweenerBreakdown* breakdown )
                              {
                                  delete breakdown;

                                  return true;
                              } );
}

FOdysseyVectorTagInbetweener::FOdysseyVectorTagInbetweener( FOdysseyVectorObject* iOwnerObject
                                                          , uint32 iNumQuadX
                                                          , uint32 iNumQuadY
                                                          , eInbetweenerGridType iGridType )
    : FOdysseyVectorTag( iOwnerObject )
    //, mSharedEnv ( iSharedEnv )
    // Remember the scene because when removing the tag, we'll need to be able to redraw inbetween cells but
    // as the owner object won't be linked to the screne anymore, the scene won't be retrievable.
    // Note: hence the Owner MUST be in the scene's hierarchy
    , mScene ( iOwnerObject->GetScene() )
    //, mGrid( nullptr )
    , mGridType( iGridType )
    , mGridNumQuadX( iNumQuadX )
    , mGridNumQuadY( iNumQuadY )
    , mInterpolationType( eInbetweenerInterpolationType::ARAP )
    , mInvalidationFlags( INVALIDATE_MAP
                        | INVALIDATE_ROUTES
                        | INVALIDATE_SPACING
                        | INVALIDATE_CELLS )
    , bMapAsPolyline( true )
    , bWithThickness( true )
    , bContiguous( true )
    , bARAPPrecomputeSucceded( false )
    , mInterpolationDirection( eInbetweenerInterpolationDirection::Forward )
    , bSquare ( true )
    , mInbetweenColor ( INBETWEEN_DEFAULT_RED_UINT8
                      , INBETWEEN_DEFAULT_GREEN_UINT8
                      , INBETWEEN_DEFAULT_BLUE_UINT8
                      , INBETWEEN_DEFAULT_ALPHA_UINT8 )
    , mChartColor( CHART_DEFAULT_RED_UINT8
                 , CHART_DEFAULT_GREEN_UINT8
                 , CHART_DEFAULT_BLUE_UINT8
                 , CHART_DEFAULT_ALPHA_UINT8 )
    , mGridColor( GRID_DEFAULT_RED_UINT8
                , GRID_DEFAULT_GREEN_UINT8
                , GRID_DEFAULT_BLUE_UINT8
                , GRID_DEFAULT_ALPHA_UINT8 )
    , mTrajectoryColor( TRAJECTORY_DEFAULT_RED_UINT8
                      , TRAJECTORY_DEFAULT_GREEN_UINT8
                      , TRAJECTORY_DEFAULT_BLUE_UINT8
                      , TRAJECTORY_DEFAULT_ALPHA_UINT8 )
    , bConstantWidth ( false )
{
    FInbetweenerBreakdown* defaultBreakdown = new FInbetweenerBreakdown( this );

    // the default breakdown (has range 0 <-> 1 )
    mBreakdownList.emplace_back( defaultBreakdown );

    // force initial dispatching of the drawings
    defaultBreakdown->SetTargetDrawingIndex( 1 );

    // Note: Matrix needs chart to be allocated first.
    UpdateMatrix();
}

FOdysseyVectorTagInbetweener*
FOdysseyVectorTagInbetweener::Copy( FOdysseyVectorObject* iDestOwnerObject )
{
    FOdysseyVectorTagInbetweener* newTag = new FOdysseyVectorTagInbetweener ( iDestOwnerObject
                                                                            , mGridNumQuadX
                                                                            , mGridNumQuadY
                                                                            , mGridType );
    FInbetweenerBreakdown* newTagDefaultBreakdown = newTag->GetBreakdownList().front();

    newTag->mInterpolationType = mInterpolationType;
    newTag->mInbetweenColor = mInbetweenColor;
    newTag->bMapAsPolyline = bMapAsPolyline;
    newTag->bWithThickness = bWithThickness;
    newTag->bContiguous = bContiguous;
    newTag->mInterpolationDirection = mInterpolationDirection;
    newTag->bSquare = bSquare;
    newTag->mChartColor = mChartColor;
    newTag->mGridColor = mGridColor;

    newTagDefaultBreakdown->SetTargetDrawingIndex( GetLength() - 1 );

    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        uint32 breakdownTargetDrawingIndex = breakdown->GetTargetDrawingIndex();

        if( ( breakdownTargetDrawingIndex > newTagDefaultBreakdown->GetSourceDrawingIndex() )
         && ( breakdownTargetDrawingIndex < newTagDefaultBreakdown->GetTargetDrawingIndex() ) )
        {
            newTag->AddBreakdown( breakdown->GetTargetDrawingIndex(), false, false );
        }
    }
/*
    for( FInbetweenerRoute* route : mRouteList )
    {
        newTag->AddRoute( breakdown->GetTargetDrawingIndex(), false );
    }
*/
    return newTag;
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

                  // do not call Update(), call UpdateShape().
                  //  Update would call tag->Update() (this fuinction)
                  // resulting in a inifinite loop.
                  path->UpdateShape( // force minimum segment subdivision (important for being able to deform
                                     // straight segments
                                     FOdysseyVectorObject::UPDATE_NEEDPOLYLINE
                                     // passing FORCe will force the update of all segments. We cannot call InvalidateallSegments()
                                     // because this is an Update process and it should not call an Invalidate process.
                                   | FOdysseyVectorObject::UPDATE_FORCE );

                  mInterpolatedPathBuffer.emplace_back( path
                                                      , GetLength()
                                                      , bMapAsPolyline );

                  return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
              }
          }
          else
          {
              return FOdysseyVectorEngine::TRAVERSE_OBJECT_IGNORE_CHILDREN;
          }

          return 0;
      } );

    /* Map on first grid */
    mBreakdownList.front()->GetGrid()->MapInterpolatedPaths( mInterpolatedPathBuffer );
}

const FColor&
FOdysseyVectorTagInbetweener::GetChartColor()
{
    return mChartColor;
}

void
FOdysseyVectorTagInbetweener::SetChartColor( const FColor& iChartColor )
{
    mChartColor = iChartColor;
}

const FColor&
FOdysseyVectorTagInbetweener::GetGridColor()
{
    return mGridColor;
}

void
FOdysseyVectorTagInbetweener::SetGridColor( const FColor& iGridColor )
{
    mGridColor = iGridColor;
}

std::list<FInbetweenerRoute*>&
FOdysseyVectorTagInbetweener::GetRouteList()
{
    return mRouteList;
}

FInbetweenerDrawing*
FOdysseyVectorTagInbetweener::GetDrawing( uint32 iIndex )
{
    return &mDrawingBuffer[iIndex];
}

eInbetweenerInterpolationDirection
FOdysseyVectorTagInbetweener::GetInterpolationDirection()
{
    return mInterpolationDirection;
}

void
FOdysseyVectorTagInbetweener::InvertInterpolationDirection()
{
    if( mInterpolationDirection == eInbetweenerInterpolationDirection::Forward )
    {
        SetInterpolationDirection( eInbetweenerInterpolationDirection::Backward );

        return;
    }

    if( mInterpolationDirection == eInbetweenerInterpolationDirection::Backward )
    {
        SetInterpolationDirection( eInbetweenerInterpolationDirection::Forward );

        return;
    }
}

void
FOdysseyVectorTagInbetweener::SetInterpolationDirection( eInbetweenerInterpolationDirection iDirection )
{
    // Invalidate current cells
    RedrawCells();

    mInterpolationDirection = iDirection;

    Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_CELLS );
}

bool
FOdysseyVectorTagInbetweener::IsSquare()
{
    return bSquare;
}

void
FOdysseyVectorTagInbetweener::AddRoute( FInbetweenerRoute* iRoute )
{
    mRouteList.push_back( iRoute );

    iRoute->SetInbetweenerTag( this );

    Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_ROUTES
              | FOdysseyVectorTagInbetweener::INVALIDATE_SPACING
              | FOdysseyVectorTagInbetweener::INVALIDATE_CELLS );
}

void
FOdysseyVectorTagInbetweener::RemoveRoute( FInbetweenerRoute* iRoute )
{
    mRouteList.remove( iRoute );

    iRoute->SetInbetweenerTag( nullptr );

    Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING
              | FOdysseyVectorTagInbetweener::INVALIDATE_CELLS );
}

void
FOdysseyVectorTagInbetweener::RemoveAllRoutes()
{
    mRouteList.clear();

    Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING
              | FOdysseyVectorTagInbetweener::INVALIDATE_CELLS );
}

FInbetweenerRoute*
FOdysseyVectorTagInbetweener::AddRoute( const ::ULIS::FVec2D& iLocalCoords, bool iFit )
{
    FInbetweenerGrid* referenceGrid = mBreakdownList.front()->GetGrid();
    int quadIndex = referenceGrid->GetQuadIndex( iLocalCoords );

    if( quadIndex >= 0 )
    {
        FInbetweenerQuad* quad = &referenceGrid->GetQuadBuffer()[quadIndex];
        FInbetweenerPoint** quadPoint = quad->GetPoints();
        ::ULIS::FVec2D p0Coords = quadPoint[0]->GetSourcePosition();
        ::ULIS::FVec2D p1Coords = quadPoint[1]->GetSourcePosition();
        ::ULIS::FVec2D p2Coords = quadPoint[2]->GetSourcePosition();
        ::ULIS::FVec2D p3Coords = quadPoint[3]->GetSourcePosition();
        double difX = p1Coords.x - p0Coords.x;
        double difY = p2Coords.y - p1Coords.y;
        double quadU = difX ? ( iLocalCoords.x - p0Coords.x ) / difX : 0.0f;
        double quadV = difY ? ( iLocalCoords.y - p0Coords.y ) / difY : 0.0f;
        FInbetweenerRoute* route = new FInbetweenerRoute( this
                                                        , quadIndex
                                                        , quadU
                                                        , quadV );

        AddRoute( route );

        if( iFit )
        {
            //route->Fit( 0 );
        }

        return route;
    }

    return nullptr;
}

bool
FOdysseyVectorTagInbetweener::GetWithThickness()
{
    return bWithThickness;
}

void
FOdysseyVectorTagInbetweener::SetWithThickness( bool iWithThickness )
{
    bWithThickness = iWithThickness;

    Invalidate( INVALIDATE_CELLS );
}

bool
FOdysseyVectorTagInbetweener::GetMapAsPolyline()
{
    return bMapAsPolyline;
}

void
FOdysseyVectorTagInbetweener::SetMapAsPolyline( bool iMapAsPolyline )
{
    bMapAsPolyline = iMapAsPolyline;

    Invalidate( INVALIDATE_MAP | INVALIDATE_CELLS );
}

const FColor&
FOdysseyVectorTagInbetweener::GetInbetweenColor()
{
    return mInbetweenColor;
}

void
FOdysseyVectorTagInbetweener::SetInbetweenColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA )
{
    mInbetweenColor.R = iR;
    mInbetweenColor.G = iG;
    mInbetweenColor.B = iB;
    mInbetweenColor.A = iA;
}

void
FOdysseyVectorTagInbetweener::SetInbetweenColor( const FColor& iColor )
{
    mInbetweenColor = iColor;
}

const FColor&
FOdysseyVectorTagInbetweener::GetTrajectoryColor()
{
    return mTrajectoryColor;
}

void
FOdysseyVectorTagInbetweener::SetTrajectoryColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA )
{
    mTrajectoryColor.R = iR;
    mTrajectoryColor.G = iG;
    mTrajectoryColor.B = iB;
    mTrajectoryColor.A = iA;
}

void
FOdysseyVectorTagInbetweener::SetTrajectoryColor( const FColor& iColor )
{
    mTrajectoryColor = iColor;
}

FInbetweenerBreakdown*
FOdysseyVectorTagInbetweener::AddBreakdown( uint32 iDrawingIndex
                                          , bool   iCopyGeometry
                                          , bool   iFitNewTrajectories )
{
    return AddBreakdown( nullptr, iDrawingIndex, iCopyGeometry, iFitNewTrajectories );
}

// Removes all breakdowns but the default one
void
FOdysseyVectorTagInbetweener::ResetLayout( bool iFreeMemNow, FInbetweenerBreakdown* iNewDefaultBreakdown )
{
    //FInbetweenerBreakdown* newDefaultBreakdown = new FInbetweenerBreakdown( this );
    uint32 currentTargetDrawingIndex = mBreakdownList.back()->GetTargetDrawingIndex();

    std::vector<::ULIS::FVec2D> sourceGeometry;
    std::vector<::ULIS::FVec2D> targetGeometry;

    mBreakdownList.front()->GetGrid()->GetGeometry( sourceGeometry, eInbetweenerPointPositionType::SourcePosition );
    mBreakdownList.back() ->GetGrid()->GetGeometry( targetGeometry, eInbetweenerPointPositionType::TargetPosition );

    mBreakdownList.remove_if( [ this
                              , iNewDefaultBreakdown
                              , iFreeMemNow ]( FInbetweenerBreakdown* breakdown )
                              {
                                  if( ( iFreeMemNow ) && ( breakdown != iNewDefaultBreakdown ) )
                                  {
                                      delete breakdown;
                                  }

                                  return true;
                              } );

    if( iNewDefaultBreakdown )
    {
        mBreakdownList.push_back( iNewDefaultBreakdown );

        iNewDefaultBreakdown->SetInbetweenerTag( this );

        iNewDefaultBreakdown->SetTargetDrawingIndex( currentTargetDrawingIndex );

        iNewDefaultBreakdown->GetGrid()->SetGeometry( sourceGeometry, eInbetweenerPointPositionType::SourcePosition, true );
        iNewDefaultBreakdown->GetGrid()->SetGeometry( targetGeometry, eInbetweenerPointPositionType::TargetPosition, true );
    }

    ChainBreakdowns();

    Invalidate( INVALIDATE_BREAKDOWN_LIST );
}

void
FOdysseyVectorTagInbetweener::ChainBreakdowns()
{
    uint32 breakdownIndex = 0;

    for( std::list<FInbetweenerBreakdown*>::iterator it = mBreakdownList.begin(); it !=  mBreakdownList.end(); ++it )
    {
        FInbetweenerBreakdown* breakdown = *it;
        std::list<FInbetweenerBreakdown*>::iterator prevIt = std::prev( it );
        std::list<FInbetweenerBreakdown*>::iterator nextIt = std::next( it );

        breakdown->SetIndex( breakdownIndex++ );

        breakdown->SetPrevBreakdown( (     it != mBreakdownList.begin() ) ? *prevIt : nullptr );
        breakdown->SetNextBreakdown( ( nextIt != mBreakdownList.end()   ) ? *nextIt : nullptr );
    }
}

FInbetweenerBreakdown*
FOdysseyVectorTagInbetweener::AddBreakdown( FInbetweenerBreakdown* iNewBreakdown
                                          , uint32 iDrawingIndex
                                          , bool iCopyGeometry
                                          , bool iFitNewTrajectories )
{
    std::vector<::ULIS::FVec2D> curBreakdownSourceGeometry;
    std::vector<::ULIS::FVec2D> curBreakdownInterpGeometry;
    std::list<FInbetweenerBreakdown*>::iterator curBreakdownIterator = GetBreakdownItem( iDrawingIndex, true );

    if( curBreakdownIterator!= mBreakdownList.end() )
    {
        FInbetweenerBreakdown* curBreakdown = *curBreakdownIterator;
        FInbetweenerBreakdown* prevBreakdown = curBreakdown->GetPrevBreakdown();
        int32 curSourceDrawingIndex = curBreakdown->GetSourceDrawingIndex();
        int32 curTargetDrawingIndex = curBreakdown->GetTargetDrawingIndex();
        int32 newSourceDrawingIndex = curSourceDrawingIndex;
        int32 newTargetDrawingIndex = iDrawingIndex;
        // relative inbetween index, i.e within the breakdown
        uint32 inbetweenIndex = iDrawingIndex - curSourceDrawingIndex;
        FChartDivision* inbetween = &curBreakdown->GetChart()->GetDivisionBuffer()[inbetweenIndex];
        FInbetweenerBreakdown* newBreakdown = iNewBreakdown ? iNewBreakdown
                                                            : new FInbetweenerBreakdown( this );
        FInbetweenerChart* curChart = curBreakdown->GetChart();
        FInbetweenerChart* newChart = newBreakdown->GetChart();
        std::vector<float> curChartSpacingBuffer;

        curChart->GetSpacing( curChartSpacingBuffer );

        LockDrawing();

        // This is for an already existing breakdown if it had been removed before
        // (then its pointer to the tag would be null)
        newBreakdown->SetInbetweenerTag( this );

        // Note: SetTargetDrawingIndex() will reset the chart so we do this before the call to SetTargetDrawingIndex()
        if( iCopyGeometry )
        {
            // we will take grid coords from interpolated coords
            DeformGridAtInbetween( inbetween );

            curBreakdown->GetGrid()->GetGeometry( curBreakdownSourceGeometry, eInbetweenerPointPositionType::SourcePosition );
            curBreakdown->GetGrid()->GetGeometry( curBreakdownInterpGeometry, eInbetweenerPointPositionType::InterpPosition );
        }

        mBreakdownList.insert( curBreakdownIterator, newBreakdown );

        ChainBreakdowns();

        newBreakdown->SetTargetDrawingIndex( newTargetDrawingIndex );

        if( iFitNewTrajectories )
        {
            FitRoutes( iDrawingIndex );
        }

        // adapt the new spacings for the new breakdown
        for( uint32 i = 1; i < inbetweenIndex; i++ )
        {
            float newSpacing = ( curChartSpacingBuffer[i] ) / ( curChartSpacingBuffer[inbetweenIndex] );
            // Note: inbetweenIndex cannot be 0
            newChart->GetDivisionBuffer()[i].spacing = newSpacing;
        }

        // adapt the new spacings for the current breakdown
        for( uint32 i = inbetweenIndex + 1, j = 1; i < (uint32) curChartSpacingBuffer.size() - 1; i++, j++ )
        {
            float newSpacing = ( curChartSpacingBuffer[i] - curChartSpacingBuffer[inbetweenIndex] )
                             / ( 1.0f - curChartSpacingBuffer[inbetweenIndex] );
            // Note: inbetweenIndex cannot be 0
            curChart->GetDivisionBuffer()[j].spacing = newSpacing;
        }

        /*ResizeRoutes();*/

        // must be done after the intertweaning of the breakdowns
        if( iCopyGeometry )
        {
            // compute mid-way transformation
            double t = ( double ) ( iDrawingIndex - curSourceDrawingIndex ) / ( curTargetDrawingIndex - curSourceDrawingIndex );
            double prevTranslationX = prevBreakdown ? prevBreakdown->GetTargetTranslationX() : 0.0f;
            double prevTranslationY = prevBreakdown ? prevBreakdown->GetTargetTranslationY() : 0.0f;
            double prevRotation = prevBreakdown ? prevBreakdown->GetTargetRotation() : 0.0f;
            double prevScalingX = prevBreakdown ? prevBreakdown->GetTargetScalingX() : 1.0f;
            double prevScalingY = prevBreakdown ? prevBreakdown->GetTargetScalingY() : 1.0f;
            double translationX = curBreakdown->GetTargetTranslationX();
            double translationY = curBreakdown->GetTargetTranslationY();
            double rotation = curBreakdown->GetTargetRotation();
            double scalingX = curBreakdown->GetTargetScalingX();
            double scalingY = curBreakdown->GetTargetScalingY();

            newBreakdown->GetGrid()->SetGeometry( curBreakdownSourceGeometry, eInbetweenerPointPositionType::SourcePosition, true );
            newBreakdown->GetGrid()->SetGeometry( curBreakdownInterpGeometry, eInbetweenerPointPositionType::TargetPosition, true );

            newBreakdown->SetTargetTransform( prevTranslationX + ( ( translationX - prevTranslationX ) * t )
                                            , prevTranslationY + ( ( translationY - prevTranslationY ) * t )
                                            , prevRotation     + ( ( rotation     - prevRotation     ) * t )
                                            , prevScalingX     + ( ( scalingX     - prevScalingX     ) * t )
                                            , prevScalingY     + ( ( scalingY     - prevScalingY     ) * t ) );
            newBreakdown->UpdateMatrix();
        }

        UnlockDrawing();

        // Invalidation might trigger a redrawing. It must be done outside the mutex locking mechanism
        // because redrawing will also lock the mutex.
        Invalidate( INVALIDATE_BREAKDOWN_LIST );

        return newBreakdown;
    }

    return nullptr;
}

FInbetweenerBreakdown*
FOdysseyVectorTagInbetweener::GetBreakdownByTargetIndex( uint32 iDrawingIndex )
{
    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        if( breakdown->GetTargetDrawingIndex() == iDrawingIndex )
        {
            return breakdown;
        }
    }

    return nullptr;
}

FInbetweenerBreakdown*
FOdysseyVectorTagInbetweener::GetBreakdownByCellIndex( uint32 iCellIndex )
{
    uint32 tagCellIndex = mScene->GetEngine()->GetCell()->GetIndex();
    int32 drawingIndex = ( mInterpolationDirection == eInbetweenerInterpolationDirection::Forward ) ? ( iCellIndex - tagCellIndex )
                                                                                                    : ( tagCellIndex - iCellIndex );

    if( drawingIndex >= 0 )
    {
        return GetBreakdown( drawingIndex, false );
    }

    return nullptr;
}

FInbetweenerBreakdown*
FOdysseyVectorTagInbetweener::GetBreakdown( uint32 iDrawingIndex, bool iStrict )
{
    std::list<FInbetweenerBreakdown*>::iterator curBreakdownIterator = GetBreakdownItem( iDrawingIndex, iStrict );

    return ( curBreakdownIterator != mBreakdownList.end() ) ? *curBreakdownIterator : nullptr;
}

std::list<FInbetweenerBreakdown*>::iterator
FOdysseyVectorTagInbetweener::GetBreakdownItem( uint32 iDrawingIndex, bool iStrict )
{
    std::list<FInbetweenerBreakdown*>::iterator
        curBreakdownIterator = std::find_if( mBreakdownList.begin()
                                           , mBreakdownList.end()
                                           , [ &iDrawingIndex
                                             , &iStrict ]( FInbetweenerBreakdown* breakdown ) -> bool
                                             {
                                                 if( iStrict )
                                                 {
                                                     if( ( iDrawingIndex > breakdown->GetSourceDrawingIndex() )
                                                      && ( iDrawingIndex < breakdown->GetTargetDrawingIndex() ) )
                                                     {
                                                         return true;
                                                     }
                                                 }
                                                 else
                                                 {
                                                     if( ( iDrawingIndex >= breakdown->GetSourceDrawingIndex() )
                                                      && ( iDrawingIndex <= breakdown->GetTargetDrawingIndex() ) )
                                                     {
                                                         return true;
                                                     }
                                                 }

                                                 return false;
                                             } );

    return curBreakdownIterator;
}

void
FOdysseyVectorTagInbetweener::RemoveBreakdown( FInbetweenerBreakdown* iBreakdown, bool iFreeMemNow )
{
    LockDrawing();

    if( mBreakdownList.size() > 1 )
    {
        FInbetweenerBreakdown* nextBreakdown = iBreakdown->GetNextBreakdown();

        mBreakdownList.remove_if( [iBreakdown]( FInbetweenerBreakdown* listedBreakdown )
                                    {
                                        return ( iBreakdown == listedBreakdown ) ? true : false;
                                    } );

        iBreakdown->SetInbetweenerTag( nullptr );

        ChainBreakdowns();

        if( nextBreakdown )
        {
            uint32 removedBreakdownDrawingCount = iBreakdown->GetDrawingCount();
            uint32 nextBreakdownDrawingCount = nextBreakdown ? nextBreakdown->GetDrawingCount() : 0;
            uint32 totalDrawingCount = ( removedBreakdownDrawingCount + nextBreakdownDrawingCount );
            float leftRatio = ( float ) removedBreakdownDrawingCount / totalDrawingCount;
            float rightRatio = ( float ) nextBreakdownDrawingCount / totalDrawingCount;
            std::vector<::ULIS::FVec2D> breakdownSourceGeometry;
            std::vector<float> removedBreakdownSpacing;
            std::vector<float> nextBreakdownSpacing;
            uint32 newInbetweenIndex, i;

            iBreakdown->GetChart()->GetSpacing( removedBreakdownSpacing );
            nextBreakdown->GetChart()->GetSpacing( nextBreakdownSpacing );

            // This will force reallocation of the chart and dispatching of drawings
            nextBreakdown->SetTargetDrawingIndex( nextBreakdown->GetTargetDrawingIndex() );

            // Adapt the spacings
            for( i = 1, newInbetweenIndex = 1; i < removedBreakdownDrawingCount - 1; i++, newInbetweenIndex++ )
            {
                nextBreakdown->GetChart()->GetDivisionBuffer()[newInbetweenIndex].spacing = removedBreakdownSpacing[i] * leftRatio;
            }

            nextBreakdown->GetChart()->GetDivisionBuffer()[newInbetweenIndex++].spacing = leftRatio;

            for( i = 1                       ; i < nextBreakdownDrawingCount    - 1; i++, newInbetweenIndex++ )
            {
                nextBreakdown->GetChart()->GetDivisionBuffer()[newInbetweenIndex].spacing = leftRatio + ( nextBreakdownSpacing[i] * rightRatio );
            }
            // ------------------

            iBreakdown->GetGrid()->GetGeometry( breakdownSourceGeometry, eInbetweenerPointPositionType::SourcePosition );

            // nextBreakdown source grid gets its shape from this removed breakdown source grid.
            nextBreakdown->GetGrid()->SetGeometry( breakdownSourceGeometry, eInbetweenerPointPositionType::SourcePosition, true );
        }
    }

    UnlockDrawing();

    // Invalidation might trigger a redrawing. It must be done outside the mutex locking mechanism
    // because redrawing will also lock the mutex.
    Invalidate( INVALIDATE_BREAKDOWN_LIST );
}

void
FOdysseyVectorTagInbetweener::ResizeRoutes()
{
    for( FInbetweenerRoute* route : mRouteList )
    {
        route->Resize();
    }
}

void
FOdysseyVectorTagInbetweener::FitRoutes( uint32 iFitFrom )
{
    for( FInbetweenerRoute* route : mRouteList )
    {
        route->Fit( iFitFrom );
    }
}

std::list<FInbetweenerBreakdown*>&
FOdysseyVectorTagInbetweener::GetBreakdownList()
{
    return mBreakdownList;
}

uint32
FOdysseyVectorTagInbetweener::GetBreakdownCount()
{
    return mBreakdownList.size();
}

std::vector<uint32>&
FOdysseyVectorTagInbetweener::GetUsedQuadIndexBuffer()
{
    return mUsedQuadIndexBuffer;
}

std::vector<uint32>&
FOdysseyVectorTagInbetweener::GetUsedPointIndexBuffer()
{
    return mUsedPointIndexBuffer;
}

void
FOdysseyVectorTagInbetweener::SetUsedQuadCount( uint32 iUsedQuadCount )
{
    std::vector<FInbetweenerQuad>& quadBuffer = mBreakdownList.front()->GetGrid()->GetQuadBuffer();

    mUsedQuadCount = iUsedQuadCount;

    // acceleration structure
    mUsedQuadIndexBuffer.clear();
    mUsedQuadIndexBuffer.reserve( iUsedQuadCount );

    // Acceleration structure for faster access to the useful quads
    for( uint32 i = 0; i < quadBuffer.size(); i++ )
    {
        if( quadBuffer[i].IsLinked() )
        {
            mUsedQuadIndexBuffer.push_back( i );
        }
    }
    //---------------
}

void
FOdysseyVectorTagInbetweener::SetUsedPointCount( uint32 iUsedPointCount  )
{
    std::vector<FInbetweenerPoint>& pointBuffer = mBreakdownList.front()->GetGrid()->GetPointBuffer();

    mUsedPointCount = iUsedPointCount;

    // acceleration structure
    mUsedPointIndexBuffer.clear();
    mUsedPointIndexBuffer.reserve( iUsedPointCount );

    // Acceleration structure for faster access to the useful points
    for( uint32 i = 0; i < pointBuffer.size(); i++ )
    {
        if( pointBuffer[i].GetQuadCount() )
        {
            mUsedPointIndexBuffer.push_back( i );
        }
    }
}

uint32
FOdysseyVectorTagInbetweener::GetUsedQuadCount()
{
    return mUsedQuadCount;
}

uint32
FOdysseyVectorTagInbetweener::GetUsedPointCount()
{
    return mUsedPointCount;
}

void
FOdysseyVectorTagInbetweener::ObjectAdded()
{
    mScene = mOwner->GetScene();
    mSharedEnv = mOwner->GetSharedEnv();

    if( bShared == false )
    {
        if( mScene )
        {
            Share( mSharedEnv );
        }

        // commented-out : celles are redrawn in the FOdysseyVectorTagInbetweener::Update() method
        //RedrawCells();
    }
}

void
FOdysseyVectorTagInbetweener::ObjectRemoved()
{
    if( bShared == true )
    {
        Unshare( mSharedEnv );

        //RedrawCells();
    }
}

void
FOdysseyVectorTagInbetweener::Added()
{
    mScene = mOwner->GetScene();
    mSharedEnv = mOwner->GetSharedEnv();

    if( mSharedEnv )
    {
        Share( mSharedEnv );
    }
/* will be remade by Update()
    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        breakdown->GetGrid()->Make( true );
    }
*/
    RedrawCells();
}

void
FOdysseyVectorTagInbetweener::Removed()
{
    Unshare( mSharedEnv );

    RedrawCells();
}

void FOdysseyVectorTagInbetweener::Update( uint32 iUpdateFlags
                                         , uint64 iOwnerInvalidationFlags )
{
    if( ( bShared == true )
     && ( ( iUpdateFlags & FOdysseyVectorObject::UPDATE_NOINBETWEENING ) == 0 ) )
    {
        LockDrawing();

        if( ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_HIERARCHY      )
         || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_SHAPE          )
         || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_TOPOLOGY       )
         || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_TAG_LIST )
         || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_TOPOLOGY )
         || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_SHAPE    ) )
        {
            for( FInbetweenerBreakdown* breakdown : mBreakdownList )
            {
                std::vector<::ULIS::FVec2D> sourceGeometry;
                std::vector<::ULIS::FVec2D> targetGeometry;
/*
                // save positions for restoring when calling Make()
                if( iUpdateFlags & FOdysseyVectorObject::UPDATE_FROMFILE )
                {
                    breakdown->GetGrid()->GetGeometry( sourceGeometry, eInbetweenerPointPositionType::SourcePosition );
                }
*/
                breakdown->GetGrid()->GetGeometry( targetGeometry, eInbetweenerPointPositionType::TargetPosition );
                // Note: we cannot call Invalidate in Make() (hence the "false" arg), so we set the flags manually.
                breakdown->GetGrid()->Make( sourceGeometry, targetGeometry, false );

                // calling Invalidate make trigger a call to draw and this would block due to the mutexes.
                mInvalidationFlags |= ( INVALIDATE_MAP );
            }
        }

        if( ( mInvalidationFlags & INVALIDATE_MAP            )
            || ( mInvalidationFlags & INVALIDATE_BREAKDOWN_LIST )
            // owner flags
            || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_HIERARCHY      )
            || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_TOPOLOGY       )
            || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_SHAPE          )
            || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_TAG_LIST )
            || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_TOPOLOGY )
            || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_SHAPE    ) )
        {
            // map object to the first grid
            Map();

            mInvalidationFlags |= INVALIDATE_BUFFERS;
        }

        // will update grids' BBoxes (needed for transform HUD and discarding of unused quads in ARAP grids)
        for( FInbetweenerBreakdown* breakdown : mBreakdownList )
        {
            breakdown->GetGrid()->UpdateBBox( iUpdateFlags, mInvalidationFlags );
        }

        if( mInvalidationFlags & INVALIDATE_BUFFERS )
        {
            // alloc position for points at each interpolation step
            AllocBuffers();

            DeformPathsAtSource();
        }

        // will update grids' center of mass (needed for interpolation).
        // MUST be done after mapping because mapping will elimniate some quads, and this is taken into account
        // for the center of mass.�
        for( FInbetweenerBreakdown* breakdown : mBreakdownList )
        {
            FChartDivision* inbetween = &breakdown->GetChart()->GetDivisionBuffer().back();

            // Precompute ARAP interpolation after the grid and routes have been updated
            if( ( mInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_INTERPOLATIONTYPE )
             || ( mInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_GRIDTYPE          )
             || ( mInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_SPACING           )
             || ( mInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_MAP               ) )
            {
                if( breakdown->GetInbetweenerTag()->GetInterpolationType() == eInbetweenerInterpolationType::ARAP )
                {
                     breakdown->GetGrid()->UpdateCenterOfMass( iUpdateFlags, mInvalidationFlags );

                    bARAPPrecomputeSucceded = breakdown->GetGrid()->PrecomputeARAPInterpolation();

                    if( bARAPPrecomputeSucceded  == false )
                    {
                        UE_LOG( LogTemp, Error, TEXT("ERROR DURING ARAP PRECOMPUTE"));
                    }
                }

                // altering breakdown range alters buffers. We then have to deform target anew.
                // deform the path according to grid geometry
                breakdown->GetGrid()->DeformPaths( mInterpolatedPathBuffer
                                                 , inbetween
                                                 , eInbetweenerPointPositionType::TargetPosition );
            }
        }

        for( FInbetweenerRoute* route : mRouteList )
        {
            route->Update( iUpdateFlags, iOwnerInvalidationFlags, mInvalidationFlags );
        }

        // TODO: separate Transform interpolation from shape interpolation. Transform interpolation
        // should then react to a specific flag, as well as shape interpolation
        Interpolate();

        if( ( iUpdateFlags & FOdysseyVectorObject::UPDATE_INTERACTIVE ) == 0 )
        {
            mInvalidationFlags = 0;
        }

        UnlockDrawing();
    }

    if( ( iUpdateFlags & FOdysseyVectorObject::UPDATE_INTERACTIVE ) == 0 )
    {
        RedrawCells();
    }
}

FOdysseyVectorGroupPaint*
FOdysseyVectorTagInbetweener::GetScene()
{
    return mScene;
}

IOdysseyVectorCell*
FOdysseyVectorTagInbetweener::GetCell()
{
    return mOwner->GetEngine()->GetCell();
}

IOdysseyVectorCell*
FOdysseyVectorTagInbetweener::GetSourceCell()
{
    uint32 sourceCellIndex = GetSourceCellIndex();

    return GetOwner()->GetEngine()->GetLayer()->GetCellByIndex( sourceCellIndex );
}

int32
FOdysseyVectorTagInbetweener::GetSourceCellIndex()
{
    return mBreakdownList.front()->GetSourceCellIndex();
}

IOdysseyVectorCell*
FOdysseyVectorTagInbetweener::GetTargetCell()
{
    uint32 targetCellIndex = GetTargetCellIndex();

    return GetOwner()->GetEngine()->GetLayer()->GetCellByIndex( targetCellIndex );
}

int32
FOdysseyVectorTagInbetweener::GetTargetCellIndex()
{
    return mBreakdownList.back()->GetTargetCellIndex();
}

void
FOdysseyVectorTagInbetweener::Invalidate( uint64 iInvalidationFlags )
{
    // do not reinvalidate
    if( mInvalidationFlags == 0 )
    {
        mOwner->InvalidateTag( this );
    }

    mInvalidationFlags |= iInvalidationFlags;
}

void
FOdysseyVectorTagInbetweener::ResizeDrawings()
{
    mDrawingBuffer.resize( GetLength(), this );

    // TODO: reset these transformations in a function
    mDrawingBuffer.front().translationX = 0.0f;
    mDrawingBuffer.front().translationY = 0.0f;
    mDrawingBuffer.front().rotation = 0.0f;
    mDrawingBuffer.front().scalingX = 1.0f;
    mDrawingBuffer.front().scalingY = 1.0f;

    mDrawingBuffer.front().localMatrix   =
    mDrawingBuffer.front().inverseMatrix =
    mDrawingBuffer.front().worldMatrix   =
    mDrawingBuffer.front().inverseWorldMatrix = BLMatrix2D::makeIdentity();

    DispatchDrawings();

    ResizeRoutes();
}

void
FOdysseyVectorTagInbetweener::UpdateMatrix()
{
    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        // deform the path according to grid geometry
        breakdown->UpdateMatrix();
    }
}

void
FOdysseyVectorTagInbetweener::DeformPathsAtSource()
{
    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        FChartDivision* inbetween = &breakdown->GetChart()->GetDivisionBuffer().front();

        // deform the path according to grid geometry
        breakdown->GetGrid()->DeformPaths( mInterpolatedPathBuffer
                                         , inbetween
                                         , eInbetweenerPointPositionType::SourcePosition );

        // we do it only for the first breakdown
        break;
    }
}

void
FOdysseyVectorTagInbetweener::DeformPathsAtTarget()
{
    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        FChartDivision* inbetween = &breakdown->GetChart()->GetDivisionBuffer().back();

        // deform the path according to grid geometry
        breakdown->GetGrid()->DeformPaths( mInterpolatedPathBuffer
                                         , inbetween
                                         , eInbetweenerPointPositionType::TargetPosition );
    }
}

std::vector<FInbetweenerDrawing>&
FOdysseyVectorTagInbetweener::GetDrawingBuffer()
{
    return mDrawingBuffer;
}

void
FOdysseyVectorTagInbetweener::DispatchDrawings()
{
    double breakdownFirstSpacing = 0.0f;

    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        uint32 sourceDrawingIndex = breakdown->GetSourceDrawingIndex();
        uint32 targetDrawingIndex = breakdown->GetTargetDrawingIndex();

        for( uint32 i = 0; i < GetLength(); i++ )
        {
            if( ( i >= sourceDrawingIndex ) && ( i <= targetDrawingIndex ) )
            {
                uint32 chartDivisionIndex = i - sourceDrawingIndex;

                breakdown->GetChart()->GetDivisionBuffer()[chartDivisionIndex].drawing = &mDrawingBuffer[i];
            }
        }
    }
}

void
FOdysseyVectorTagInbetweener::DeformGridAtInbetween( FChartDivision *iInbetween )
{
    double t = iInbetween->spacing;

    if( mInterpolationType == eInbetweenerInterpolationType::Linear )
    {
        for( FInbetweenerPoint& point : iInbetween->chart->GetBreakdown()->GetGrid()->GetPointBuffer() )
        {
            if( point.GetQuadCount() )
            {
                ::ULIS::FVec2D targetPosition = point.GetTargetPosition();
                ::ULIS::FVec2D sourcePosition = point.GetSourcePosition();
                ::ULIS::FVec2D diff = ( targetPosition - sourcePosition );
                ::ULIS::FVec2D step = diff * t;

                // TODO: protect the access by using methods
                point.mInterpPosition = sourcePosition + step;
            }
        }
    }

    if( mInterpolationType == eInbetweenerInterpolationType::ARAP )
    {
        if( bARAPPrecomputeSucceded )
        {
            iInbetween->chart->GetBreakdown()->GetGrid()->ComputeARAPInterpolation( iInbetween, false );
        }
    }
}

void
FOdysseyVectorTagInbetweener::DeformPathsAtInbetween( FChartDivision *iInbetween )
{
    double t = iInbetween->spacing;

    DeformGridAtInbetween( iInbetween );

    // deform the path according to grid geometry
    iInbetween->chart->GetBreakdown()->GetGrid()->DeformPaths( mInterpolatedPathBuffer
                                                             , iInbetween
                                                             , eInbetweenerPointPositionType::InterpPosition );
}

uint32
FOdysseyVectorTagInbetweener::GetLength()
{
    return mBreakdownList.back()->GetTargetDrawingIndex() + 1;
}

void
FOdysseyVectorTagInbetweener::RedrawCells()
{
    RedrawCells( GetLength() );
}

int32
FOdysseyVectorTagInbetweener::GetDrawingIndexFromCellIndex( uint32 iCellIndex )
{
    uint32 tagCellIndex = GetSourceCellIndex();

    if( mInterpolationDirection == eInbetweenerInterpolationDirection::Forward )
    {
        return iCellIndex - tagCellIndex;
    }


    if( mInterpolationDirection == eInbetweenerInterpolationDirection::Backward )
    {
        return tagCellIndex - iCellIndex;
    }

    return 0;
}

void
FOdysseyVectorTagInbetweener::RedrawCells( uint32 iDrawingCount )
{
    // scene could be non existent when the tag's owner is removed, as it would still trigger call to Update()
    // right after the removal of an object in the hierarchy.
    if( mSharedEnv )
    {
        IOdysseyVectorCell* cell = mScene->GetEngine()->GetCell();
        IOdysseyVectorLayer* layer = mScene->GetEngine()->GetLayer();

        if( cell )
        {
            int32 cellIndex = cell->GetIndex();

            // Redraw impacted cells
            for( uint32 i = 1; ( i < iDrawingCount ) && ( cell != nullptr ); i++ )
            {
                FInbetweenerDrawing* drawing = GetDrawing( i );
                int32 inbetweenCellIndex = drawing->GetCellIndex();
                IOdysseyVectorCell* nextCell = layer->GetCellByIndex( inbetweenCellIndex );

                if( nextCell )
                {
                    //nextCell->GetEngine()->Invalidate();
                    // request redraw
                    nextCell->GetEngine()->Invalidate( 0 );
                }

                cell = nextCell;
            }
        }
    }
}

void
FOdysseyVectorTagInbetweener::Interpolate()
{
    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        breakdown->InterpolateTransform();
    }

    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        uint32 sourceDrawingIndex = breakdown->GetSourceDrawingIndex();
        uint32 targetDrawingIndex = breakdown->GetTargetDrawingIndex();

        for( uint32 i = 1; i < breakdown->GetDrawingCount() - 1; i++ )
        {
            FChartDivision* inbetween = &breakdown->GetChart()->GetDivisionBuffer()[i];

            DeformPathsAtInbetween( inbetween );
        }
    }
}

/*
void
FOdysseyVectorTagInbetweener::DrawMotionGrid( uint32 iDrawingIndex
                                            , BLContext* iBLContext
                                            , const ::ULIS::FRectD& iInvalidationArea
                                            , double iAncestorsOpacity
                                            , uint64 iDrawingFlags )
{
    BLMatrix2D worldMatrix = mOwner->GetWorldMatrix();

    worldMatrix.transform( mChart.drawingBuffer[iDrawingIndex].matrix );

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
*/

void
FOdysseyVectorTagInbetweener::Draw( BLContext* iBLContext
                                  , const ::ULIS::FRectD& iInvalidationArea
                                  , double iAncestorsOpacity
                                  , uint64 iDrawingFlags )
{

}

// when drawn as a shared tag
void
FOdysseyVectorTagInbetweener::Draw( FOdysseyVectorGroupPaint* iDisplayedScene
                                  , BLContext* iBLContext
                                  , const ::ULIS::FRectD& iInvalidationArea
                                  , double iAncestorsOpacity
                                  , uint64 iDrawingFlags )
{
    IOdysseyVectorCell* displayedCell = iDisplayedScene->GetEngine()->GetCell();

    LockDrawing();

    // check the object is still displayed (it could have been removed but still in memory)
    if( mOwner->GetScene() )
    {

        IOdysseyVectorCell* tagCell = mOwner->GetScene()->GetEngine()->GetCell();

        iBLContext->save();
        iBLContext->resetMatrix();

        // if th eobject hasn't been removed from the scene
        if( displayedCell && tagCell )
        {
            uint32 sourceCellIndex = tagCell->GetIndex();
            uint32 targetCellIndex = sourceCellIndex + ( ( GetLength() - 1 ) * (int)mInterpolationDirection );
            uint32 displayedCellIndex = displayedCell->GetIndex();
            uint32 fromCellIndex = std::min( sourceCellIndex, targetCellIndex );
            uint32   toCellIndex = std::max( sourceCellIndex, targetCellIndex );

            if ( ( displayedCellIndex > fromCellIndex )
              && ( displayedCellIndex <= toCellIndex   ) )
            {
                uint32 drawingIndex = abs( (int) (displayedCellIndex - sourceCellIndex) );

                // don't draw the object at the source position, it's already drawn
                if( drawingIndex > 0 )
                {
                    FInbetweenerBreakdown* breakdown = GetBreakdown( drawingIndex, false );

                    if( ( drawingIndex != breakdown->GetTargetDrawingIndex() ) || breakdown->IsTargetVisible() )
                    {
                        uint32 inbetweenIndex = drawingIndex - breakdown->GetSourceDrawingIndex();
                        FChartDivision* inbetween = &breakdown->GetChart()->GetDivisionBuffer()[inbetweenIndex];

                        DrawPathsInbetween( iDisplayedScene
                                          , inbetween
                                          , iBLContext
                                          , false );
                    }
                }
            }
        }

        iBLContext->restore();
    }

    UnlockDrawing();
}

static ::ULIS::FVec2D
GetPerpendicularVector( const ::ULIS::FVec2D* iP0
                      , const ::ULIS::FVec2D& iP1
                      , const ::ULIS::FVec2D* iP2 )
{
   ::ULIS::FVec2D p0p1 = ::ULIS::FVec2D( 0.0f, 0.0f );
   ::ULIS::FVec2D p1p2 = ::ULIS::FVec2D( 0.0f, 0.0f );
   ::ULIS::FVec2D average;

    if( iP0 )
    {
        p0p1 = iP1 - (*iP0);
    }

    if( iP2 )
    {
        p1p2 = (*iP2) - iP1;
    }

    average = ( p0p1 + p1p2 );

    if( average.DistanceSquared() )
    {
        average.Normalize();

        return ::ULIS::FVec2D( -average.y, average.x );
    }

    return ::ULIS::FVec2D( 0.0f, 0.0f );
}

void
FOdysseyVectorTagInbetweener::DrawPathAt( FOdysseyVectorGroupPaint* iDisplayedScene
                                        , FChartDivision* iInbetween
                                        , FInterpolatedPath* iInterpolatedPath
                                        , BLContext* iBLContext
                                        , bool iLock )
{
    if( iLock )
        LockDrawing();

    uint32 pointCount = iInterpolatedPath->mInterpolatedPointBuffer.size();
    uint32 inbetweenAbsoluteIndex = iInbetween->GetAbsoluteIndex();
    ::ULIS::FVec2D* pointPositionBuffer = &iInterpolatedPath->mInterpolatedPointPositionBuffer[pointCount * inbetweenAbsoluteIndex];
    float scalingSq = bConstantWidth ? 1.0f / ( iInbetween->drawing->scalingX
                                              * iInbetween->drawing->scalingY ) : 1.0f;
    // note: a surface grows or shrink at the square of the scaling factor.
    // That's why we use sqrt to get the actual scaling factor from the surface ratio.
    float scaling = sqrt( scalingSq );
    FOdysseyVectorBrush& brush = iInterpolatedPath->GetOriginalPath()->GetBrush();
    FColor pathColor = iInterpolatedPath->GetOriginalPath()->GetForegroundColor();
    BLMatrix2D worldMatrix = mOwner->GetWorldMatrix();
    FOdysseyVectorEngine* displayedSceneEngine = iDisplayedScene->GetEngine();

    // passed to DrawPathAt()
    worldMatrix.transform( iInbetween->drawing->localMatrix );

    iBLContext->setStrokeStyle( BLRgba32( pathColor.R, pathColor.G, pathColor.B, pathColor.A ) );
    iBLContext->setFillStyle( BLRgba32( pathColor.R, pathColor.G, pathColor.B, pathColor.A ) );

    brush.Lock();

    for( FInterpolatedSegment& interpolatedSegment : iInterpolatedPath->mInterpolatedSegmentBuffer )
    {
        if( bMapAsPolyline )
        {
            FOdysseyVectorSegment* segment = interpolatedSegment.GetOriginalSegment();
            std::vector<FOdysseyVectorFraction>& fractionCache = segment->GetFractionCache();
            uint32 segmentPointCount = interpolatedSegment.mInterpolatedPointArray.size();
            ::ULIS::FVec2D perpi;
            ::ULIS::FVec2D perpn;

            for( uint32 i = 0; i < segmentPointCount - 1; i++ )
            {
                uint32 p = i - 1;
                uint32 n = i + 1;
                uint32 q = i + 2;
                FInterpolatedPoint* pointp = ( i == 0                 ) ? nullptr : interpolatedSegment.mInterpolatedPointArray[p];
                FInterpolatedPoint* pointi = interpolatedSegment.mInterpolatedPointArray[i];
                FInterpolatedPoint* pointn = interpolatedSegment.mInterpolatedPointArray[n];
                FInterpolatedPoint* pointq = ( q == segmentPointCount ) ? nullptr : interpolatedSegment.mInterpolatedPointArray[q];
                ::ULIS::FVec2D* localPointPositionp = pointp ? &pointPositionBuffer[pointp->mIndex] : nullptr;
                ::ULIS::FVec2D* localPointPositioni = &pointPositionBuffer[pointi->mIndex];
                ::ULIS::FVec2D* localPointPositionn = &pointPositionBuffer[pointn->mIndex];
                ::ULIS::FVec2D* localPointPositionq = pointq ? &pointPositionBuffer[pointq->mIndex] : nullptr;

                if( 1/*bWithThickness*/ )
                {
                    perpi = i == 0 ? GetPerpendicularVector(  localPointPositionp
                                                           , *localPointPositioni
                                                           ,  localPointPositionn )
                                   // No need to recompute, just use the next value
                                   : perpn;
                    perpn = GetPerpendicularVector(  localPointPositioni
                                                  , *localPointPositionn
                                                  ,  localPointPositionq );
                    double radiusi = pointi->GetRadius() * scaling;
                    double radiusn = pointn->GetRadius() * scaling;
                    BLPoint pt[6] = { worldMatrix.mapPoint( localPointPositioni->x
                                                          , localPointPositioni->y )
                                    , worldMatrix.mapPoint( localPointPositioni->x + ( perpi.x * radiusi )
                                                          , localPointPositioni->y + ( perpi.y * radiusi ) )
                                    , worldMatrix.mapPoint( localPointPositionn->x + ( perpn.x * radiusn )
                                                          , localPointPositionn->y + ( perpn.y * radiusn ) )
                                    , worldMatrix.mapPoint( localPointPositionn->x
                                                          , localPointPositionn->y )
                                    , worldMatrix.mapPoint( localPointPositionn->x - ( perpn.x * radiusn )
                                                          , localPointPositionn->y - ( perpn.y * radiusn ) )
                                    , worldMatrix.mapPoint( localPointPositioni->x - ( perpi.x * radiusi )
                                                          , localPointPositioni->y - ( perpi.y * radiusi ) ) };

                                        // HUDs pass nullptr as the displayedScene
                    if( brush.pixels )
                    {
                        FOdysseyVectorFraction* fraction = &fractionCache[i];
                        double startU = segment->GetTextureStartU();
                        double endU = segment->GetTextureEndU();
                        double difU = endU - startU;

                         // We have to divide the hexagon into 2 quads or else it can creates artefacts
                        // due to UV Mapping when the hexagon is not "a square".
                        ::ULIS::FVec2D quad0P[4] = { { pt[0].x, pt[0].y }
                                                   , { pt[1].x, pt[1].y }
                                                   , { pt[2].x, pt[2].y }
                                                   , { pt[3].x, pt[3].y } };
                        double quad0U[4] = { startU + ( fraction->polygon.U[0] * difU )
                                           , startU + ( fraction->polygon.U[1] * difU )
                                           , startU + ( fraction->polygon.U[2] * difU )
                                           , startU + ( fraction->polygon.U[3] * difU ) };
                        double quad0V[4] = { fraction->polygon.V[0]
                                           , fraction->polygon.V[1]
                                           , fraction->polygon.V[2]
                                           , fraction->polygon.V[3] };
                        ::ULIS::FVec2D quad1P[4] = { { pt[3].x, pt[3].y }
                                                   , { pt[4].x, pt[4].y }
                                                   , { pt[5].x, pt[5].y }
                                                   , { pt[0].x, pt[0].y } };
                        double quad1U[4] = { startU + ( fraction->polygon.U[3] * difU )
                                           , startU + ( fraction->polygon.U[4] * difU )
                                           , startU + ( fraction->polygon.U[5] * difU )
                                           , startU + ( fraction->polygon.U[0] * difU ) };
                        double quad1V[4] = { fraction->polygon.V[3]
                                           , fraction->polygon.V[4]
                                           , fraction->polygon.V[5]
                                           , fraction->polygon.V[0] };
                        uint64 polygonDrawingFlags = 0;

                        polygonDrawingFlags |= brush.ColorFromBrush    ? 0 : FPolygonDrawingFlags::BRUSHALPHAONLY;
                        polygonDrawingFlags |= brush.BilinearFiltering ? FPolygonDrawingFlags::BILINEARFILTERING : 0;

                        // should be a static function
                        displayedSceneEngine->FillQuad( iBLContext
                                                      , quad0P
                                                      , quad0U
                                                      , quad0V
                                                      , 1.0f /*iCombinedOpacity*/
                                                      , pathColor
                                                      , (int8*) brush.pixels // will be nullptr if no texture is loaded
                                                      , brush.width
                                                      , brush.height
                                                      , brush.bitsPerPixel
                                                      , polygonDrawingFlags );

                        displayedSceneEngine->FillQuad( iBLContext
                                                      , quad1P
                                                      , quad1U
                                                      , quad1V
                                                      , 1.0f /*iCombinedOpacity*/
                                                      , pathColor
                                                      , (int8*) brush.pixels // will be nullptr if no texture is loaded
                                                      , brush.width
                                                      , brush.height
                                                      , brush.bitsPerPixel
                                                      , polygonDrawingFlags );
                    }
                    else
                    {
                        // we draw lines between the polygons to correct the artefacts,
                        // otherwise there is a thin line between the polygons
                        // line stroking is done in world coordinates because we need a 1 pixel width
                        iBLContext->setStrokeWidth( 1.2f * scaling );
                        iBLContext->strokeLine( pt[0], pt[1] );
                        iBLContext->strokeLine( pt[5], pt[0] );

                        iBLContext->fillPolygon( pt, 6 );
                    }
                }
                else
                {
                    BLPoint pt[2] = { worldMatrix.mapPoint( localPointPositioni->x
                                                          , localPointPositioni->y )
                                    , worldMatrix.mapPoint( localPointPositionn->x
                                                          , localPointPositionn->y ) };

                    iBLContext->strokeLine( pt[0].x, pt[0].y, pt[1].x, pt[1].y );
                }
            }
        }
        else
        {
            if( interpolatedSegment.GetOriginalSegment()->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
            {
                FInterpolatedPoint* interpolatedPoint[4] = { interpolatedSegment.mInterpolatedPointArray[0]
                                                           , interpolatedSegment.mInterpolatedPointArray[1]
                                                           , interpolatedSegment.mInterpolatedPointArray[2]
                                                           , interpolatedSegment.mInterpolatedPointArray[3] };
                BLPoint pt[4] = { worldMatrix.mapPoint( pointPositionBuffer[interpolatedPoint[0]->mIndex].x
                                                      , pointPositionBuffer[interpolatedPoint[0]->mIndex].y )
                                , worldMatrix.mapPoint( pointPositionBuffer[interpolatedPoint[1]->mIndex].x
                                                      , pointPositionBuffer[interpolatedPoint[1]->mIndex].y )
                                , worldMatrix.mapPoint( pointPositionBuffer[interpolatedPoint[2]->mIndex].x
                                                      , pointPositionBuffer[interpolatedPoint[2]->mIndex].y )
                                , worldMatrix.mapPoint( pointPositionBuffer[interpolatedPoint[3]->mIndex].x
                                                      , pointPositionBuffer[interpolatedPoint[3]->mIndex].y ) };
                BLPath path;

                path.moveTo ( pt[0].x, pt[0].y );
                path.cubicTo( pt[1].x, pt[1].y
                            , pt[2].x, pt[2].y
                            , pt[3].x, pt[3].y );

                iBLContext->setStrokeWidth( 3.0f );

                iBLContext->strokePath( path );
            }
        }
    }

    brush.Unlock();

    if( iLock )
        UnlockDrawing();
}

std::vector<FInterpolatedPath>&
FOdysseyVectorTagInbetweener::GetInterpolatedPathBuffer()
{
    return mInterpolatedPathBuffer;
}

void
FOdysseyVectorTagInbetweener::SetConstantWidth( bool iConstantWidth )
{
    bConstantWidth = iConstantWidth;

    Invalidate( INVALIDATE_CELLS );
}

bool
FOdysseyVectorTagInbetweener::HasConstantWidth()
{
    return bConstantWidth;
}

void
FOdysseyVectorTagInbetweener::DrawPathsInbetween( FOdysseyVectorGroupPaint* iDisplayedScene
                                                , FChartDivision* inbetween
                                                , BLContext* iBLContext
                                                , bool iLock )
{
    if( iLock )
        LockDrawing();

    iBLContext->save();
    iBLContext->resetMatrix();

    for( FInterpolatedPath& interpolatedPath : mInterpolatedPathBuffer )
    {
        DrawPathAt( iDisplayedScene
                  , inbetween
                  , &interpolatedPath
                  //, pointPositionBuffer
                  //, worldMatrix
                  , iBLContext
                  , false );
    }

    iBLContext->restore();

    if( iLock )
        UnlockDrawing();
}

void
FOdysseyVectorTagInbetweener::MoveInbetween( FChartDivision* iInbetween
                                           , float iNewSpacing
                                           , bool iRelative )
{
    FInbetweenerBreakdown* breakdown = iInbetween->chart->GetBreakdown();

    if( ( iNewSpacing > 0.0f ) && ( iNewSpacing < 1.0f ) )
    {
        IOdysseyVectorCell* animationCell = mOwner->GetScene()->GetEngine()->GetCell();
        int32  prevIndex  = iInbetween->GetIndex() - 1;
        uint32 nextIndex  = iInbetween->GetIndex() + 1;
        float prevSpacing = iInbetween->chart->GetDivisionBuffer()[prevIndex].spacing;
        float nextSpacing = iInbetween->chart->GetDivisionBuffer()[nextIndex].spacing;

        if( iRelative == false )
        {
            if( ( iNewSpacing > prevSpacing )
             && ( iNewSpacing < nextSpacing ) )
            {
                uint32 inbetweenCellIndex = iInbetween->GetCellIndex();
                IOdysseyVectorCell* inbetweenCell = GetOwner()->GetEngine()->GetLayer()->GetCellByIndex( inbetweenCellIndex );

                iInbetween->spacing = iNewSpacing;

                if( inbetweenCell )
                {
                    inbetweenCell->GetEngine()->Invalidate( 0 );
                }

                // recompute single inbetweens
                //InterpolateTransform( iInbetween->index );
                //DeformPathsAtInbetween( iInbetween->index );

                //Interpolate();
            }
        }
        else
        {
            for( uint32 i = 1; i < breakdown->GetDrawingCount()- 1; i++ )
            {
                FChartDivision* otherInbetween = &breakdown->GetChart()->GetDivisionBuffer()[i];

                if( otherInbetween != iInbetween )
                {
                    double otherInbetweenOldSpacing = otherInbetween->spacing;

                    if( otherInbetween->spacing < iInbetween->spacing )
                    {
                        float length = iInbetween->spacing;
                        float ratio = iInbetween->spacing ? ( otherInbetween->spacing / length ) : 0.0f;
                        float newLength = iNewSpacing;

                        otherInbetween->spacing = newLength * ratio;
                    }
                    else
                    {
                        float length = 1.0f - iInbetween->spacing;
                        float ratio = iInbetween->spacing ? ( ( otherInbetween->spacing - iInbetween->spacing ) / length ) : 0.0f;
                        float newLength = 1.0f - iNewSpacing;

                        otherInbetween->spacing = iNewSpacing + ( newLength * ratio );
                    }
                }
            }

            iInbetween->spacing = iNewSpacing;

            // recompute all inbetweens
            //Interpolate();
        }
    }

    Invalidate( INVALIDATE_ROUTES | INVALIDATE_SPACING );
}



void
FOdysseyVectorTagInbetweener::AllocBuffers()
{
    for( FInterpolatedPath& interpolatedPath : mInterpolatedPathBuffer )
    {
        uint32 pointCount = interpolatedPath.mInterpolatedPointBuffer.size();

        // Note: the position at the first drawing will not be used
        interpolatedPath.mInterpolatedPointPositionBuffer.resize( GetLength() * pointCount );
    }
}

void
FOdysseyVectorTagInbetweener::ResetGrid()
{
    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        breakdown->GetGrid()->Make( true );
    }

    Invalidate( INVALIDATE_MAP );
}

eInbetweenerGridType
FOdysseyVectorTagInbetweener::GetGridType()
{
    return mGridType;
}

uint32
FOdysseyVectorTagInbetweener::GetGridNumQuadX()
{
    return mGridNumQuadX;
}

uint32
FOdysseyVectorTagInbetweener::GetGridNumQuadY()
{
    return mGridNumQuadY;
}

// static
void
FOdysseyVectorTagInbetweener::EvalSize( ::ULIS::FRectD& iWorldBBox
                                       , uint32& oGridNumQuadX
                                       , uint32& oGridNumQuadY )
{
    oGridNumQuadX = std::clamp<uint32>( iWorldBBox.w / 24, 1, 32 );
    oGridNumQuadY = std::clamp<uint32>( iWorldBBox.h / 24, 1, 32 );
}

void
FOdysseyVectorTagInbetweener::SetGrid( eInbetweenerGridType iGridType
                                     , uint32 iGridNumQuadX
                                     , uint32 iGridNumQuadY
                                     , bool iSquare )
{
    if( ( mGridNumQuadX != iGridNumQuadX )
     || ( mGridNumQuadY != iGridNumQuadY )
     || ( iSquare != bSquare ) )
    {
        RemoveAllRoutes();
    }

    mGridType = iGridType;
    mGridNumQuadX = iGridNumQuadX ? iGridNumQuadX : 1;
    mGridNumQuadY = iGridNumQuadY ? iGridNumQuadY : 1;
    bSquare = iSquare;

    Invalidate( INVALIDATE_MAP
              | INVALIDATE_SPACING
              | INVALIDATE_GRIDTYPE
              | INVALIDATE_CELLS );

    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        breakdown->SetGrid( iGridType );
        breakdown->GetGrid()->Make( true );
    }
}

void
FOdysseyVectorTagInbetweener::SetGridNumQuad( uint32 iGridNumQuadX
                                            , uint32 iGridNumQuadY
                                            , bool iSquare
                                            , const std::vector<::ULIS::FVec2D>& iSourcePositionBuffer
                                            , const std::vector<::ULIS::FVec2D>& iTargetPositionBuffer )
{
    Invalidate( INVALIDATE_MAP
              | INVALIDATE_SPACING
              | INVALIDATE_CELLS );

    if( ( mGridNumQuadX != iGridNumQuadX ) || ( mGridNumQuadY != iGridNumQuadY ) )
    {
        RemoveAllRoutes();
    }

    mGridNumQuadX = iGridNumQuadX;
    mGridNumQuadY = iGridNumQuadY;
    bSquare = iSquare;

    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        breakdown->GetGrid()->Make( iSourcePositionBuffer
                                  , iTargetPositionBuffer
                                  , true );
    }
}

void
FOdysseyVectorTagInbetweener::SetGridNumQuad( uint32 iGridNumQuadX
                                            , uint32 iGridNumQuadY
                                            , bool iSquare )
{
    Invalidate( INVALIDATE_MAP
              | INVALIDATE_SPACING
              | INVALIDATE_CELLS );

    if( ( mGridNumQuadX != iGridNumQuadX ) || ( mGridNumQuadY != iGridNumQuadY ) )
    {
        RemoveAllRoutes();
    }

    mGridNumQuadX = iGridNumQuadX;
    mGridNumQuadY = iGridNumQuadY;
    bSquare = iSquare;

    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        breakdown->GetGrid()->Make( true );
    }
}

bool
FOdysseyVectorTagInbetweener::IsTopSelectedTag()
{
    FOdysseyVectorObject* parent = mOwner->GetParent();

    while( parent )
    {
        if( parent->IsSelected() && parent->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() ) )
        {
            return false;
        }

        parent = parent->GetParent();
    }

    return true;
}

void
FOdysseyVectorTagInbetweener::Commit( std::list<FOdysseyVectorTag*>& oRemovedTagList
                                    , std::list<FOdysseyVectorObject*>& oAddedObjectList
                                    , std::list<FOdysseyVectorGroupPaint*>& oCommittedSceneList )
{
    IOdysseyVectorCell* cell = mOwner->GetScene()->GetEngine()->GetCell();
    IOdysseyVectorLayer* layer = mOwner->GetScene()->GetEngine()->GetLayer();
    int32 animationCellIndex = cell->GetIndex();

    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        uint32 breakdownSourceDrawingIndex = breakdown->GetSourceDrawingIndex();
        uint32 breakdownTargetDrawingIndex = breakdown->GetTargetDrawingIndex();

        for( uint32 drawingIndex = breakdownSourceDrawingIndex + 1; drawingIndex <= breakdownTargetDrawingIndex; drawingIndex++ )
        {
            IOdysseyVectorCell* inbetweenCell = layer->GetCellByIndex( animationCellIndex + ( drawingIndex * (int)mInterpolationDirection ) );

            if( ( drawingIndex != breakdownTargetDrawingIndex ) || breakdown->IsTargetVisible() )
            {
                if( inbetweenCell )
                {
                    std::list<FOdysseyVectorObject*> newObjectList;

                    // change vertices coords before copying the object
                    std::function<uint64(FOdysseyVectorObject*,uint64)> preProcess = [ drawingIndex ]( FOdysseyVectorObject* vectorObject, uint64 copyFlags ) -> uint64
                    {
                        BLMatrix2D& ownerWorldMatrix = vectorObject->GetWorldMatrix();
                        FOdysseyVectorTag* tag = vectorObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

                        if( tag )
                        {
                            FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);
                            FInbetweenerDrawing* drawing = inbetweenerTag->GetDrawing( drawingIndex );

                            if( inbetweenerTag->GetMapAsPolyline() )
                            {
                                copyFlags |= FOdysseyVectorObject::COPY_RETOPOLOGY;
                            }
                            else
                            {
                                copyFlags &= (~FOdysseyVectorObject::COPY_RETOPOLOGY);
                            }

                            for( FInterpolatedPath& interpolatedPath : inbetweenerTag->mInterpolatedPathBuffer )
                            {
                                uint32 pointCount = interpolatedPath.mInterpolatedPointBuffer.size();
                                uint32 skippedOffset = ( drawingIndex * pointCount );



                                for( uint32 i = 0; i < interpolatedPath.mInterpolatedPointBuffer.size(); i++ )
                                {
                                    FInterpolatedPoint* interpolatedPoint = &interpolatedPath.mInterpolatedPointBuffer[i];
                                    ::ULIS::FVec2D* commitPosition = &interpolatedPath.mInterpolatedPointPositionBuffer[skippedOffset + i];
                                    ::ULIS::FVec2D swapPosition = interpolatedPoint->mOriginalPoint->GetCoords();
                                    ::ULIS::FVec2D transformedLocalPosition;
                                    //double transformedLocalRadius;
                                    //BLPoint transformedWorldPosition;

                                    transformedLocalPosition = FOdysseyVector::MapPoint( drawing->localMatrix, ::ULIS::FVec2D( commitPosition->x
                                                                                                                             , commitPosition->y ) );


                                    interpolatedPoint->mOriginalPoint->Set( transformedLocalPosition.x
                                                                          , transformedLocalPosition.y );

                                    *commitPosition = swapPosition;
                                }
                            }
                        }

                        return copyFlags;
                    };

                    std::function<uint64(FOdysseyVectorObject*,FOdysseyVectorObject*,uint64)> postProcess = [ drawingIndex
                                                                                                            , preProcess
                                                                                                            , &newObjectList ]( FOdysseyVectorObject* sourceObject
                                                                                                                              , FOdysseyVectorObject* objectCopy
                                                                                                                              , uint64 copyFlags ) -> uint64
                    {
                        FOdysseyVectorTag* tag = sourceObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

                        newObjectList.push_back( objectCopy );

                        if( tag )
                        {
                            FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);
                            FInbetweenerDrawing* drawing = inbetweenerTag->GetDrawing( drawingIndex );

                            //drawing->localMatrix.reset();

                            // revert vertices coords after having copied the object.
                            // Coords were saved in the point position buffer
                            for( FInterpolatedPath& interpolatedPath : inbetweenerTag->mInterpolatedPathBuffer )
                            {
                                uint32 pointCount = interpolatedPath.mInterpolatedPointBuffer.size();
                                uint32 skippedOffset = ( drawingIndex * pointCount );

                                for( uint32 i = 0; i < interpolatedPath.mInterpolatedPointBuffer.size(); i++ )
                                {
                                    FInterpolatedPoint* interpolatedPoint = &interpolatedPath.mInterpolatedPointBuffer[i];
                                    ::ULIS::FVec2D* commitPosition = &interpolatedPath.mInterpolatedPointPositionBuffer[skippedOffset + i];

                                    interpolatedPoint->mOriginalPoint->Set( commitPosition->x
                                                                          , commitPosition->y );
                                }
                            }
                        }

                        return 0;
                    };

                    FOdysseyVectorGroupPaint* inbetweenScene = inbetweenCell->GetEngine()->GetScene();

                    FOdysseyVectorObject* copiedObject = mOwner->Copy( FOdysseyVectorObject::COPY_WORLDCOORDS
                                                                     | FOdysseyVectorObject::COPY_NOTAG
                                                                     , preProcess
                                                                     , postProcess );

                    oAddedObjectList.push_back( copiedObject );

                    if( std::find( oCommittedSceneList.begin()
                                 , oCommittedSceneList.end()
                                 , inbetweenScene ) == oCommittedSceneList.end() )
                    {
                        oCommittedSceneList.push_back( inbetweenScene );
                    }

                    inbetweenScene->AppendChild( copiedObject );

                    inbetweenScene->UpdateMatrix();

                    // convert vertices from world to local coordinates (for consistency with what is seen on the screen)
                    for( FOdysseyVectorObject* newObject : newObjectList )
                    {
                        if( newObject->GetClass() == FOdysseyVectorPath::StaticClass() )
                        {
                            FOdysseyVectorPath* newPath = static_cast<FOdysseyVectorPath*>(newObject);
                            BLMatrix2D& inverseWorldMatrix = newPath->GetInverseWorldMatrix();

                            for( FOdysseyVectorVertex* vertex : newPath->GetVertexList() )
                            {
                                BLPoint vertexLocalPosition = inverseWorldMatrix.mapPoint( vertex->GetX()
                                                                                         , vertex->GetY() );
                                double vertexLocalRadius = FOdysseyVector::MapVector( inverseWorldMatrix
                                                                                    , ::ULIS::FVec2D( vertex->GetRadius() * 0.7071f
                                                                                                    , vertex->GetRadius() * 0.7071f ) ).Distance();

                                vertex->Set( vertexLocalPosition.x, vertexLocalPosition.y );
                                vertex->SetRadius( vertexLocalRadius );
                            }

                            for( FOdysseyVectorSegment* segment : newPath->GetSegmentList() )
                            {
                                if( segment->HasBaseClass( FOdysseyVectorSegmentCubic::StaticClass() ) )
                                {
                                    FOdysseyVectorHandleSegment* handle0 = segment->GetHandle(0);
                                    FOdysseyVectorHandleSegment* handle1 = segment->GetHandle(1);
                                    BLPoint handle0localPosition = inverseWorldMatrix.mapPoint( handle0->GetX()
                                                                                              , handle0->GetY() );
                                    BLPoint handle1localPosition = inverseWorldMatrix.mapPoint( handle1->GetX()
                                                                                              , handle1->GetY() );

                                    handle0->Set( handle0localPosition.x, handle0localPosition.y );
                                    handle1->Set( handle1localPosition.x, handle1localPosition.y );
                                }
                            }
                        }
                    }

                    inbetweenScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
                }
            }
        }
    }

    mOwner->RecursiveRemoveTagByType( FOdysseyVectorTagInbetweener::StaticClass()
                                    , oRemovedTagList );

    RedrawCells();
}

eInbetweenerInterpolationType
FOdysseyVectorTagInbetweener::GetInterpolationType()
{
    return mInterpolationType;
}

void
FOdysseyVectorTagInbetweener::SetInterpolationType( eInbetweenerInterpolationType iInterpolationType )
{
    if( iInterpolationType == eInbetweenerInterpolationType::Linear )
    {
        RemoveAllRoutes();
    }

    mInterpolationType = iInterpolationType;

    Invalidate( INVALIDATE_SPACING
              | INVALIDATE_INTERPOLATIONTYPE
              | INVALIDATE_CELLS );
}
