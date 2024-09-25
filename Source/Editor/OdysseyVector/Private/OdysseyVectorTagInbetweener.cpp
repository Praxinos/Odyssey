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

    //delete mGrid;
    mBreakdownList.remove_if( []( FInbetweenerBreakdown* breakdown )
                              {
                                  if( breakdown != breakdown->GetMasterBreakdown() )
                                  {
                                      delete breakdown;
                                  }

                                  return true;
                              } );
}

FOdysseyVectorTagInbetweener::FOdysseyVectorTagInbetweener( FOdysseyVectorSharedEnv* iSharedEnv
                                                          , FOdysseyVectorObject* iOwnerObject
                                                          , uint32 iNumQuadX
                                                          , uint32 iNumQuadY )
    : FOdysseyVectorTag( iOwnerObject )
    //, mSharedEnv ( iSharedEnv )
    // Remember the scene because when removing the tag, we'll need to be able to redraw inbetween cells but
    // as the owner object won't be linked to the screne anymore, the scene won't be retrievable.
    // Note: hence the Owner MUST be in the scene's hierarchy
    , mScene ( iOwnerObject->GetScene() )
    , mSharedEnv ( iOwnerObject->GetSharedEnv() )
    //, mGrid( nullptr )
    , mGridType( eInbetweenerGridType::ARAP )
    , mGridNumQuadX( iNumQuadX )
    , mGridNumQuadY( iNumQuadY )
    , mInterpolationType( eInbetweenerInterpolationType::ARAP )
    , mInvalidationFlags( INVALIDATE_MAP
                        | INVALIDATE_SOURCEGRID
                        | INVALIDATE_TARGETGRID
                        | INVALIDATE_ROUTES
                        | INVALIDATE_SPACING
                        | INVALIDATE_CELLS )
    , mColor ( DEFAULT_RED_UINT8, DEFAULT_GREEN_UINT8, DEFAULT_BLUE_UINT8, DEFAULT_ALPHA_UINT8 )
    , bMapAsPolyline( true )
    , mMasterBreakdown( this )
    , bARAPPrecomputeSucceded( false )
    , mInterpolationDirection( eInbetweenerInterpolationDirection::Forward )
{
    // the default breakdown (has range 0 <-> 1 )
    mBreakdownList.emplace_back( &mMasterBreakdown );

    // Note: Grid building needs the bbox to be set.
    //SetGrid( mGridType, iNumQuadX, iNumQuadY );

    // match drawing with the number of inbetween in the breakdown's chart at first.
    mDrawingBuffer.reserve( mMasterBreakdown.GetDrawingCount() );
    for( uint32 i = 0; i < mMasterBreakdown.GetDrawingCount(); i++ )
    {
        mDrawingBuffer.emplace_back( this );
    }
    //ResizeChart( true );

    // Note: Matrix needs chart to be allocated first.
    UpdateMatrix();
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

                  // force minimum segment subdivision (important for being able to deform
                  // straight segments
                  path->InvalidateAllSegments();
                  // do not call Update(), call UpdateShape().
                  //  Update would call tag->Update() (this fuinction)
                  // resulting in a inifinite loop.
                  path->UpdateShape( FOdysseyVectorObject::UPDATE_NEEDPOLYLINE );

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
    RedrawAnimationCells();

    mInterpolationDirection = iDirection;

    Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_CELLS );
}

void
FOdysseyVectorTagInbetweener::AddRoute( FInbetweenerRoute* iRoute )
{
    mRouteList.push_back( iRoute );

    Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_ROUTES
              | FOdysseyVectorTagInbetweener::INVALIDATE_ROUTE_LIST
              | FOdysseyVectorTagInbetweener::INVALIDATE_SPACING
              | FOdysseyVectorTagInbetweener::INVALIDATE_CELLS );
}

void
FOdysseyVectorTagInbetweener::RemoveRoute( FInbetweenerRoute* iRoute )
{
    mRouteList.remove( iRoute );

    Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_ROUTE_LIST
              | FOdysseyVectorTagInbetweener::INVALIDATE_SPACING
              | FOdysseyVectorTagInbetweener::INVALIDATE_CELLS );
}

void
FOdysseyVectorTagInbetweener::RemoveAllRoutes()
{
    mRouteList.clear();

    Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_ROUTE_LIST
              | FOdysseyVectorTagInbetweener::INVALIDATE_SPACING
              | FOdysseyVectorTagInbetweener::INVALIDATE_CELLS );
}

