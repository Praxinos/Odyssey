// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyVectorTagInbetweener.h"
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
    , bSquare ( iNumQuadX == iNumQuadY )
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

    //ResizeFullChartHUD();

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

    // copy breakdowns
    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        uint32 breakdownTargetDrawingIndex = breakdown->GetTargetDrawingIndex();

        if( ( breakdownTargetDrawingIndex > newTagDefaultBreakdown->GetSourceDrawingIndex() )
         && ( breakdownTargetDrawingIndex < newTagDefaultBreakdown->GetTargetDrawingIndex() ) )
        {
            FInbetweenerBreakdown* newBreakdown = newTag->AddBreakdown( breakdown->GetTargetDrawingIndex(), false, false );
            std::vector<::ULIS::FVec2D> targetPositionBuffer;
            std::vector<float> spacingBuffer;

            breakdown->GetGrid()->GetGeometry( targetPositionBuffer, eInbetweenerPointPositionType::TargetPosition );

            newBreakdown->SetTargetTransform( breakdown->GetTargetTranslationX()
                                           ,  breakdown->GetTargetTranslationY()
                                           ,  breakdown->GetTargetRotation()
                                           ,  breakdown->GetTargetScalingX()
                                           ,  breakdown->GetTargetScalingY() );

            newBreakdown->GetGrid()->Make( targetPositionBuffer, true );

            // copy charts
            breakdown->GetChart()->GetSpacing( spacingBuffer );

            for( uint32 i = 0; i < spacingBuffer.size(); i++ )
            {
                FInbetweenerChart::Inbetween& inbetween = newBreakdown->GetChart()->GetInbetweenBuffer()[i];

                inbetween.SetSpacing( spacingBuffer[i] );
            }
        }
    }

    // copy routes/trajectories
    for( FInbetweenerRoute* route : mRouteList )
    {
        FInbetweenerRoute* newRoute = new FInbetweenerRoute( newTag
                                                           , route->GetQuadIndex()
                                                           , route->GetQuadU()
                                                           , route->GetQuadV() );

        newTag->AddRoute( newRoute );

        for( uint32 i = 0; i < newRoute->GetTrajectoryBuffer().size(); i++ )
        {
            FInbetweenerTrajectory& srcTrajectory =    route->GetTrajectoryBuffer()[i];
            FInbetweenerTrajectory& dstTrajectory = newRoute->GetTrajectoryBuffer()[i];

            for( uint32 j = 0; j < dstTrajectory.GetWaypointBuffer().size(); j++ )
            {
                FInbetweenerWaypoint& srcWaypoint = srcTrajectory.GetWaypointBuffer()[j];
                FInbetweenerWaypoint& dstWaypoint = dstTrajectory.GetWaypointBuffer()[j];

                srcWaypoint.SetRatio( dstWaypoint.GetRatio() );
            }

            dstTrajectory.GetHandle(0)->Set( srcTrajectory.GetHandle(0)->GetDirection()
                                           , srcTrajectory.GetHandle(0)->GetLengthRatio() );

            dstTrajectory.GetHandle(1)->Set( srcTrajectory.GetHandle(1)->GetDirection()
                                           , srcTrajectory.GetHandle(1)->GetLengthRatio() );
        }
    }


    return newTag;
}

void
FOdysseyVectorTagInbetweener::Map()
{
    FInbetweenerGrid* firstGrid = mBreakdownList.front()->GetGrid();
    uint32 paintgroupCount = 0;
    uint32 pathCount = 0;

    mInterpolatedPathBuffer.clear();
    mInterpolatedGroupPaintBuffer.clear();
    mInterpolatedObjectArray.clear();

    FOdysseyVectorObject::Traverse
    ( mOwner
    , 0
    , [ this
      , &paintgroupCount
      , &pathCount ]( FOdysseyVectorObject* object, uint64 travesalFlags ) -> uint64
      {
          FOdysseyVectorTag* objectInbetweenerTag = object->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

          if( ( objectInbetweenerTag == nullptr ) || ( objectInbetweenerTag == this ) )
          {
              if( object->GetClass() == FOdysseyVectorPath::StaticClass() )
              {
                  pathCount++;

                  return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED;
              }

              if( object->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
              {
                  paintgroupCount++;

                  return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED;
              }
          }
          else
          {
              return FOdysseyVectorObject::TRAVERSE_OBJECT_IGNORE_CHILDREN;
          }

          return 0;
      } );

    mInterpolatedPathBuffer.reserve( pathCount );
    mInterpolatedGroupPaintBuffer.reserve( paintgroupCount );
    mInterpolatedObjectArray.reserve ( pathCount + paintgroupCount );

    FOdysseyVectorObject::Traverse
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

                  mInterpolatedPathBuffer.emplace_back( this
                                                      , path
                                                      , bMapAsPolyline );

                  mInterpolatedObjectArray.push_back( &mInterpolatedPathBuffer.back() );

                  return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED;
              }

              if( object->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
              {
                  FOdysseyVectorGroupPaint* paintgroup = static_cast<FOdysseyVectorGroupPaint*>(object);

                  mInterpolatedGroupPaintBuffer.emplace_back( this
                                                            , paintgroup );

                  mInterpolatedObjectArray.push_back( &mInterpolatedGroupPaintBuffer.back() );

                  return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED;
              }
          }
          else
          {
              return FOdysseyVectorObject::TRAVERSE_OBJECT_IGNORE_CHILDREN;
          }

          return 0;
      } );

    /* Map on first grid */
    firstGrid->MapInterpolatedObjects();
}

