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

void
FOdysseyVectorTagInbetweener::UpdateBBox( ::ULIS::FRectD& iBBox
                                        , eInbetweenerPointPositionType iPositionType )
{
    double xmin = DBL_MAX, ymin = DBL_MAX, xmax = -DBL_MAX, ymax = -DBL_MAX;
    bool hasBBox = false;

    for( FInbetweenerPoint& gridPoint : mBreakdownList.back()->GetGrid()->GetPointBuffer() )
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

                  // force minimum segment subdivision (important for being able to deform
                  // straight segments
                  path->InvalidateAllSegments();
                  // do not call Update(), call UpdateShape().
                  //  Update would call tag->Update() (this fuinction)
                  // resulting in a inifinite loop.
                  path->UpdateShape( FOdysseyVectorObject::UPDATE_NEEDPOLYLINE );

                  mInterpolatedPathBuffer.emplace_back( path
                                                      , GetDrawingCount()
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

std::list<FInbetweenerRoute*>&
FOdysseyVectorTagInbetweener::GetRouteList()
{
    return mRouteList;
}

FInbetweenerDrawing*
FOdysseyVectorTagInbetweener::GetDrawing( uint32 iIndex )
{
    return mChart.GetDrawing( iIndex );
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

    Invalidate( INVALIDATE_MAP | INVALIDATE_CELLS | INVALIDATE_TARGET );
}

FOdysseyVectorTagInbetweener::FOdysseyVectorTagInbetweener( FOdysseyVectorSharedEnv* iSharedEnv
                                                          , FOdysseyVectorObject* iOwnerObject
                                                          , uint32 iNumQuadX
                                                          , uint32 iNumQuadY )
    : FOdysseyVectorTag( iOwnerObject )
    // note: mSharedEnv is remebered as a member variable because GetEngine() calls
    // GetClass() and the latter is a virtual function. virtual function don't work
    // in destructors.
    , mSharedEnv ( iSharedEnv )
    //, mGrid( nullptr )
    , mGridType( eInbetweenerGridType::ARAP )
    , mGridNumQuadX( iNumQuadX )
    , mGridNumQuadY( iNumQuadY )
    , mInterpolationType( eInbetweenerInterpolationType::ARAP )
    , mInvalidationFlags( INVALIDATE_MAP
                        | INVALIDATE_BUFFERS
                        | INVALIDATE_SOURCE
                        | INVALIDATE_TARGET
                        | INVALIDATE_ROUTES
                        | INVALIDATE_SPACING
                        | INVALIDATE_CELLS )
    , mColor ( DEFAULT_RED_UINT8, DEFAULT_GREEN_UINT8, DEFAULT_BLUE_UINT8, DEFAULT_ALPHA_UINT8 )
    , bMapAsPolyline( true )
    , bShared ( false )
    , mARAPRigidity ( 10 )
    , mMasterBreakdown( this )
    , mChart( this )
{
    // the default breakdown (has range 0 <-> 1 )
    mBreakdownList.emplace_back( &mMasterBreakdown );

    // Note: Grid building needs the bbox to be set.
    //SetGrid( mGridType, iNumQuadX, iNumQuadY );

    // chart has 2 drawings at first.
    mChart.GetDrawingBuffer().emplace_back( &mChart ).spacing = 0.0f;
    mChart.GetDrawingBuffer().emplace_back( &mChart ).spacing = 1.0f;
    //ResizeChart( true );

    // Note: Matrix needs chart to be allocated first.
    UpdateMatrix();
}

uint32
FOdysseyVectorTagInbetweener::GetARAPRigidity()
{
    return mARAPRigidity;
}

void
FOdysseyVectorTagInbetweener::SetARAPRigidity( uint32 iRigidity )
{
    mARAPRigidity = iRigidity;
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

void FOdysseyVectorTagInbetweener::Added()
{
    Share();
}

void FOdysseyVectorTagInbetweener::Removed()
{
    Unshare();
}

void
FOdysseyVectorTagInbetweener::Share()
{
    mSharedEnv->AddSharedTag( this );

    bShared = true;
}

void
FOdysseyVectorTagInbetweener::Unshare()
{
    mSharedEnv->RemoveSharedTag( this );

    bShared = false;
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

    mMasterBreakdown.SetSourceDrawingIndex( 0 );

    mBreakdownList.push_back( &mMasterBreakdown );

    ChainBreakdowns();

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
    std::list<FInbetweenerBreakdown*>::iterator curBreakdownIterator = GetBreakdownItem( iDrawingIndex );

    if( curBreakdownIterator!= mBreakdownList.end() )
    {
        FInbetweenerBreakdown* curBreakdown = *curBreakdownIterator;

        if ( ( iDrawingIndex > curBreakdown->GetSourceDrawingIndex() )
          && ( iDrawingIndex < curBreakdown->GetTargetDrawingIndex() ) )
        {
            FInbetweenerBreakdown* prevBreakdown = curBreakdown->GetPrevBreakdown();
            int32 curSourceDrawingIndex = curBreakdown->GetSourceDrawingIndex();
            int32 curTargetDrawingIndex = curBreakdown->GetTargetDrawingIndex();
            int32 newSourceDrawingIndex = curSourceDrawingIndex;
            int32 newTargetDrawingIndex = iDrawingIndex;
            FInbetweenerBreakdown* newBreakdown = iNewBreakdown ? iNewBreakdown 
                                                                : new FInbetweenerBreakdown( this );

            // This is for an already existing breakdown if it had been removed before
            // (then its pointer to the tag would be null)
            newBreakdown->SetInbetweenerTag( this );

            // Note: SetTargetDrawingIndex() will reset the chart so we do this before the call to SetTargetDrawingIndex()
            if( iCopyGeometry )
            {
                // we will take grid coords from interpolated coords
                DeformGridAtInbetween( iDrawingIndex );

                curBreakdown->GetGrid()->GetGeometry( curBreakdownSourceGeometry, eInbetweenerPointPositionType::SourcePosition );
                curBreakdown->GetGrid()->GetGeometry( curBreakdownInterpGeometry, eInbetweenerPointPositionType::InterpPosition );
            }

            newBreakdown->SetSourceDrawingIndex( newSourceDrawingIndex );
            newBreakdown->SetTargetDrawingIndex( newTargetDrawingIndex );

            mBreakdownList.insert( curBreakdownIterator, newBreakdown );

            ChainBreakdowns();

            /*ResizeRoutes();*/

            // this also alters the previous breakdown target index, so it must be done after the chaining has been updated
            curBreakdown->SetSourceDrawingIndex( iDrawingIndex );

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
    }

    //DispatchInbetweensToBreakdowns();
    return nullptr;
}

FInbetweenerBreakdown*
FOdysseyVectorTagInbetweener::GetBreakdown( uint32 iDrawingIndex )
{
    std::list<FInbetweenerBreakdown*>::iterator curBreakdownIterator = GetBreakdownItem( iDrawingIndex );

    return ( curBreakdownIterator != mBreakdownList.end() ) ? *curBreakdownIterator : nullptr;
}

std::list<FInbetweenerBreakdown*>::iterator
FOdysseyVectorTagInbetweener::GetBreakdownItem( uint32 iDrawingIndex )
{
    std::list<FInbetweenerBreakdown*>::iterator
        curBreakdownIterator = std::find_if( mBreakdownList.begin()
                                           , mBreakdownList.end()
                                           , [&iDrawingIndex]( FInbetweenerBreakdown* breakdown ) -> bool
                                             {
                                                 if( ( iDrawingIndex >= breakdown->GetSourceDrawingIndex() )
                                                  && ( iDrawingIndex <= breakdown->GetTargetDrawingIndex() ) )
                                                 {
                                                     return true;
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

void FOdysseyVectorTagInbetweener::Update( uint32 iUpdateFlags
                                         , uint64 iOwnerInvalidationFlags )
{
    if( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_HIERARCHY )
    {
        if( mOwner->GetScene() == nullptr )
        {
            Unshare();
        }
        else
        {
            if( mSharedEnv->HasSharedTag( this ) == false )
            {
                Share();
            }
        }
    }

    if( bShared == true )
    {
        if( ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_HIERARCHY      )
         || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_SHAPE          )
         || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_TOPOLOGY       )
         || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_TOPOLOGY )
         || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_SHAPE    ) )
        {
            if( ( iUpdateFlags & FOdysseyVectorObject::UPDATE_INTERACTIVE ) == 0 )
            {
/*
                mSourceBBox = mOwner->GetBBox( false );

                mSourceBBox.x -= 0.1f;
                mSourceBBox.y -= 0.1f;
                mSourceBBox.w += 0.2f;
                mSourceBBox.h += 0.2f;
*/
                for( FInbetweenerBreakdown* breakdown : mBreakdownList )
                {
                    std::vector<::ULIS::FVec2D> sourceGeometry;
                    std::vector<::ULIS::FVec2D> targetGeometry;

                    // save positions for restoring when calling Make()
                    breakdown->GetGrid()->GetGeometry( sourceGeometry, eInbetweenerPointPositionType::SourcePosition );
                    breakdown->GetGrid()->GetGeometry( targetGeometry, eInbetweenerPointPositionType::TargetPosition );
                    breakdown->GetGrid()->Make( sourceGeometry, targetGeometry );
                }
            }
        }

        if( ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_HIERARCHY      )
         || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_TOPOLOGY       )
         || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_SHAPE          )
         || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_TAG_LIST )
         || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_TOPOLOGY ) 
         || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_SHAPE    ) )
        {
            mInvalidationFlags |= INVALIDATE_MAP;
        }

        if( ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_SHAPE    )
         || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_TAG_LIST )
         //|| ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_MATRIX   )
         || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_SHAPE    )
         || ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_TAG_LIST ) )
         //|| ( iOwnerInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_MATRIX   ) )
        {
            mInvalidationFlags |= ( INVALIDATE_SPACING );
        }

        // will update grids' BBoxes (needed for transform HUD and discarding of unused quads in ARAP grids)
        for( FInbetweenerBreakdown* breakdown : mBreakdownList )
        {
            breakdown->GetGrid()->UpdateBBox( iUpdateFlags, mInvalidationFlags );
        }

        if( ( mInvalidationFlags & INVALIDATE_MAP            )
         || ( mInvalidationFlags & INVALIDATE_BREAKDOWN_LIST ) )
        {
            // map object to the first grid
            Map();

            mInvalidationFlags |= INVALIDATE_BUFFERS;
        }

        // will update grids' center of mass (needed for interpolation).
        // MUST be done after mapping because mapping will elimniate some quads, and this is taken into account
        // for the center of mass.²
        for( FInbetweenerBreakdown* breakdown : mBreakdownList )
        {
            breakdown->GetGrid()->UpdateCenterOfMass( iUpdateFlags, mInvalidationFlags );
        }

        if( ( mInvalidationFlags & INVALIDATE_BUFFERS )
         || ( mInvalidationFlags & INVALIDATE_TARGET  ) )
        {
            // alloc buffers for interpolation
            AllocBuffers();
        }