FInbetweenerRoute*
FOdysseyVectorTagInbetweener::AddRoute( const ::ULIS::FVec2D& iLocalCoords )
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

        return route;
    }

    return nullptr;
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

    Invalidate( INVALIDATE_MAP | INVALIDATE_CELLS | INVALIDATE_TARGETGRID );
}

const FColor&
FOdysseyVectorTagInbetweener::GetColor()
{
    return mColor;
}

void
FOdysseyVectorTagInbetweener::SetColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA )
{
    mColor.R = iR;
    mColor.G = iG;
    mColor.B = iB;
    mColor.A = iA;
}

void
FOdysseyVectorTagInbetweener::SetColor( const FColor& iColor )
{
    mColor = iColor;
}

FInbetweenerBreakdown*
FOdysseyVectorTagInbetweener::AddBreakdown( uint32 iDrawingIndex, bool iCopyGeometry )
{
    return AddBreakdown( nullptr, iDrawingIndex, iCopyGeometry );
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

    // Reset grid at source position for first the breakdown (must always be squared)
    std::vector<::ULIS::FVec2D> sourceGeometry;
    std::vector<::ULIS::FVec2D> targetGeometry;

    // save positions for restoring when calling Make()
/*
    mBreakdownList.front()->GetGrid()->GetGeometry( targetGeometry, eInbetweenerPointPositionType::TargetPosition );
    mBreakdownList.front()->GetGrid()->Make( sourceGeometry, targetGeometry );
*/
}

// Removes all breakdowns but the default one
void
FOdysseyVectorTagInbetweener::ResetLayout( bool iFreeMemNow )
{
    std::vector<::ULIS::FVec2D> sourceGeometry;
    std::vector<::ULIS::FVec2D> targetGeometry;

    mBreakdownList.front()->GetGrid()->GetGeometry( sourceGeometry, eInbetweenerPointPositionType::SourcePosition );
    mBreakdownList.back() ->GetGrid()->GetGeometry( targetGeometry, eInbetweenerPointPositionType::TargetPosition );

    mBreakdownList.remove_if( [ this
                              , iFreeMemNow ]( FInbetweenerBreakdown* breakdown )
                              {
                                  if( iFreeMemNow && ( breakdown != breakdown->GetMasterBreakdown() ) )
                                  {
                                      delete breakdown;
                                  }

                                  return true;
                              } );

    mBreakdownList.push_back( &mMasterBreakdown );

    ChainBreakdowns();

    mMasterBreakdown.SetSourceDrawingIndex( 0 );

    mMasterBreakdown.GetGrid()->SetGeometry( sourceGeometry, eInbetweenerPointPositionType::SourcePosition );
    mMasterBreakdown.GetGrid()->SetGeometry( targetGeometry, eInbetweenerPointPositionType::TargetPosition );

    Invalidate( INVALIDATE_BREAKDOWN_LIST );
}