std::vector<FInterpolatedObject*>&
FOdysseyVectorTagInbetweener::GetInterpolatedObjectArray()
{
    return mInterpolatedObjectArray;
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

    // check this quad does not already have a route in this quad
    for( FInbetweenerRoute* route : mRouteList )
    {
        if( route->GetQuadIndex() == quadIndex )
        {
            return nullptr;
        }
    }

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
            route->Fit( 0 );
        }
        // Smooth the first route only. The other route will be fitted via ARAP
        if( mRouteList.size() == 1 )
        {
            route->Smooth();
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
FOdysseyVectorTagInbetweener::ResetLayout( bool iFreeMemNow )
{
    //FInbetweenerBreakdown* newDefaultBreakdown = new FInbetweenerBreakdown( this );
    uint32 currentTargetDrawingIndex = mBreakdownList.back()->GetTargetDrawingIndex();

    std::vector<::ULIS::FVec2D> sourceGeometry;
    std::vector<::ULIS::FVec2D> targetGeometry;

    mBreakdownList.front()->GetGrid()->GetGeometry( sourceGeometry, eInbetweenerPointPositionType::SourcePosition );
    mBreakdownList.back() ->GetGrid()->GetGeometry( targetGeometry, eInbetweenerPointPositionType::TargetPosition );

    mBreakdownList.remove_if( [ this
                              , iFreeMemNow ]( FInbetweenerBreakdown* breakdown )
                              {
                                  breakdown->SetInbetweenerTag( nullptr );

                                  if( iFreeMemNow )
                                  {
                                      delete breakdown;
                                  }

                                  return true;
                              } );

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
        FInbetweenerChart::Inbetween* inbetween = &curBreakdown->GetChart()->GetInbetweenBuffer()[inbetweenIndex];
        FInbetweenerBreakdown* newBreakdown = iNewBreakdown ? iNewBreakdown
                                                            : new FInbetweenerBreakdown( this );
        FInbetweenerChart* curChart = curBreakdown->GetChart();
        FInbetweenerChart* newChart = newBreakdown->GetChart();
        std::vector<float> curChartSpacingBuffer;

        curChart->GetSpacing( curChartSpacingBuffer );

        mOwner->LockDrawing();

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

        // fit chart Full HUD by fitting splitting the quadratic bezier. Must be after the above call to ChainBreakdowns()
        if( iNewBreakdown == nullptr )
        {
            double quadraticT = curChart->GetHUDBezier()->GetQuadraticT( inbetween->GetSpacing() );
            ::ULIS::FVec2D quadratic[2][3] = { { curChart->GetHUDBezier()->GetPoints()[0].GetPosition()
                                               , curChart->GetHUDBezier()->GetPoints()[1].GetPosition()
                                               , curChart->GetHUDBezier()->GetPoints()[2].GetPosition() }
                                             , { curChart->GetHUDBezier()->GetPoints()[0].GetPosition()
                                               , curChart->GetHUDBezier()->GetPoints()[1].GetPosition()
                                               , curChart->GetHUDBezier()->GetPoints()[2].GetPosition() } };

            ::ULIS::QuadraticBezierSplitAtParameter       ( &quadratic[0][0]
                                                          , &quadratic[0][1]
                                                          , &quadratic[0][2]
                                                          , quadraticT );

            ::ULIS::QuadraticBezierInverseSplitAtParameter( &quadratic[1][0]
                                                          , &quadratic[1][1]
                                                          , &quadratic[1][2]
                                                          , quadraticT );

            newChart->GetHUDBezier()->GetPoints()[0].SetPosition( quadratic[0][0].x, quadratic[0][0].y );
            newChart->GetHUDBezier()->GetPoints()[1].SetPosition( quadratic[0][1].x, quadratic[0][1].y );
            newChart->GetHUDBezier()->GetPoints()[2].SetPosition( quadratic[0][2].x, quadratic[0][2].y );

            curChart->GetHUDBezier()->GetPoints()[0].SetPosition( quadratic[1][0].x, quadratic[1][0].y );
            curChart->GetHUDBezier()->GetPoints()[1].SetPosition( quadratic[1][1].x, quadratic[1][1].y );
            curChart->GetHUDBezier()->GetPoints()[2].SetPosition( quadratic[1][2].x, quadratic[1][2].y );
        }

        // Note: this will also alloc memory for trajectories etc...
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
            newChart->GetInbetweenBuffer()[i].SetSpacing( newSpacing );
        }

        // adapt the new spacings for the current breakdown
        for( uint32 i = inbetweenIndex + 1, j = 1; i < (uint32) curChartSpacingBuffer.size() - 1; i++, j++ )
        {
            float newSpacing = ( curChartSpacingBuffer[i] - curChartSpacingBuffer[inbetweenIndex] )
                             / ( 1.0f - curChartSpacingBuffer[inbetweenIndex] );
            // Note: inbetweenIndex cannot be 0
            curChart->GetInbetweenBuffer()[j].SetSpacing( newSpacing );
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

        mOwner->UnlockDrawing();

        // Invalidation might trigger a redrawing. It must be done outside the mutex locking mechanism
        // because redrawing will also lock the mutex.
        Invalidate( INVALIDATE_BREAKDOWN_LIST );

        return newBreakdown;
    }
    else
    {
        FInbetweenerBreakdown* newBreakdown = iNewBreakdown ? iNewBreakdown
                                                            : new FInbetweenerBreakdown( this );

        mOwner->LockDrawing();

        newBreakdown->SetInbetweenerTag( this );

        mBreakdownList.push_back( newBreakdown );

        ChainBreakdowns();

        // Note: this will also alloc memory for trajectories etc...
        newBreakdown->SetTargetDrawingIndex( iDrawingIndex );

        // Invalidation might trigger a redrawing. It must be done outside the mutex locking mechanism
        // because redrawing will also lock the mutex.
        Invalidate( INVALIDATE_BREAKDOWN_LIST );

        mOwner->UnlockDrawing();

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
    uint32 tagCellIndex = mScene->GetCell()->GetIndex();
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
    mOwner->LockDrawing();

    if( mBreakdownList.size() > 1 )
    {
        // note: it is guaranteed that we wil have at least a previous or next breakdown
        FInbetweenerBreakdown* nextBreakdown = iBreakdown->GetNextBreakdown();
        FInbetweenerBreakdown* prevBreakdown = iBreakdown->GetPrevBreakdown();
        uint32 removedBreakdownDrawingCount = iBreakdown->GetDrawingCount();
        uint32 nextBreakdownDrawingCount = nextBreakdown ? nextBreakdown->GetDrawingCount() : 0;

        mBreakdownList.remove_if( [iBreakdown]( FInbetweenerBreakdown* listedBreakdown )
                                    {
                                        return ( iBreakdown == listedBreakdown ) ? true : false;
                                    } );

        iBreakdown->SetInbetweenerTag( nullptr );

        ChainBreakdowns();

        if( nextBreakdown )
        {
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
            // and resizing of the routes
            nextBreakdown->SetTargetDrawingIndex( nextBreakdown->GetTargetDrawingIndex() );

            // Adapt the spacings
            for( i = 1, newInbetweenIndex = 1; i < removedBreakdownDrawingCount - 1; i++, newInbetweenIndex++ )
            {
                nextBreakdown->GetChart()->GetInbetweenBuffer()[newInbetweenIndex].SetSpacing( removedBreakdownSpacing[i] * leftRatio );
            }

            nextBreakdown->GetChart()->GetInbetweenBuffer()[newInbetweenIndex++].SetSpacing( leftRatio );

            for( i = 1                       ; i < nextBreakdownDrawingCount    - 1; i++, newInbetweenIndex++ )
            {
                nextBreakdown->GetChart()->GetInbetweenBuffer()[newInbetweenIndex].SetSpacing( leftRatio + ( nextBreakdownSpacing[i] * rightRatio ) );
            }
            // ------------------

            iBreakdown->GetGrid()->GetGeometry( breakdownSourceGeometry, eInbetweenerPointPositionType::SourcePosition );

            // nextBreakdown source grid gets its shape from this removed breakdown source grid.
            nextBreakdown->GetGrid()->SetGeometry( breakdownSourceGeometry, eInbetweenerPointPositionType::SourcePosition, true );
        }
        else
        {
            // This will force reallocation of the chart and dispatching of drawings
            // and resizing of the routes
            prevBreakdown->SetTargetDrawingIndex( prevBreakdown->GetTargetDrawingIndex() );
        }
    }

    mOwner->UnlockDrawing();

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

void
FOdysseyVectorTagInbetweener::GetBreakdownArray( std::vector<FInbetweenerBreakdown*>& oBreakdownArray )
{
    oBreakdownArray.reserve( mBreakdownList.size() );

    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        oBreakdownArray.push_back( breakdown );
    }
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
    mSharedEnv = mOwner->GetLayer();

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
    mSharedEnv = mOwner->GetLayer();

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
        if( ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_HIERARCHY      )
         || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_SHAPE          )
         || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_TOPOLOGY       )
         || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_TAG_LIST )
         || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_TOPOLOGY )
         || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_SHAPE    )
         || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_MATRIX   ) )
        {
            for( FInbetweenerBreakdown* breakdown : mBreakdownList )
            {
                std::vector<::ULIS::FVec2D> targetGeometry;

                breakdown->GetGrid()->GetGeometry( targetGeometry, eInbetweenerPointPositionType::TargetPosition );
                // Note: we cannot call Invalidate in Make() (hence the "false" arg), so we set the flags manually.
                breakdown->GetGrid()->Make( targetGeometry, false );

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
            || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_COLOR          ) // for buckets
            || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_TAG_LIST )
            || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_TOPOLOGY )
            || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_SHAPE    )
            || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_COLOR    ) ) // for buckets
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

            DeformObjectsAtSource();
        }

        // will update grids' center of mass (needed for interpolation).
        // MUST be done after mapping because mapping will elimniate some quads, and this is taken into account
        // for the center of mass.
        for( FInbetweenerBreakdown* breakdown : mBreakdownList )
        {
            FInbetweenerChart::Inbetween* inbetween = &breakdown->GetChart()->GetInbetweenBuffer().back();

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
                breakdown->GetGrid()->DeformObjects( inbetween
                                                   , eInbetweenerPointPositionType::TargetPosition );
            }
        }

        // Update chart HUDs
        if( mInvalidationFlags & INVALIDATE_CHARTHUD )
        {
            // Per-breakdown chart
            for( FInbetweenerBreakdown* breakdown : mBreakdownList )
            {
                if( breakdown->GetChart()->GetHUDBezier()->IsInvalidated() )
                {
                    breakdown->GetChart()->GetHUDBezier()->Update();
                }
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

FOdysseyVectorCell*
FOdysseyVectorTagInbetweener::GetCell()
{
    return mOwner->GetCell();
}

FOdysseyVectorCell*
FOdysseyVectorTagInbetweener::GetSourceCell()
{
    FOdysseyVectorLayer* layer = GetOwner()->GetLayer();

    if( layer )
    {
        uint32 sourceCellIndex = GetSourceCellIndex();

        return layer->GetCellByIndex( sourceCellIndex );
    }

    return nullptr;
}

int32
FOdysseyVectorTagInbetweener::GetSourceCellIndex()
{
    return mBreakdownList.front()->GetSourceCellIndex();
}

FOdysseyVectorCell*
FOdysseyVectorTagInbetweener::GetTargetCell()
{
    FOdysseyVectorLayer* layer = GetOwner()->GetLayer();

    if( layer )
    {
        uint32 targetCellIndex = GetTargetCellIndex();

        return layer->GetCellByIndex( targetCellIndex );
    }

    return nullptr;
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

    //ResizeFullChartHUD();
}

void
FOdysseyVectorTagInbetweener::ResizeFullChartHUD()
{
/*
    uint32 currentsize = mFullChartHUDBezierBuffer.size();
    ::ULIS::FVec2D startAt = currentsize ? mFullChartHUDBezierBuffer.back().GetPoints()[2].GetPosition()
                                         : ::ULIS::FVec2D( FInbetweenerChart::DEFAULT_POSITION_P0_X
                                                         , FInbetweenerChart::DEFAULT_POSITION_P0_Y );

    mFullChartHUDBezierBuffer.resize( GetBreakdownCount(), this );

    for( uint32 i = currentsize; i < mFullChartHUDBezierBuffer.size(); i++ )
    {
        FInbetweenerChart::HUDBezier& quadraticBezier = mFullChartHUDBezierBuffer[i];

        quadraticBezier.GetPoints()[0].SetPosition( startAt.x
                                                  , startAt.y );

        quadraticBezier.GetPoints()[1].SetPosition( startAt.x + ( FInbetweenerChart::DEFAULT_POSITION_P1_X - FInbetweenerChart::DEFAULT_POSITION_P0_X )
                                                  , startAt.y );

        quadraticBezier.GetPoints()[2].SetPosition( startAt.x + ( FInbetweenerChart::DEFAULT_POSITION_P2_X - FInbetweenerChart::DEFAULT_POSITION_P0_X )
                                                  , startAt.y );

        startAt = quadraticBezier.GetPoints()[2].GetPosition();
    }
*/
}

void
FOdysseyVectorTagInbetweener::UpdateMatrix()
{
    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        breakdown->UpdateMatrix();
    }
}

void
FOdysseyVectorTagInbetweener::DeformObjectsAtSource()
{
    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        FInbetweenerChart::Inbetween* inbetween = &breakdown->GetChart()->GetInbetweenBuffer().front();

        // deform the path according to grid geometry
        breakdown->GetGrid()->DeformObjects( inbetween
                                         , eInbetweenerPointPositionType::SourcePosition );

        // we do it only for the first breakdown
        break;
    }
}
/*
void
FOdysseyVectorTagInbetweener::DeformObjectsAtTarget()
{
    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        FInbetweenerChart::Inbetween* inbetween = &breakdown->GetChart()->GetInbetweenBuffer().back();

        // deform the path according to grid geometry
        breakdown->GetGrid()->DeformObjects( inbetween
                                         , eInbetweenerPointPositionType::TargetPosition );
    }
}
*/
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

                breakdown->GetChart()->GetInbetweenBuffer()[chartDivisionIndex].SetDrawing( &mDrawingBuffer[i] );
            }
        }
    }
}