// TODO: needs to be placed somewhere else
    DispatchInbetweensToBreakdowns();

        if( ( mInvalidationFlags & INVALIDATE_TARGET         )
          ||( mInvalidationFlags & INVALIDATE_BREAKDOWN_LIST ) )
        {
            ResizeRoutes();
        }

        // Precompute ARAP interpolation after the grid and routes have been updated
        if( ( mInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_SOURCE              )
         || ( mInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_TARGET              )
         || ( mInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_INTERPOLATIONTYPE   )
         || ( mInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_GRIDTYPE            )
        // || ( mInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_SPACING    )
         || ( mInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_ROUTE_LIST          )
         || ( mInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_MAP                 ) )
        {
            //if( ( iUpdateFlags & FOdysseyVectorObject::UPDATE_INTERACTIVE ) == 0 )
            {
                for( FInbetweenerBreakdown* breakdown : mBreakdownList )
                {
                    if( breakdown->GetInbetweenerTag()->GetInterpolationType() == eInbetweenerInterpolationType::ARAP )
                    {
                        if( breakdown->GetGrid()->PrecomputeARAPInterpolation() == false )
                        {
                            UE_LOG( LogTemp, Warning, TEXT("ERROR DURING PRECOMPUTE"));
                        }
                    }
                }
            }
        }


        if( mInvalidationFlags & INVALIDATE_TARGET )
        {
            DeformPathsAtTarget( );

            if( iUpdateFlags & FOdysseyVectorObject::UPDATE_INTERACTIVE )
            {
                mInvalidationFlags &= (~INVALIDATE_SPACING);
            }
        }

        //if( ( mInvalidationFlags & INVALIDATE_ROUTES )
        //||  ( mInvalidationFlags & INVALIDATE_TARGET ) )
        {
            for( FInbetweenerRoute* route : mRouteList )
            {
                route->Update( iUpdateFlags, iOwnerInvalidationFlags, mInvalidationFlags );
            }
        }



        if( ( mInvalidationFlags & INVALIDATE_SPACING )
         || ( mInvalidationFlags & INVALIDATE_MAP     )
         || ( mInvalidationFlags & INVALIDATE_TARGET  ) )
        {
            Interpolate();
        }

        if( ( iUpdateFlags & FOdysseyVectorObject::UPDATE_INTERACTIVE ) == 0 )
        {
            //if( mInvalidationFlags & INVALIDATE_CELLS )
            {
                RedrawAnimationCells();
            }
        }

        // reset tag's invalidation flags (do not confuse with object's invalidation flags)
        if( ( iUpdateFlags & FOdysseyVectorObject::UPDATE_INTERACTIVE ) == 0 )
        {
            mInvalidationFlags = 0;
        }
    }
}