FInbetweenerBreakdown*
FOdysseyVectorTagInbetweener::AddBreakdown( FInbetweenerBreakdown* iNewBreakdown
                                          , uint32 iDrawingIndex
                                          , bool iCopyGeometry )
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
        FChartDivision* inbetween = &curBreakdown->GetChart()->GetDivisionArray()[inbetweenIndex];
        FInbetweenerBreakdown* newBreakdown = iNewBreakdown ? iNewBreakdown 
                                                            : new FInbetweenerBreakdown( this );

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

        // this also alters the previous breakdown target index, so it must be done after the chaining has been updated
        curBreakdown->SetSourceDrawingIndex( iDrawingIndex );

        newBreakdown->SetTargetDrawingIndex( newTargetDrawingIndex );
        newBreakdown->SetSourceDrawingIndex( newSourceDrawingIndex );

        ChainBreakdowns();

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

            newBreakdown->GetGrid()->SetGeometry( curBreakdownSourceGeometry, eInbetweenerPointPositionType::SourcePosition );
            newBreakdown->GetGrid()->SetGeometry( curBreakdownInterpGeometry, eInbetweenerPointPositionType::TargetPosition );

            newBreakdown->SetTargetTransform( prevTranslationX + ( ( translationX - prevTranslationX ) * t )
                                            , prevTranslationY + ( ( translationY - prevTranslationY ) * t )
                                            , prevRotation     + ( ( rotation     - prevRotation     ) * t )
                                            , prevScalingX     + ( ( scalingX     - prevScalingX     ) * t )
                                            , prevScalingY     + ( ( scalingY     - prevScalingY     ) * t ) );
            newBreakdown->UpdateMatrix();
        }

        Invalidate( INVALIDATE_BREAKDOWN_LIST );

        return newBreakdown;
    }

    //DispatchInbetweensToBreakdowns();
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
    FInbetweenerBreakdown* nextBreakdown = iBreakdown->GetNextBreakdown();

    mBreakdownList.remove_if( [iBreakdown]( FInbetweenerBreakdown* listedBreakdown )
                              {
                                  return ( iBreakdown == listedBreakdown ) ? true : false;
                              } );

    if( nextBreakdown )
    {
        std::vector<::ULIS::FVec2D> breakdownSourceGeometry;

        iBreakdown->GetGrid()->GetGeometry( breakdownSourceGeometry, eInbetweenerPointPositionType::SourcePosition );

        // nextBreakdown source grid gets its shape from this removed breakdown source grid.
        nextBreakdown->GetGrid()->SetGeometry( breakdownSourceGeometry, eInbetweenerPointPositionType::SourcePosition );
        // as well as its source drawing index
        nextBreakdown->SetSourceDrawingIndex( iBreakdown->GetSourceDrawingIndex() );
    }

    iBreakdown->SetInbetweenerTag( nullptr );

    // Readd if its the default breakdown
    if( iBreakdown == &mMasterBreakdown )
    {
        mMasterBreakdown.SetInbetweenerTag( this );

        mBreakdownList.push_back( &mMasterBreakdown );
    }

    ChainBreakdowns();

    /*ResizeRoutes();*/

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

void
FOdysseyVectorTagInbetweener::SetUsedQuadCount( uint32 iUsedQuadCount )
{
    mUsedQuadCount = iUsedQuadCount;
}

void
FOdysseyVectorTagInbetweener::SetUsedPointCount( uint32 iUsedPointCount  )
{
    mUsedPointCount = iUsedPointCount;
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
    if( bShared == false )
    {
        Share( mSharedEnv );
    }
}

void
FOdysseyVectorTagInbetweener::ObjectRemoved()
{
    if( bShared == true )
    {
        Unshare( mSharedEnv );
    }
}

void
FOdysseyVectorTagInbetweener::Added()
{
    Share( mSharedEnv );

    RedrawAnimationCells();
}

void
FOdysseyVectorTagInbetweener::Removed()
{
    Unshare( mSharedEnv );

    RedrawAnimationCells();
}