void
FOdysseyVectorTagInbetweener::DeformGridAtInbetween( FInbetweenerChart::Inbetween *iInbetween )
{
    double t = iInbetween->GetSpacing();

    if( mInterpolationType == eInbetweenerInterpolationType::Linear )
    {
        for( FInbetweenerPoint& point : iInbetween->GetChart()->GetBreakdown()->GetGrid()->GetPointBuffer() )
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
            iInbetween->GetChart()->GetBreakdown()->GetGrid()->ComputeARAPInterpolation( iInbetween, false );
        }
    }
}

void
FOdysseyVectorTagInbetweener::DeformObjectsAtInbetween( FInbetweenerChart::Inbetween *iInbetween )
{
    double t = iInbetween->GetSpacing();

    DeformGridAtInbetween( iInbetween );

    // deform the path according to grid geometry
    iInbetween->GetChart()->GetBreakdown()->GetGrid()->DeformObjects( iInbetween
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
    if( mScene->GetLayer() )
    {
        FOdysseyVectorCell* cell = mScene->GetCell();
        FOdysseyVectorLayer* layer = mScene->GetLayer();

        if( cell )
        {
            int32 cellIndex = cell->GetIndex();

            // Redraw impacted cells
            for( uint32 i = 1; ( i < iDrawingCount ) && ( cell != nullptr ); i++ )
            {
                FInbetweenerDrawing* drawing = GetDrawing( i );
                int32 inbetweenCellIndex = drawing->GetCellIndex();
                FOdysseyVectorCell* nextCell = layer->GetCellByIndex( inbetweenCellIndex );

                if( nextCell )
                {
                    layer->InvalidateCell( nextCell );
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
            FInbetweenerChart::Inbetween* inbetween = &breakdown->GetChart()->GetInbetweenBuffer()[i];

            DeformObjectsAtInbetween( inbetween );
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
                                  , FOdysseyVectorEngine* iEngine
                                  , const ::ULIS::FRectD& iInvalidationArea
                                  , double iAncestorsOpacity
                                  , uint64 iDrawingFlags )
{

}

// when drawn as a shared tag
void
FOdysseyVectorTagInbetweener::Draw( FOdysseyVectorGroupPaint* iDisplayedScene
                                  , BLContext* iBLContext
                                  , FOdysseyVectorEngine* iEngine
                                  , const ::ULIS::FRectD& iInvalidationArea
                                  , double iAncestorsOpacity
                                  , uint64 iDrawingFlags )
{
    FOdysseyVectorCell* displayedCell = iDisplayedScene->GetCell();

    // check the object is still displayed (it could have been removed but still in memory)
    if( /*mOwner->GetScene()*/ mOwner->IsVisible( true ) )
    {

        FOdysseyVectorCell* tagCell = mOwner->GetScene()->GetCell();

        iBLContext->save();
        iBLContext->resetMatrix();

        // if th eobject hasn't been removed from the scene
        if( displayedCell && tagCell )
        {
            int32 sourceCellIndex = tagCell->GetIndex();
            // Note: target can be negative
            int32 targetCellIndex = sourceCellIndex + ( ( GetLength() - 1 ) * (int)mInterpolationDirection );
            int32 displayedCellIndex = displayedCell->GetIndex();
            int32 fromCellIndex = std::min( sourceCellIndex, targetCellIndex );
            int32   toCellIndex = std::max( sourceCellIndex, targetCellIndex );

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
                        FInbetweenerChart::Inbetween* inbetween = &breakdown->GetChart()->GetInbetweenBuffer()[inbetweenIndex];

                        DrawPathsInbetween( iDisplayedScene
                                          , inbetween
                                          , iBLContext
                                          , iEngine );
                    }
                }
            }
        }

        iBLContext->restore();
    }
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
                                        , FInbetweenerChart::Inbetween* iInbetween
                                        , FInterpolatedPath* iInterpolatedPath
                                        , BLContext* iBLContext
                                        , FOdysseyVectorEngine* iEngine )
{
    uint32 pointCount = iInterpolatedPath->GetInterpolatedPointBuffer().size();
    uint32 inbetweenAbsoluteIndex = iInbetween->GetIndexInInbetweener();
    FInterpolatedPath::PointGeometry* interpolatedPointGeometryBuffer = &iInterpolatedPath->GetInterpolatedPointGeometryBuffer()[pointCount * inbetweenAbsoluteIndex];
    float scalingSq = bConstantWidth ? 1.0f / ( iInbetween->GetDrawing()->scalingX
                                              * iInbetween->GetDrawing()->scalingY ) : 1.0f;
    // note: a surface grows or shrink at the square of the scaling factor.
    // That's why we use sqrt to get the actual scaling factor from the surface ratio.
    float scaling = sqrt( scalingSq );
    FOdysseyVectorBrush& brush = iInterpolatedPath->GetOriginalPath()->GetBrush();
    FOdysseyVectorPath* originalPath = iInterpolatedPath->GetOriginalPath();
    FColor pathColor = originalPath->GetForegroundColor();
    BLMatrix2D worldMatrix = mOwner->GetWorldMatrix();

    // passed to DrawPathAt()
    worldMatrix.transform( iInbetween->GetDrawing()->localMatrix );
    worldMatrix.transform( iInterpolatedPath->mRelativeMatrix );

    iBLContext->setStrokeStyle( BLRgba32( pathColor.R, pathColor.G, pathColor.B, pathColor.A ) );
    iBLContext->setFillStyle( BLRgba32( pathColor.R, pathColor.G, pathColor.B, pathColor.A ) );

    brush.Lock();

    // object is already locked when tag::Draw is called, we only lock/unlock if it's not the same object
    if( iInterpolatedPath->GetOriginalPath() != mOwner )
    {
        iInterpolatedPath->GetOriginalPath()->LockDrawing();
    }

    if( bMapAsPolyline )
    {
        for( FInterpolatedSegment& interpolatedSegment : iInterpolatedPath->GetInterpolatedSegmentBuffer() )
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
                ::ULIS::FVec2D* localPointPositionp = pointp ? &interpolatedPointGeometryBuffer[pointp->mIndex].position : nullptr;
                ::ULIS::FVec2D* localPointPositioni = &interpolatedPointGeometryBuffer[pointi->mIndex].position;
                ::ULIS::FVec2D* localPointPositionn = &interpolatedPointGeometryBuffer[pointn->mIndex].position;
                ::ULIS::FVec2D* localPointPositionq = pointq ? &interpolatedPointGeometryBuffer[pointq->mIndex].position : nullptr;

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
                        iEngine->FillQuad( iBLContext
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

                        iEngine->FillQuad( iBLContext
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
    }

    if( bMapAsPolyline == false )
    {
        for( FInterpolatedSegment& interpolatedSegment : iInterpolatedPath->GetInterpolatedSegmentBuffer() )
        {
            if( interpolatedSegment.GetOriginalSegment()->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
            {
                FInterpolatedPoint* interpolatedPoint[4] = { interpolatedSegment.mInterpolatedPointArray[0]
                                                           , interpolatedSegment.mInterpolatedPointArray[1]
                                                           , interpolatedSegment.mInterpolatedPointArray[2]
                                                           , interpolatedSegment.mInterpolatedPointArray[3] };
                FOdysseyVectorVertex* vertex0 = static_cast<FOdysseyVectorVertex*>(interpolatedPoint[0]->GetOriginalPoint());
                FOdysseyVectorVertex* vertex1 = static_cast<FOdysseyVectorVertex*>(interpolatedPoint[3]->GetOriginalPoint());
                FOdysseyVectorHandleSegment* handle0 = static_cast<FOdysseyVectorHandleSegment*>(interpolatedPoint[1]->GetOriginalPoint());
                FOdysseyVectorHandleSegment* handle1 = static_cast<FOdysseyVectorHandleSegment*>(interpolatedPoint[2]->GetOriginalPoint());
                FOdysseyVectorSegment* segment = interpolatedSegment.GetOriginalSegment();
                FOdysseyVectorSegment* prevSegment = vertex0->GetOtherSegment( segment );
                FOdysseyVectorSegment* nextSegment = vertex1->GetOtherSegment( segment );

                vertex0->SetCoordsSilent( interpolatedPointGeometryBuffer[interpolatedPoint[0]->mIndex].position );
                handle0->SetCoordsSilent( interpolatedPointGeometryBuffer[interpolatedPoint[1]->mIndex].position );
                handle1->SetCoordsSilent( interpolatedPointGeometryBuffer[interpolatedPoint[2]->mIndex].position );
                vertex1->SetCoordsSilent( interpolatedPointGeometryBuffer[interpolatedPoint[3]->mIndex].position );

                interpolatedSegment.GetOriginalSegment()->Update( FOdysseyVectorObject::UPDATE_NOINVALIDATERECT );

                vertex0->Update( prevSegment, segment, FOdysseyVectorObject::UPDATE_NOINVALIDATERECT );
                vertex1->Update( segment, nextSegment, FOdysseyVectorObject::UPDATE_NOINVALIDATERECT );
            }
        }

        iBLContext->save();
        iBLContext->setMatrix( worldMatrix );
        for( FOdysseyVectorChain& chain : iInterpolatedPath->GetOriginalPath()->GetChainArray() )
        {
            iInterpolatedPath->GetOriginalPath()->DrawChain( iBLContext
                                                           , iEngine
                                                           , 1.0f/*double iCombinedOpacity*/
                                                           , chain
                                                           , 0 );
        }
        iBLContext->restore();

        // restore
        for( FInterpolatedSegment& interpolatedSegment : iInterpolatedPath->GetInterpolatedSegmentBuffer() )
        {
            if( interpolatedSegment.GetOriginalSegment()->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
            {
                FInterpolatedPoint* interpolatedPoint[4] = { interpolatedSegment.mInterpolatedPointArray[0]
                                                           , interpolatedSegment.mInterpolatedPointArray[1]
                                                           , interpolatedSegment.mInterpolatedPointArray[2]
                                                           , interpolatedSegment.mInterpolatedPointArray[3] };
                FOdysseyVectorVertex* vertex0 = static_cast<FOdysseyVectorVertex*>(interpolatedPoint[0]->GetOriginalPoint());
                FOdysseyVectorVertex* vertex1 = static_cast<FOdysseyVectorVertex*>(interpolatedPoint[3]->GetOriginalPoint());
                FOdysseyVectorHandleSegment* handle0 = static_cast<FOdysseyVectorHandleSegment*>(interpolatedPoint[1]->GetOriginalPoint());
                FOdysseyVectorHandleSegment* handle1 = static_cast<FOdysseyVectorHandleSegment*>(interpolatedPoint[2]->GetOriginalPoint());
                FOdysseyVectorSegment* segment = interpolatedSegment.GetOriginalSegment();
                FOdysseyVectorSegment* prevSegment = vertex0->GetOtherSegment( segment );
                FOdysseyVectorSegment* nextSegment = vertex1->GetOtherSegment( segment );

                vertex0->SetCoordsSilent( interpolatedPoint[0]->GetOriginalCoords() );
                handle0->SetCoordsSilent( interpolatedPoint[1]->GetOriginalCoords());
                handle1->SetCoordsSilent( interpolatedPoint[2]->GetOriginalCoords() );
                vertex1->SetCoordsSilent( interpolatedPoint[3]->GetOriginalCoords() );

                interpolatedSegment.GetOriginalSegment()->Update( FOdysseyVectorObject::UPDATE_NOINVALIDATERECT );

                vertex0->Update( prevSegment, segment, FOdysseyVectorObject::UPDATE_NOINVALIDATERECT );
                vertex1->Update( segment, nextSegment, FOdysseyVectorObject::UPDATE_NOINVALIDATERECT );
            }
        }
    }

    brush.Unlock();

    // object is already locked when tag::Draw is called, we only lock/unlock if it's not the same object
    if( iInterpolatedPath->GetOriginalPath() != mOwner )
    {
        iInterpolatedPath->GetOriginalPath()->UnlockDrawing();
    }
}

std::vector<FInterpolatedPath>&
FOdysseyVectorTagInbetweener::GetInterpolatedPathBuffer()
{
    return mInterpolatedPathBuffer;
}

std::vector<FInterpolatedGroupPaint>&
FOdysseyVectorTagInbetweener::GetInterpolatedGroupPaintBuffer()
{
    return mInterpolatedGroupPaintBuffer;
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
                                                , FInbetweenerChart::Inbetween* inbetween
                                                , BLContext* iBLContext
                                                , FOdysseyVectorEngine* iEngine )
{
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
                  , iEngine );
    }

    iBLContext->restore();
}

void
FOdysseyVectorTagInbetweener::MoveInbetween( FInbetweenerChart::Inbetween* iInbetween
                                           , float iNewSpacing
                                           , bool iRelative )
{
    FInbetweenerBreakdown* breakdown = iInbetween->GetChart()->GetBreakdown();

    if( ( iNewSpacing > 0.0f ) && ( iNewSpacing < 1.0f ) )
    {
        FOdysseyVectorCell* animationCell = mOwner->GetCell();
        FOdysseyVectorLayer* animationLayer = mOwner->GetLayer();
        int32  prevIndex  = iInbetween->GetIndex() - 1;
        uint32 nextIndex  = iInbetween->GetIndex() + 1;
        float prevSpacing = iInbetween->GetChart()->GetInbetweenBuffer()[prevIndex].GetSpacing();
        float nextSpacing = iInbetween->GetChart()->GetInbetweenBuffer()[nextIndex].GetSpacing();

        if( iRelative == false )
        {
            if( ( iNewSpacing > prevSpacing )
             && ( iNewSpacing < nextSpacing ) )
            {
                uint32 inbetweenCellIndex = iInbetween->GetCellIndex();
                FOdysseyVectorCell* inbetweenCell = GetOwner()->GetLayer()->GetCellByIndex( inbetweenCellIndex );

                iInbetween->SetSpacing( iNewSpacing );

                if( inbetweenCell )
                {
                   animationLayer->InvalidateCell( inbetweenCell );
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
                FInbetweenerChart::Inbetween* otherInbetween = &breakdown->GetChart()->GetInbetweenBuffer()[i];

                if( otherInbetween != iInbetween )
                {
                    double otherInbetweenOldSpacing = otherInbetween->GetSpacing();

                    if( otherInbetween->GetSpacing() < iInbetween->GetSpacing() )
                    {
                        float length = iInbetween->GetSpacing();
                        float ratio = iInbetween->GetSpacing() ? ( otherInbetween->GetSpacing() / length ) : 0.0f;
                        float newLength = iNewSpacing;

                        otherInbetween->SetSpacing( newLength * ratio );
                    }
                    else
                    {
                        float length = 1.0f - iInbetween->GetSpacing();
                        float ratio = iInbetween->GetSpacing() ? ( ( otherInbetween->GetSpacing() - iInbetween->GetSpacing() ) / length ) : 0.0f;
                        float newLength = 1.0f - iNewSpacing;

                        otherInbetween->SetSpacing( iNewSpacing + ( newLength * ratio ) );
                    }
                }
            }

            iInbetween->SetSpacing( iNewSpacing );

            // recompute all inbetweens
            //Interpolate();
        }
    }

    Invalidate( INVALIDATE_ROUTES | INVALIDATE_SPACING );
}



void
FOdysseyVectorTagInbetweener::AllocBuffers()
{
    for( FInterpolatedObject* interpolatedObject : mInterpolatedObjectArray )
    {
        uint32 pointCount = interpolatedObject->GetInterpolatedPointBuffer().size();

        // Note: the position at the first drawing will not be used
        interpolatedObject->GetInterpolatedPointGeometryBuffer().resize( GetLength() * pointCount );
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
    iGridNumQuadY = iSquare ? iGridNumQuadX : iGridNumQuadY;

    if( ( mGridNumQuadX != iGridNumQuadX )
     || ( mGridNumQuadY != iGridNumQuadY )
     || ( iSquare != bSquare ) )
    {
        RemoveAllRoutes();
    }

    mGridType = iGridType;
    mGridNumQuadX = iGridNumQuadX;
    mGridNumQuadY = iGridNumQuadY;
    bSquare = iSquare;

    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        breakdown->SetGrid( iGridType );
        breakdown->GetGrid()->Make( true );
    }

    Invalidate( INVALIDATE_MAP
              | INVALIDATE_SPACING
              | INVALIDATE_GRIDTYPE
              | INVALIDATE_CELLS );
}

void
FOdysseyVectorTagInbetweener::SetGridNumQuad( uint32 iGridNumQuadX
                                            , uint32 iGridNumQuadY
                                            , bool iSquare
                                            , const std::vector<::ULIS::FVec2D>& iTargetPositionBuffer )
{
    iGridNumQuadY = iSquare ? iGridNumQuadX : iGridNumQuadY;

    if( ( mGridNumQuadX != iGridNumQuadX ) || ( mGridNumQuadY != iGridNumQuadY ) )
    {
        RemoveAllRoutes();
    }

    mGridNumQuadX = iGridNumQuadX;
    mGridNumQuadY = iGridNumQuadY;
    bSquare = iSquare;

    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        breakdown->GetGrid()->Make( iTargetPositionBuffer
                                  , true );
    }

    Invalidate( INVALIDATE_MAP
              | INVALIDATE_SPACING
              | INVALIDATE_CELLS );
}

void
FOdysseyVectorTagInbetweener::SetGridNumQuad( uint32 iGridNumQuadX
                                            , uint32 iGridNumQuadY
                                            , bool iSquare )
{
    iGridNumQuadY = iSquare ? iGridNumQuadX : iGridNumQuadY;

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

    Invalidate( INVALIDATE_MAP
              | INVALIDATE_SPACING
              | INVALIDATE_CELLS );
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
    FOdysseyVectorCell* cell = mOwner->GetCell();
    FOdysseyVectorLayer* layer = mOwner->GetLayer();
    int32 animationCellIndex = cell->GetIndex();

    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        uint32 breakdownSourceDrawingIndex = breakdown->GetSourceDrawingIndex();
        uint32 breakdownTargetDrawingIndex = breakdown->GetTargetDrawingIndex();

        for( uint32 drawingIndex = breakdownSourceDrawingIndex + 1; drawingIndex <= breakdownTargetDrawingIndex; drawingIndex++ )
        {
            FOdysseyVectorCell* inbetweenCell = layer->GetCellByIndex( animationCellIndex + ( drawingIndex * (int)mInterpolationDirection ) );

            if( ( drawingIndex != breakdownTargetDrawingIndex ) || breakdown->IsTargetVisible() )
            {
                //FInbetweenerDrawing* drawing = GetDrawing( drawingIndex );

                if( inbetweenCell )
                {
                    std::list<FOdysseyVectorObject*> newObjectList;

                    // change vertices coords before copying the object
                    std::function<uint64(FOdysseyVectorObject*,uint64)> preProcess = [ drawingIndex ]( FOdysseyVectorObject* vectorObject, uint64 copyFlags ) -> uint64
                    {
                        FOdysseyVectorTag* tag = vectorObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

                        if( tag )
                        {
                            FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);
                            FInbetweenerDrawing* drawing = inbetweenerTag->GetDrawing( drawingIndex );
                            float scalingSq = inbetweenerTag->HasConstantWidth() ? 1.0f / ( drawing->scalingX
                                                                                          * drawing->scalingY ) : 1.0f;
                            // note: a surface grows or shrink at the square of the scaling factor.
                            // That's why we use sqrt to get the actual scaling factor from the surface ratio.
                            float scaling = sqrt( scalingSq );

                            if( inbetweenerTag->GetMapAsPolyline() )
                            {
                                copyFlags |= FOdysseyVectorObject::COPY_RETOPOLOGY;
                            }
                            else
                            {
                                copyFlags &= (~FOdysseyVectorObject::COPY_RETOPOLOGY);
                            }

                            for( FInterpolatedGroupPaint& interpolatedGroupPaint : inbetweenerTag->mInterpolatedGroupPaintBuffer )
                            {
                                BLMatrix2D commitWorldMatrix = inbetweenerTag->GetOwner()->GetWorldMatrix();
                                uint32 pointCount = interpolatedGroupPaint.GetInterpolatedPointBuffer().size();
                                uint32 skippedOffset = ( drawing->GetIndex() * pointCount );
                                FOdysseyVectorGroupPaint* paintgroup = interpolatedGroupPaint.GetOriginalGroupPaint();

                                commitWorldMatrix.transform( drawing->localMatrix );
                                commitWorldMatrix.transform( interpolatedGroupPaint.GetRelativeMatrix() );

                                for( uint32 i = 0; i < interpolatedGroupPaint.GetInterpolatedPointBuffer().size(); i++ )
                                {
                                    FInterpolatedPoint* interpolatedPoint = &interpolatedGroupPaint.GetInterpolatedPointBuffer()[i];
                                    FOdysseyVectorPoint* originalPoint = interpolatedPoint->GetOriginalPoint();
                                    ::ULIS::FVec2D* commitPosition = &interpolatedGroupPaint.GetInterpolatedPointGeometryBuffer()[skippedOffset + i].position;
                                    ::ULIS::FVec2D worldBucketPosition = FOdysseyVector::MapPoint( commitWorldMatrix, *commitPosition );
                                    ::ULIS::FVec2D localBucketPosition = FOdysseyVector::MapPoint( paintgroup->GetInverseWorldMatrix(), worldBucketPosition );

                                    if( originalPoint->GetClass() == FOdysseyVectorBucket::StaticClass() )
                                    {
                                        FOdysseyVectorBucket* originalBucket = static_cast<FOdysseyVectorBucket*>(originalPoint);
                                        ::ULIS::FVec2D originalBucketPosition = originalBucket->GetCoords();

                                        originalBucket->Set( localBucketPosition );

                                        *commitPosition = originalBucketPosition;
                                    }
                                }
                            }

                            for( FInterpolatedPath& interpolatedPath : inbetweenerTag->mInterpolatedPathBuffer )
                            {
                                BLMatrix2D pathWorldMatrix = inbetweenerTag->GetOwner()->GetWorldMatrix();
                                uint32 pointCount = interpolatedPath.GetInterpolatedPointBuffer().size();
                                uint32 skippedOffset = ( drawing->GetIndex() * pointCount );
                                FOdysseyVectorPath* path = interpolatedPath.GetOriginalPath();
                                double translationX, translationY, rotation, scalingX, scalingY;
                                BLMatrix2D pathLocalMatrix = path->GetParent()->GetInverseWorldMatrix();

                                // save transformations. Will be restored in post-processing
                                path->GetTransform( interpolatedPath.commitTranslationX
                                                  , interpolatedPath.commitTranslationY
                                                  , interpolatedPath.commitRotation
                                                  , interpolatedPath.commitScalingX
                                                  , interpolatedPath.commitScalingY );

                                pathWorldMatrix.transform( drawing->localMatrix );
                                pathWorldMatrix.transform( interpolatedPath.GetRelativeMatrix() );

                                pathLocalMatrix.transform( pathWorldMatrix );

                                FOdysseyVector::ExtractTransformations( pathLocalMatrix
                                                                      , &translationX
                                                                      , &translationY
                                                                      , &rotation
                                                                      , &scalingX
                                                                      , &scalingY
                                                                      , true );
                                path->SetTransform( translationX, translationY, rotation, scalingX, scalingY );
                                //path->UpdateMatrix();

                                for( uint32 i = 0; i < interpolatedPath.GetInterpolatedPointBuffer().size(); i++ )
                                {
                                    FInterpolatedPoint* interpolatedPoint = &interpolatedPath.GetInterpolatedPointBuffer()[i];
                                    FOdysseyVectorPoint* originalPoint = interpolatedPoint->GetOriginalPoint();
                                    ::ULIS::FVec2D* commitPosition = &interpolatedPath.GetInterpolatedPointGeometryBuffer()[skippedOffset + i].position;
                                    ::ULIS::FVec2D swapPosition = interpolatedPoint->mOriginalPoint->GetCoords();

                                    if( originalPoint->GetClass() == FOdysseyVectorVertex::StaticClass() )
                                    {
                                        FOdysseyVectorVertex* originalVertex = static_cast<FOdysseyVectorVertex*>(originalPoint);
                                        double* commitRadius = &interpolatedPath.GetInterpolatedPointGeometryBuffer()[skippedOffset + i].radius;
                                        double swapRadius = originalVertex->GetRadius();

                                        originalVertex->SetRadius( originalVertex->GetRadius() * scaling );

                                        *commitRadius = swapRadius;
                                    }

                                    interpolatedPoint->mOriginalPoint->Set( commitPosition->x
                                                                          , commitPosition->y );

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


                            // revert buckets coords after having copied the object.
                            // Coords were saved in the point position buffer
                            for( FInterpolatedGroupPaint& interpolatedGroupPaint : inbetweenerTag->mInterpolatedGroupPaintBuffer )
                            {
                                uint32 pointCount = interpolatedGroupPaint.GetInterpolatedPointBuffer().size();
                                uint32 skippedOffset = ( drawing->GetIndex() * pointCount );
                                FOdysseyVectorGroupPaint* paintgroup = interpolatedGroupPaint.GetOriginalGroupPaint();

                                for( uint32 i = 0; i < interpolatedGroupPaint.GetInterpolatedPointBuffer().size(); i++ )
                                {
                                    FInterpolatedPoint* interpolatedPoint = &interpolatedGroupPaint.GetInterpolatedPointBuffer()[i];
                                    FOdysseyVectorPoint* originalPoint = interpolatedPoint->GetOriginalPoint();
                                    ::ULIS::FVec2D* commitPosition = &interpolatedGroupPaint.GetInterpolatedPointGeometryBuffer()[skippedOffset + i].position;

                                    if( originalPoint->GetClass() == FOdysseyVectorBucket::StaticClass() )
                                    {
                                        FOdysseyVectorBucket* originalBucket = static_cast<FOdysseyVectorBucket*>(originalPoint);

                                        originalBucket->Set( *commitPosition );
                                    }
                                }
                            }

                            // revert vertices coords after having copied the object.
                            // Coords were saved in the point position buffer
                            for( FInterpolatedPath& interpolatedPath : inbetweenerTag->mInterpolatedPathBuffer )
                            {
                                uint32 pointCount = interpolatedPath.GetInterpolatedPointBuffer().size();
                                uint32 skippedOffset = ( drawingIndex * pointCount );
                                FOdysseyVectorPath* path = interpolatedPath.GetOriginalPath();

                                // restore transformations that were changed in the pre-process
                                path->SetTransform( interpolatedPath.commitTranslationX
                                                  , interpolatedPath.commitTranslationY
                                                  , interpolatedPath.commitRotation
                                                  , interpolatedPath.commitScalingX
                                                  , interpolatedPath.commitScalingY );
                                path->UpdateMatrix();

                                // restore point coords that were changed in the pre-process
                                for( uint32 i = 0; i < interpolatedPath.GetInterpolatedPointBuffer().size(); i++ )
                                {
                                    FInterpolatedPoint* interpolatedPoint = &interpolatedPath.GetInterpolatedPointBuffer()[i];
                                    FOdysseyVectorPoint* originalPoint = interpolatedPoint->GetOriginalPoint();
                                    ::ULIS::FVec2D* commitPosition = &interpolatedPath.GetInterpolatedPointGeometryBuffer()[skippedOffset + i].position;

                                    if( originalPoint->GetClass() == FOdysseyVectorVertex::StaticClass() )
                                    {
                                        FOdysseyVectorVertex* originalVertex = static_cast<FOdysseyVectorVertex*>(originalPoint);
                                        double* commitRadius = &interpolatedPath.GetInterpolatedPointGeometryBuffer()[skippedOffset + i].radius;

                                        // restore radius that was saved in pre-process
                                        originalVertex->SetRadius( *commitRadius );
                                    }

                                    // restore position that was saved in pre-process
                                    interpolatedPoint->mOriginalPoint->Set( commitPosition->x
                                                                          , commitPosition->y );
                                }
                            }
                        }

                        return 0;
                    };

                    FOdysseyVectorGroupPaint* inbetweenScene = inbetweenCell->GetScene();
                    FOdysseyVectorObject* copiedObject = mOwner->Copy( FOdysseyVectorObject::COPY_NOTAG
                                                                     , preProcess
                                                                     , postProcess );
                    BLMatrix2D conversionMatrix = inbetweenScene->GetInverseWorldMatrix();
                    BLMatrix2D copiedObjectWorldMatrix = mOwner->GetWorldMatrix();
                    double translationX, translationY, rotation, scalingX, scalingY;

                    oAddedObjectList.push_back( copiedObject );

                    if( std::find( oCommittedSceneList.begin()
                                 , oCommittedSceneList.end()
                                 , inbetweenScene ) == oCommittedSceneList.end() )
                    {
                        oCommittedSceneList.push_back( inbetweenScene );
                    }

                    // the scene that receives the commited objects might be zoomed-in/out or paned.
                    // Thus we have to get sure the committed object will keep the look it had in the preview
                    // by adapting is transformations to the receiving scene.
                    //copiedObjectWorldMatrix.transform( drawing->localMatrix );

                    conversionMatrix.transform( copiedObjectWorldMatrix );

                    FOdysseyVector::ExtractTransformations( conversionMatrix
                                                          , &translationX
                                                          , &translationY
                                                          , &rotation
                                                          , &scalingX
                                                          , &scalingY
                                                          , true );

                    copiedObject->SetTransform( translationX, translationY, rotation, scalingX, scalingY );

                    inbetweenScene->AppendChild( copiedObject );
                    inbetweenScene->UpdateMatrix();

                    //inbetweenScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
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