void
FOdysseyVectorTagInbetweener::Invalidate( uint64 iInvalidationFlags )
{
    mOwner->InvalidateTag( this );

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

void
FOdysseyVectorTagInbetweener::ResizeChart( bool iResetSpacing )
{
    mChart.Resize( iResetSpacing );
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
FOdysseyVectorTagInbetweener::DeformPathsAtTarget()
{
    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        // deform the path according to grid geometry
        breakdown->GetGrid()->DeformPaths( mInterpolatedPathBuffer
                                         , breakdown->GetTargetDrawingIndex()
                                         , eInbetweenerPointPositionType::TargetPosition );
    }
}

void
FOdysseyVectorTagInbetweener::DispatchInbetweensToBreakdowns()
{
    double breakdownFirstSpacing = 0.0f;

    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        uint32 sourceDrawingIndex = breakdown->GetSourceDrawingIndex();
        uint32 targetDrawingIndex = breakdown->GetTargetDrawingIndex();
        double breakdownLastSpacing = ( breakdown->GetMasterBreakdown() == nullptr ) ? 1.0f
                                                                                     : mChart.GetDrawing( targetDrawingIndex )->spacing;

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
    }
}

void
FOdysseyVectorTagInbetweener::DeformGridAtInbetween( uint32 iDrawingIndex )
{
    FInbetweenerDrawing* drawing = mChart.GetDrawing( iDrawingIndex );
    double t = mChart.GetDrawing( iDrawingIndex )->breakdownSpacing;
    //::ULIS::FRectD bbox = mSourceBBox;

    if( mInterpolationType == eInbetweenerInterpolationType::Linear )
    {
        for( FInbetweenerPoint& point : drawing->breakdown->GetGrid()->GetPointBuffer() )
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
        drawing->breakdown->GetGrid()->ComputeARAPInterpolation( drawing, false );
    }
}