void FOdysseyVectorTagInbetweener::Update( uint32 iUpdateFlags
                                         , uint64 iOwnerInvalidationFlags )
{
/*
    if( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_HIERARCHY )
    {
        if( ( mOwner->GetSharedEnv() != nullptr ) && ( bShared == false ) )
        {
            Share( mSharedEnv );
        }

        if( ( mOwner->GetSharedEnv() == nullptr ) && ( bShared == true ) )
        {
            Unshare( mSharedEnv );
        }
    }
*/
    if( ( bShared == true )
     && ( ( iUpdateFlags & FOdysseyVectorObject::UPDATE_INTERACTIVE ) == 0 ) )
    {
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

                // save positions for restoring when calling Make()
                //breakdown->GetGrid()->GetGeometry( sourceGeometry, eInbetweenerPointPositionType::SourcePosition );
                breakdown->GetGrid()->GetGeometry( targetGeometry, eInbetweenerPointPositionType::TargetPosition );
                breakdown->GetGrid()->Make( sourceGeometry, targetGeometry );
            }
        }

        // will update grids' BBoxes (needed for transform HUD and discarding of unused quads in ARAP grids)
        for( FInbetweenerBreakdown* breakdown : mBreakdownList )
        {
            breakdown->GetGrid()->UpdateBBox( iUpdateFlags, mInvalidationFlags );
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

        if( ( mInvalidationFlags & INVALIDATE_RANGE   )
         || ( mInvalidationFlags & INVALIDATE_BUFFERS ) )
        {
            // alloc position for points at each interpolation step
            AllocBuffers();

            DeformPathsAtSource();
        }

        // will update grids' center of mass (needed for interpolation).
        // MUST be done after mapping because mapping will elimniate some quads, and this is taken into account
        // for the center of mass.²
        for( FInbetweenerBreakdown* breakdown : mBreakdownList )
        {
            breakdown->GetGrid()->UpdateCenterOfMass( iUpdateFlags, mInvalidationFlags );
        }
/*
        if( ( mInvalidationFlags & INVALIDATE_RANGE          )
         || ( mInvalidationFlags & INVALIDATE_BREAKDOWN_LIST ) )
        {
            ResizeRoutes();
        }
*/
        // Precompute ARAP interpolation after the grid and routes have been updated
        if( ( mInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_SOURCEGRID          )
         || ( mInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_TARGETGRID          )
         || ( mInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_INTERPOLATIONTYPE   )
         || ( mInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_GRIDTYPE            )
        // || ( mInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_SPACING    )
         || ( mInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_ROUTE_LIST          )
         || ( mInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_MAP                 ) )
        {
            for( FInbetweenerBreakdown* breakdown : mBreakdownList )
            {
                if( breakdown->GetInbetweenerTag()->GetInterpolationType() == eInbetweenerInterpolationType::ARAP )
                {
                    bARAPPrecomputeSucceded = breakdown->GetGrid()->PrecomputeARAPInterpolation();

                    if( bARAPPrecomputeSucceded  == false )
                    {
                        UE_LOG( LogTemp, Error, TEXT("ERROR DURING ARAP PRECOMPUTE"));
                    }
                }
            }
        }

        // altering breakdown range alters buffers. We then have to deform target anew.
        if( ( mInvalidationFlags & INVALIDATE_TARGETGRID )
         || ( mInvalidationFlags & INVALIDATE_RANGE      ) )
        {
            DeformPathsAtTarget( );
        }

        for( FInbetweenerRoute* route : mRouteList )
        {
            route->Update( iUpdateFlags, iOwnerInvalidationFlags, mInvalidationFlags );
        }

        Interpolate();

        RedrawAnimationCells();

        mInvalidationFlags = 0;
    }
}

IOdysseyVectorAnimationCell*
FOdysseyVectorTagInbetweener::GetAnimationCell()
{
    return mOwner->GetEngine()->GetAnimationCell();
}

uint32
FOdysseyVectorTagInbetweener::GetAnimationCellIndex()
{
    return mOwner->GetEngine()->GetAnimationCell()->GetIndex();
}

void
FOdysseyVectorTagInbetweener::Invalidate( uint64 iInvalidationFlags )
{
    if( mInvalidationFlags == 0 )
    {
        mOwner->InvalidateTag( this );
    }

    mInvalidationFlags |= iInvalidationFlags;
}
/*
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
*/
/*
void
FOdysseyVectorTagInbetweener::ResetChart()
{
    mChart.Reset();
}

void
FOdysseyVectorTagInbetweener::ResizeChart()
{
    mChart.Resize();
}

FInbetweenerChart&
FOdysseyVectorTagInbetweener::GetChart()
{
    return mChart;
}
void
FOdysseyVectorTagInbetweener::SetChart( const FInbetweenerChart& iChart )
{
    mChart = iChart;

    Invalidate( INVALIDATE_SPACING | INVALIDATE_CELLS );
}
*/

void
FOdysseyVectorTagInbetweener::ResizeDrawings()
{
    mDrawingBuffer.resize( GetLength(), this );

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
        FChartDivision* inbetween = &breakdown->GetChart()->GetDivisionArray().front();

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
        FChartDivision* inbetween = &breakdown->GetChart()->GetDivisionArray().back();

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

                breakdown->GetChart()->GetDivisionArray()[chartDivisionIndex].drawing = &mDrawingBuffer[i];
            }
        }
/*
        for( uint32 i = 0; i < GetDrawingCount(); i++ )
        {
            FInbetweenerDrawing* inbetween = mChart.GetDrawing( i );

            if( ( i >= sourceDrawingIndex ) && ( i < targetDrawingIndex ) )
            {
                double spacing = inbetween->spacing;

                inbetween->breakdown = breakdown;
                // compute the spacing relative to the start of the breakdown
                inbetween->breakdownSpacing = ( spacing - breakdownFirstSpacing ) / ( breakdownLastSpacing - breakdownFirstSpacing );
            }
        }

        breakdownFirstSpacing = breakdownLastSpacing;
*/
    }
}

void
FOdysseyVectorTagInbetweener::DeformGridAtInbetween( FChartDivision *iInbetween )
{
    double t = iInbetween->spacing;
    //::ULIS::FRectD bbox = mSourceBBox;

    if( mInterpolationType == eInbetweenerInterpolationType::Linear )
    {
        for( FInbetweenerPoint& point : iInbetween->chart->GetBreakdown()->GetGrid()->GetPointBuffer() )
        {
            if( point.GetQuadCount() )
            {
                ::ULIS::FVec2D diff = ( point.mTargetPosition - point.mSourcePosition );
                ::ULIS::FVec2D step = diff * t;

                point.mInterpPosition = point.mSourcePosition + step;
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
/*
void
FOdysseyVectorTagInbetweener::SetDrawingCount( uint32 iDrawingCount )
{
    uint32 minDrawingCount = ::ULIS::FMath::Max( (int)iDrawingCount, 2 );

    mBreakdownList.back()->SetTargetDrawingIndex( minDrawingCount - 1 );

    Invalidate( INVALIDATE_SPACING
              | INVALIDATE_BUFFERS
              | INVALIDATE_CELLS );

    ResetChart();

    RedrawAnimationCells( minDrawingCount );
}
*/
void
FOdysseyVectorTagInbetweener::RedrawAnimationCells()
{
    RedrawAnimationCells( GetLength() );
}

int32
FOdysseyVectorTagInbetweener::GetDrawingIndexFromCellIndex( uint32 iCellIndex )
{
    uint32 tagCellIndex = GetAnimationCellIndex();

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
FOdysseyVectorTagInbetweener::RedrawAnimationCells( uint32 iDrawingCount )
{
    // scene could be non existent when the tag's owner is removed, as it would still trigger call to Update()
    // right after the removal of an object in the hierarchy.
    if( mScene->GetSharedEnv() )
    {
        IOdysseyVectorAnimationCell* animationCell = mScene->GetEngine()->GetAnimationCell();

        if( animationCell )
        {
            int32 animationCellIndex = animationCell->GetIndex();

            // Redraw impacted cells
            for( uint32 i = 1; ( i < iDrawingCount ) && ( animationCell != nullptr ); i++ )
            {
                FInbetweenerDrawing* drawing = GetDrawing( i );
                int32 inbetweenCellIndex = drawing->GetAnimationCellIndex();
                IOdysseyVectorAnimationCell* nextAnimationCell = animationCell->GetCellByIndex( inbetweenCellIndex );

                if( nextAnimationCell )
                {
                    //nextAnimationCell->GetEngine()->Invalidate();
                    // request redraw
                    nextAnimationCell->GetEngine()->Invalidate( 0 );
                }

                animationCell = nextAnimationCell;
            }
        }
    }
}

void
FOdysseyVectorTagInbetweener::Interpolate()
{
    if( mInterpolationType == eInbetweenerInterpolationType::ARAP )
    {
/*
        if( mGrid->PrecomputeARAPInterpolation() == false )
        {
            UE_LOG( LogTemp, Warning, TEXT("ERROR DURING PRECOMPUTE"));
        }
*/
    }

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
            FChartDivision* inbetween = &breakdown->GetChart()->GetDivisionArray()[i];

            DeformPathsAtInbetween( inbetween );
        }
    }

    //UE_LOG( LogTemp, Warning, TEXT("Interpolate geometry"));
/*
    for( uint32 i = 0; i < mDrawingCount; i++ )
    {
        InterpolateTransform( i );
        DeformPathsAtInbetween( i );
    }
*/
    //UE_LOG( LogTemp, Warning, TEXT("-------------------"));
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
            uint32 sourceCellIndex = tagCell->GetIndex();
            uint32 targetCellIndex = sourceCellIndex + ( ( GetLength() - 1 ) * (int)mInterpolationDirection );
            uint32 displayedCellIndex = displayedCell->GetIndex();
            uint32 fromCellIndex = std::min( sourceCellIndex, targetCellIndex );
            uint32   toCellIndex = std::max( sourceCellIndex, targetCellIndex );

            if ( ( displayedCellIndex > fromCellIndex )
              && ( displayedCellIndex < toCellIndex   ) )
            {
                uint32 drawingIndex = abs( (int) (displayedCellIndex - sourceCellIndex) );

                // don't draw the object at the source position, it's already drawn
                if( drawingIndex > 0 )
                {
                    FInbetweenerBreakdown* breakdown = GetBreakdown( drawingIndex, false );
                    uint32 inbetweenIndex = drawingIndex - breakdown->GetSourceDrawingIndex();
                    FChartDivision* inbetween = &breakdown->GetChart()->GetDivisionArray()[inbetweenIndex];

                    DrawPathsInbetween( inbetween, iBLContext );
                }
            }
        }

        iBLContext->restore();

////////////////////////////////// TEMP /////////////////////////
/*
        if( mInterpolationType == eInbetweenerInterpolationType::ARAP )
        {
            uint32 tagCellIndex = tagCell->GetIndex();
            uint32 displayedCellIndex = displayedCell->GetIndex();
            uint32 iDrawingIndex = ( displayedCellIndex - tagCellIndex - 1 );
            double t = mChart.drawingBuffer[iDrawingIndex].spacing;

            mGrid->ComputeARAPInterpolation( t
                                           , t
                                           // , const FInbetweenerPoint::Affine &globalRigidTransform
                                           , false );

            DrawMotionGrid( iDrawingIndex
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
    for( FInterpolatedSegment& interpolatedSegment : iInterpolatedPath->mInterpolatedSegmentBuffer )
    {
        if( bMapAsPolyline )
        {
             uint32 pointCount = interpolatedSegment.mInterpolatedPointArray.size();

            for( uint32 i = 0; i < pointCount - 1; i++ )
            {
                uint32 n = i + 1;
                FInterpolatedPoint* pointi = interpolatedSegment.mInterpolatedPointArray[i];
                FInterpolatedPoint* pointn = interpolatedSegment.mInterpolatedPointArray[n];
                BLPoint pt[2] = { iWorldMatrix.mapPoint( iPointPositionBuffer[pointi->mIndex].x
                                                       , iPointPositionBuffer[pointi->mIndex].y )
                                , iWorldMatrix.mapPoint( iPointPositionBuffer[pointn->mIndex].x
                                                       , iPointPositionBuffer[pointn->mIndex].y ) };

                iBLContext->strokeLine( pt[0].x, pt[0].y, pt[1].x, pt[1].y );
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
    }
}

std::vector<FInterpolatedPath>&
FOdysseyVectorTagInbetweener::GetInterpolatedPathBuffer()
{
    return mInterpolatedPathBuffer;
}

void
FOdysseyVectorTagInbetweener::DrawPathsInbetween( FChartDivision* inbetween
                                                , BLContext* iBLContext )
{
    BLMatrix2D worldMatrix = mOwner->GetWorldMatrix();

    iBLContext->save();
    iBLContext->resetMatrix();

    // passed to DrawPathAt()
    worldMatrix.transform( inbetween->drawing->localMatrix );

    for( FInterpolatedPath& interpolatedPath : mInterpolatedPathBuffer )
    {
        uint32 pointCount = interpolatedPath.mInterpolatedPointBuffer.size();
        uint32 inbetweenAbsoluteIndex = inbetween->GetAbsoluteIndex();
        ::ULIS::FVec2D* pointPositionBuffer = &interpolatedPath.mInterpolatedPointPositionBuffer[pointCount * inbetweenAbsoluteIndex];

        DrawPathAt( &interpolatedPath
                  , pointPositionBuffer
                  , worldMatrix
                  , iBLContext );
    }

    iBLContext->restore();
}

void
FOdysseyVectorTagInbetweener::MoveInbetween( FChartDivision* iInbetween
                                           , float iNewSpacing
                                           , bool iRelative )
{
    FInbetweenerBreakdown* breakdown = iInbetween->chart->GetBreakdown();

    if( ( iNewSpacing > 0.0f ) && ( iNewSpacing < 1.0f ) )
    {
        IOdysseyVectorAnimationCell* animationCell = mOwner->GetScene()->GetEngine()->GetAnimationCell();
        int32  prevIndex  = iInbetween->GetIndex() - 1;
        uint32 nextIndex  = iInbetween->GetIndex() + 1;
        float prevSpacing = iInbetween->chart->GetDivisionArray()[prevIndex].spacing;
        float nextSpacing = iInbetween->chart->GetDivisionArray()[nextIndex].spacing;

        if( iRelative == false )
        {
            if( ( iNewSpacing > prevSpacing )
             && ( iNewSpacing < nextSpacing ) )
            {
                uint32 inbetweenCellIndex = iInbetween->GetAnimationCellIndex();
                IOdysseyVectorAnimationCell* inbetweenCell = GetOwner()->GetEngine()->GetAnimationCell()->GetCellByIndex( inbetweenCellIndex );

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
                FChartDivision* otherInbetween = &breakdown->GetChart()->GetDivisionArray()[i];

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
        breakdown->GetGrid()->Make();
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

FInbetweenerBreakdown*
FOdysseyVectorTagInbetweener::GetMasterBreakdown()
{
    return &mMasterBreakdown;
}

void
FOdysseyVectorTagInbetweener::SetGrid( eInbetweenerGridType iGridType
                                     , uint32 iGridNumQuadX
                                     , uint32 iGridNumQuadY )
{
    if( ( mGridNumQuadX != iGridNumQuadX ) || ( mGridNumQuadY != iGridNumQuadY ) )
    {
        RemoveAllRoutes();
    }

    mGridType = iGridType;
    mGridNumQuadX = iGridNumQuadX ? iGridNumQuadX : 1;
    mGridNumQuadY = iGridNumQuadY ? iGridNumQuadY : 1;

    Invalidate( INVALIDATE_MAP
              | INVALIDATE_SPACING
              | INVALIDATE_GRIDTYPE
              | INVALIDATE_CELLS );

    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        breakdown->SetGrid( iGridType );
        breakdown->GetGrid()->Make();
    }
}

void
FOdysseyVectorTagInbetweener::SetGridNumQuad( uint32 iGridNumQuadX
                                            , uint32 iGridNumQuadY
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

    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        breakdown->GetGrid()->Make( iSourcePositionBuffer
                                 , iTargetPositionBuffer );
    }
}

void
FOdysseyVectorTagInbetweener::SetGridNumQuad( uint32 iGridNumQuadX
                                            , uint32 iGridNumQuadY )
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

    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        breakdown->GetGrid()->Make();
    }
}

void
FOdysseyVectorTagInbetweener::Commit( std::list<FOdysseyVectorTag*>& oRemovedTagList
                                    , std::list<FOdysseyVectorObject*>& oAddedObjectList
                                    , std::list<FOdysseyVectorGroupPaint*>& oCommittedSceneList )
{
    IOdysseyVectorAnimationCell* animationCell = mOwner->GetScene()->GetEngine()->GetAnimationCell();
    int32 animationCellIndex = animationCell->GetIndex();


    for( uint32 drawingIndex = 1; drawingIndex < ( GetLength() - 1 ); drawingIndex++ )
    {
        IOdysseyVectorAnimationCell* inbetweenAnimationCell = animationCell->GetCellByIndex( animationCellIndex + drawingIndex );

        if( inbetweenAnimationCell )
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

/*
                            transformedWorldPosition = ownerWorldMatrix.mapPoint( transformedLocalPosition.x
                                                                                , transformedLocalPosition.y );
*/
                            interpolatedPoint->mOriginalPoint->Set( transformedLocalPosition.x
                                                                  , transformedLocalPosition.y );

                            *commitPosition = swapPosition;
                        }
                    }

                    //tag->GetOwner()->RemoveTag( tag );
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

                    drawing->localMatrix.reset();

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

            FOdysseyVectorGroupPaint* inbetweenScene = inbetweenAnimationCell->GetEngine()->GetScene();

            FOdysseyVectorObject* copiedObject = mOwner->Copy( FOdysseyVectorObject::COPY_WORLDCOORDS
                                                             , preProcess
                                                             , postProcess );

            oAddedObjectList.push_back( copiedObject );
            oCommittedSceneList.push_back( inbetweenScene );

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

    mOwner->RecursiveRemoveTagByType( FOdysseyVectorTagInbetweener::StaticClass()
                                    , oRemovedTagList );

    RedrawAnimationCells();
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
              | INVALIDATE_INTERPOLATIONTYPE
              | INVALIDATE_CELLS );
}