void
FOdysseyVectorTagInbetweener::DeformPathsAtInbetween( uint32 iDrawingIndex )
{
    FInbetweenerDrawing* drawing = mChart.GetDrawing( iDrawingIndex );
    double t = mChart.GetDrawing( iDrawingIndex )->breakdownSpacing;

    DeformGridAtInbetween( iDrawingIndex );

    // deform the path according to grid geometry
    drawing->breakdown->GetGrid()->DeformPaths( mInterpolatedPathBuffer
                                              , iDrawingIndex
                                              , eInbetweenerPointPositionType::InterpPosition );
}

uint32
FOdysseyVectorTagInbetweener::GetDrawingCount()
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
    RedrawAnimationCells( GetDrawingCount() );
}


void
FOdysseyVectorTagInbetweener::RedrawAnimationCells( uint32 iDrawingCount )
{
    // scene could be non existent when the tag's owner is removed, as it would still trigger call to Update()
    // right after the removal of an object in the hierarchy.
    if( mOwner->GetSharedEnv() )
    {
        IOdysseyVectorAnimationCell* animationCell = mOwner->GetScene()->GetEngine()->GetAnimationCell();

        if( animationCell )
        {
            int32 animationCellIndex = animationCell->GetIndex();

            // Redraw impacted cells
            for( uint32 i = 1; ( i < iDrawingCount ) && ( animationCell != nullptr ); i++ )
            {
                IOdysseyVectorAnimationCell* nextAnimationCell = animationCell->GetCellByIndex( animationCellIndex + i );

                if( nextAnimationCell )
                {
                    //nextAnimationCell->GetEngine()->Invalidate();
                    nextAnimationCell->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
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

        for( uint32 i = ( sourceDrawingIndex + 1 ); i < targetDrawingIndex; i++ )
        {
            DeformPathsAtInbetween( i );
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
            uint32 tagCellIndex = tagCell->GetIndex();
            uint32 displayedCellIndex = displayedCell->GetIndex();

            if ( ( displayedCellIndex >   tagCellIndex                         )
              && ( displayedCellIndex < ( tagCellIndex + GetDrawingCount() - 1 ) ) )
            {
                DrawPathsInbetween( displayedCellIndex - tagCellIndex , iBLContext );
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
FOdysseyVectorTagInbetweener::DrawPathsInbetween( uint32 iDrawingIndex
                                                , BLContext* iBLContext )
{
    BLMatrix2D worldMatrix = mOwner->GetWorldMatrix();

    iBLContext->save();
    iBLContext->resetMatrix();

    // passed to DrawPathAt()
    worldMatrix.transform( mChart.GetDrawing( iDrawingIndex )->localMatrix );

    for( FInterpolatedPath& interpolatedPath : mInterpolatedPathBuffer )
    {
        uint32 pointCount = interpolatedPath.mInterpolatedPointBuffer.size();
        ::ULIS::FVec2D* pointPositionBuffer = &interpolatedPath.mInterpolatedPointPositionBuffer[pointCount * iDrawingIndex];

        DrawPathAt( &interpolatedPath
                  , pointPositionBuffer
                  , worldMatrix
                  , iBLContext );
    }

    iBLContext->restore();
}

void
FOdysseyVectorTagInbetweener::MoveInbetween( FInbetweenerDrawing* iDrawing
                                           , float iNewSpacing
                                           , bool iRelative )
{
    if( ( iNewSpacing > 0.0f ) && ( iNewSpacing < 1.0f ) )
    {
        uint32 drawingCount = GetDrawingCount();
        IOdysseyVectorAnimationCell* animationCell = mOwner->GetScene()->GetEngine()->GetAnimationCell();
        int32  prevIndex  = iDrawing->GetIndex() - 1;
        uint32 nextIndex  = iDrawing->GetIndex() + 1;
        float prevSpacing = prevIndex > -1 ? mChart.GetDrawing( prevIndex )->spacing : 0.0f;
        float nextSpacing = nextIndex < drawingCount ? mChart.GetDrawing( nextIndex )->spacing : 1.0f;

        if( iRelative == false )
        {
            if( ( iNewSpacing > prevSpacing )
             && ( iNewSpacing < nextSpacing ) )
            {
                iDrawing->spacing = iNewSpacing;

                // recompute single inbetweens
                //InterpolateTransform( iDrawing->index );
                //DeformPathsAtInbetween( iDrawing->index );

                //Interpolate();
            }
        }
        else
        {
            for( uint32 i = 1; i < ( drawingCount - 1 ); i++ )
            {
                FInbetweenerDrawing* otherInbetween = mChart.GetDrawing( i );

                if( otherInbetween != iDrawing )
                {
                    double otherInbetweenOldSpacing = otherInbetween->spacing;

                    if( otherInbetween->spacing < iDrawing->spacing )
                    {
                        float length = iDrawing->spacing;
                        float ratio = iDrawing->spacing ? ( otherInbetween->spacing / length ) : 0.0f;
                        float newLength = iNewSpacing;

                        otherInbetween->spacing = newLength * ratio;
                    }
                    else
                    {
                        float length = 1.0f - iDrawing->spacing;
                        float ratio = iDrawing->spacing ? ( ( otherInbetween->spacing - iDrawing->spacing ) / length ) : 0.0f;
                        float newLength = 1.0f - iNewSpacing;

                        otherInbetween->spacing = iNewSpacing + ( newLength * ratio );
                    }
                }
            }

            iDrawing->spacing = iNewSpacing;

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
        interpolatedPath.mInterpolatedPointPositionBuffer.resize( GetDrawingCount() * pointCount );
    }
}

void
FOdysseyVectorTagInbetweener::ResetGrid()
{
    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        breakdown->GetGrid()->Make();
    }
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
              | INVALIDATE_BUFFERS
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


    for( uint32 drawingIndex = 1; drawingIndex < ( GetDrawingCount() - 1 ); drawingIndex++ )
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
                    FInbetweenerDrawing* drawing = inbetweenerTag->mChart.GetDrawing( drawingIndex );

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
                    FInbetweenerDrawing* drawing = inbetweenerTag->mChart.GetDrawing( drawingIndex );

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
