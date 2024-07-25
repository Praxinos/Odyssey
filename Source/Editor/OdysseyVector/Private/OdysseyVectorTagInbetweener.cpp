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
                                                      , mInbetweenCount
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
    //mGrid->MapInterpolatedPaths( mInterpolatedPathBuffer );
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
                                  delete breakdown;

                                  return true;
                              } );
}

std::list<FInbetweenerRoute*>&
FOdysseyVectorTagInbetweener::GetRouteList()
{
    return mRouteList;
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
    //, mGrid( nullptr )
    , mGridType( eInbetweenerGridType::FFD )
    , mInterpolationType( eInbetweenerInterpolationType::ARAP )
    , mInbetweenCount( iInbetweenCount )
    , mInvalidationFlags( INVALIDATE_MAP
                        | INVALIDATE_BUFFERS
                        | INVALIDATE_SOURCEBBOX
                        | INVALIDATE_TARGET
                        | INVALIDATE_ROUTES
                        | INVALIDATE_SPACING
                        | INVALIDATE_CELLS )
    , mTargetTranslationX( 0.0f )
    , mTargetTranslationY( 0.0f )
    , mTargetScalingX    ( 1.0f )
    , mTargetScalingY    ( 1.0f )
    , mTargetRotation    ( 0.0f )
    , mColor ( 255, 0, 255, 255 )
    , bMapAsPolyline( false )
    , bShared ( false )
{
    ResetChart();
    // Note: Matrix needs chart to be allocated first.
    UpdateMatrix();

    mSourceBBox = iOwnerObject->GetBBox( false );

    mSourceBBox.x -= 0.01f;
    mSourceBBox.y -= 0.01f;
    mSourceBBox.w += 0.02f;
    mSourceBBox.h += 0.02f;


    //Update();
/*
    if( mSourceBBox.h > mSourceBBox.w )
    {
        double quadH = ( mSourceBBox.h / iNumQuadY );
        double quadW = quadH;

        mSourceBBox.w = iNumQuadX * quadW;
    }
    else
    {
        double quadW = ( mSourceBBox.w / iNumQuadX );
        double quadH = quadW;

        mSourceBBox.h = iNumQuadY * quadH;
    }
*/
    // Note: Grid building needs the bbox to be set.
    SetGrid( mGridType, iNumQuadX, iNumQuadY );

    // the default breakdown
    mBreakdownList.emplace_back( new FInbetweenerBreakdown( this, -1, iInbetweenCount ) );

    //mGrid->Make( iNumQuadX, iNumQuadY, iOwnerObject->GetBBox( false ), this );

    //Map();

    //AllocBuffers();
    //Interpolate();
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
    Share();
}

void FOdysseyVectorTagInbetweener::Removed()
{
    Unshare();
}

void
FOdysseyVectorTagInbetweener::Share()
{
    mSharedEnv->AddTag( this );

    bShared = true;
}

void
FOdysseyVectorTagInbetweener::Unshare()
{
    mSharedEnv->RemoveTag( this );

    bShared = false;
}

FInbetweenerBreakdown*
FOdysseyVectorTagInbetweener::AddBreakdown( int32 iInbetweenIndex )
{
    int32 sourceInbetweenIndex = -1;
    int32 targetInbetweenIndex = mInbetweenCount;
    uint32 breakdownIndex = 0;
    std::vector<::ULIS::FVec2D> curBreakdownSourceGeometry;

    std::list<FInbetweenerBreakdown*>::iterator
        curBreakdownIterator = std::find_if( mBreakdownList.begin()
                                           , mBreakdownList.end()
                                           , [&iInbetweenIndex]( FInbetweenerBreakdown* breakdown ) -> bool
                                             {
                                                 if( ( iInbetweenIndex > breakdown->GetSourceInbetweenIndex() )
                                                  && ( iInbetweenIndex < breakdown->GetTargetInbetweenIndex() ) )
                                                 {
                                                     return true;
                                                 }

                                                 return false;
                                             } );
    //if( curBreakdownIterator != mBreakdownList.end() )
    {
        FInbetweenerBreakdown* curBreakdown = *curBreakdownIterator;

        //newBreakdown.SetOffset( curBreakdown.GetFromOffset() );
        //newBreakdown.SetLength( toOffset );

        curBreakdown->SetSourceInbetweenIndex( iInbetweenIndex );

        curBreakdown->GetGrid()->GetGeometry( curBreakdownSourceGeometry, eInbetweenerPointPositionType::SourcePosition );

        targetInbetweenIndex = iInbetweenIndex;
    }

    FInbetweenerBreakdown* newBreakdown = new FInbetweenerBreakdown( this
                                                                   , sourceInbetweenIndex
                                                                   , targetInbetweenIndex );

    newBreakdown->GetGrid()->SetGeometry( curBreakdownSourceGeometry, eInbetweenerPointPositionType::SourcePosition );
    newBreakdown->GetGrid()->SetGeometry( curBreakdownSourceGeometry, eInbetweenerPointPositionType::TargetPosition );

    mBreakdownList.insert( curBreakdownIterator, newBreakdown );

    // we need that index to extract the correct trajectories from the routes.
    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        breakdown->SetIndex( breakdownIndex++ );
    }

    //DispatchInbetweensToBreakdowns();

    return newBreakdown;
}

std::list<FInbetweenerBreakdown*>&
FOdysseyVectorTagInbetweener::GetBreakdownList()
{
    return mBreakdownList;
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
            if( mSharedEnv->HasTag( this ) == false )
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
                mSourceBBox = mOwner->GetBBox( false );

                mSourceBBox.x -= 0.1f;
                mSourceBBox.y -= 0.1f;
                mSourceBBox.w += 0.2f;
                mSourceBBox.h += 0.2f;

                for( FInbetweenerBreakdown* breakdown : mBreakdownList )
                {
                    std::vector<::ULIS::FVec2D> sourceGeometry;
                    std::vector<::ULIS::FVec2D> targetGeometry;

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

        if( mInvalidationFlags & INVALIDATE_TARGET )
        {
            UpdateBBox( mTargetBBox, eInbetweenerPointPositionType::TargetPosition );
        }

        if( mInvalidationFlags & INVALIDATE_SOURCEBBOX )
        {
            //UpdateBBox( mSourceBBox, eInbetweenerPointPositionType::SourcePosition );
        }

        if( mInvalidationFlags & INVALIDATE_MAP )
        {
            Map();

            mInvalidationFlags |= INVALIDATE_BUFFERS;
        }

        if( mInvalidationFlags & INVALIDATE_BUFFERS )
        {
            AllocBuffers();
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

        for( FInbetweenerBreakdown* breakdown : mBreakdownList )
        {
            breakdown->GetGrid()->Update( iUpdateFlags, mInvalidationFlags );
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
    mChart.inbetweenBuffer.resize( mInbetweenCount );

    for( uint32 i = 0; i < mInbetweenCount; i++ )
    {
        mChart.inbetweenBuffer[i].spacing = spacing;
        mChart.inbetweenBuffer[i].index = i;

        spacing += step;
    }

    for( FInbetweenerRoute* route : mRouteList )
    {
        route->ResetSpacing();
    }

    Invalidate( INVALIDATE_SPACING | INVALIDATE_CELLS );
}

FInbetweenerChart&
FOdysseyVectorTagInbetweener::GetChart()
{
    return mChart;
}

void
FOdysseyVectorTagInbetweener::GetTargetTransform( double& oTranslationX
                                                , double& oTranslationY
                                                , double& oRotation
                                                , double& oScalingX
                                                , double& oScalingY )
{
    oTranslationX = mTargetTranslationX;
    oTranslationY = mTargetTranslationY;
    oRotation = mTargetRotation;
    oScalingX = mTargetScalingX;
    oScalingY = mTargetScalingY;
}

void
FOdysseyVectorTagInbetweener::SetTargetTransform( double iTranslationX
                                                , double iTranslationY
                                                , double iRotation
                                                , double iScalingX
                                                , double iScalingY )
{
    mTargetTranslationX = iTranslationX;
    mTargetTranslationY = iTranslationY;
    mTargetRotation = iRotation;
    mTargetScalingX = iScalingX;
    mTargetScalingY = iScalingY;

    UpdateMatrix( );
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

    Invalidate( INVALIDATE_CELLS | INVALIDATE_ROUTES | INVALIDATE_SPACING );
}

void
FOdysseyVectorTagInbetweener::DeformPathsAtTarget()
{
    for( FInbetweenerPoint& point : mBreakdownList.back()->GetGrid()->GetPointBuffer() )
    {
        if( point.GetQuadCount() )
        {
            point.mInterpPosition = point.mTargetPosition;
        }
    }

    // deform the path according to grid geometry
    mBreakdownList.back()->GetGrid()->DeformPaths( mInterpolatedPathBuffer, mInbetweenCount );
}

void
FOdysseyVectorTagInbetweener::DispatchInbetweensToBreakdowns()
{
    double breakdownFirstSpacing = 0.0f;

    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        int32 targetInbetweenIndex = breakdown->GetTargetInbetweenIndex();
        double breakdownLastSpacing = ( mBreakdownList.size() == 1 ) ? 1.0f : mChart.inbetweenBuffer[targetInbetweenIndex].spacing;

        for( int32 i = 0; i < (int32)mInbetweenCount; i++ )
        {
            FInbetweenerInbetween* inbetween = &mChart.inbetweenBuffer[i];

            if( ( i > breakdown->GetSourceInbetweenIndex() ) && ( i < breakdown->GetTargetInbetweenIndex() ) )
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
FOdysseyVectorTagInbetweener::DeformPathsAtInbetween( uint32 iInbetweenIndex )
{
    FInbetweenerInbetween* inbetween = &mChart.inbetweenBuffer[iInbetweenIndex];
    double t = mChart.inbetweenBuffer[iInbetweenIndex].breakdownSpacing;
    //::ULIS::FRectD bbox = mSourceBBox;

    if( mInterpolationType == eInbetweenerInterpolationType::Linear )
    {
        for( FInbetweenerPoint& point : inbetween->breakdown->GetGrid()->GetPointBuffer() )
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
        inbetween->breakdown->GetGrid()->ComputeARAPInterpolation( inbetween, false );
    }
/*
    if( mGridType == eInbetweenerGridType::FFD )
    {
        for( FInbetweenerPoint& point : mGrid->GetPointBuffer() )
        {
            point.SetU( ( point.mInterpPosition.x - bbox.x ) / bbox.w );
            point.SetV( ( point.mInterpPosition.y - bbox.y ) / bbox.h );
        }
    }
*/
    // deform the path according to grid geometry
    inbetween->breakdown->GetGrid()->DeformPaths( mInterpolatedPathBuffer, iInbetweenIndex );
}

void
FOdysseyVectorTagInbetweener::RedrawAnimationCells()
{
    RedrawAnimationCells( mInbetweenCount );
}


void
FOdysseyVectorTagInbetweener::RedrawAnimationCells( uint32 iInbetweenCount )
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
    inbetween->matrix.rotate( rotation * M_PI / 180.0f ); // convert to radians
    inbetween->matrix.scale( scalingX, scalingY );

    BLMatrix2D::invert( inbetween->inverseMatrix, inbetween->matrix );
}

void
FOdysseyVectorTagInbetweener::Interpolate()
{
    DispatchInbetweensToBreakdowns();

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
        uint32 fromIndex = breakdown->GetSourceInbetweenIndex() + 1;
        uint32 toIndex = breakdown->GetTargetInbetweenIndex() - 1;
        FInbetweenerInbetween* lastInbetween = &mChart.inbetweenBuffer[toIndex];

        for( uint32 i = fromIndex; i <= toIndex; i++ )
        {
            InterpolateTransform( i );
            DeformPathsAtInbetween( i );
        }
    }

    //UE_LOG( LogTemp, Warning, TEXT("Interpolate geometry"));
/*
    for( uint32 i = 0; i < mInbetweenCount; i++ )
    {
        InterpolateTransform( i );
        DeformPathsAtInbetween( i );
    }
*/
    //UE_LOG( LogTemp, Warning, TEXT("-------------------"));
}

uint32
FOdysseyVectorTagInbetweener::GetInbetweenCount()
{
    return mInbetweenCount;
}

/*
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
FOdysseyVectorTagInbetweener::DrawPathsInbetween( uint32 iInbetweenIndex
                                                , BLContext* iBLContext )
{
    BLMatrix2D worldMatrix = mOwner->GetWorldMatrix();

    iBLContext->save();
    iBLContext->resetMatrix();

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

    iBLContext->restore();
}

void
FOdysseyVectorTagInbetweener::DrawPathsTarget( BLContext* iBLContext )
{
    BLMatrix2D worldMatrix = mOwner->GetWorldMatrix();

    iBLContext->save();
    iBLContext->resetMatrix();

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

    iBLContext->restore();
}

void
FOdysseyVectorTagInbetweener::MoveInbetween( FInbetweenerInbetween* iInbetween
                                           , float iNewSpacing
                                           , bool iRelative )
{
    if( ( iNewSpacing > 0.0f ) && ( iNewSpacing < 1.0f ) )
    {
        IOdysseyVectorAnimationCell* animationCell = mOwner->GetScene()->GetEngine()->GetAnimationCell();
        int32 prevIndex  = iInbetween->index - 1;
        uint32 nextIndex = iInbetween->index + 1;
        float prevSpacing = prevIndex > -1 ? mChart.inbetweenBuffer[prevIndex].spacing : 0.0f;
        float nextSpacing = nextIndex < mInbetweenCount ? mChart.inbetweenBuffer[nextIndex].spacing : 1.0f;

        if( iRelative == false )
        {
            if( ( iNewSpacing > prevSpacing )
             && ( iNewSpacing < nextSpacing ) )
            {
                iInbetween->spacing = iNewSpacing;

                // recompute single inbetweens
                InterpolateTransform( iInbetween->index );
                DeformPathsAtInbetween( iInbetween->index );
            }
        }
        else
        {
            for( FInbetweenerInbetween& otherInbetween : mChart.inbetweenBuffer )
            {
                if( &otherInbetween != iInbetween )
                {
                    double otherInbetweenOldSpacing = otherInbetween.spacing;

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
FOdysseyVectorTagInbetweener::ResetGrid()
{
    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        breakdown->GetGrid()->Make();
    }
}

void
FOdysseyVectorTagInbetweener::SetInbetweenCount( uint32 iInbetweenCount )
{
    uint32 maxInbetweenCount = ::ULIS::FMath::Max( iInbetweenCount, mInbetweenCount );

    mInbetweenCount = iInbetweenCount;

    mBreakdownList.back()->SetTargetInbetweenIndex( iInbetweenCount );

    Invalidate( INVALIDATE_SPACING
              | INVALIDATE_BUFFERS
              | INVALIDATE_CELLS );

    ResetChart();


    RedrawAnimationCells( maxInbetweenCount );
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


    for( uint32 inbetweenIndex = 0; inbetweenIndex < mInbetweenCount; inbetweenIndex++ )
    {
        IOdysseyVectorAnimationCell* inbetweenAnimationCell = animationCell->GetCellByIndex( animationCellIndex + inbetweenIndex + 1 );

        if( inbetweenAnimationCell )
        {
            std::list<FOdysseyVectorObject*> newObjectList;

            // change vertices coords before copying the object
            std::function<uint64(FOdysseyVectorObject*,uint64)> preProcess = [ inbetweenIndex ]( FOdysseyVectorObject* vectorObject, uint64 copyFlags ) -> uint64
            {
                BLMatrix2D& ownerWorldMatrix = vectorObject->GetWorldMatrix();
                FOdysseyVectorTag* tag = vectorObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

                if( tag )
                {
                    FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);
                    FInbetweenerInbetween* inbetween = &inbetweenerTag->mChart.inbetweenBuffer[inbetweenIndex];

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
                        uint32 skippedOffset = ( inbetweenIndex * pointCount );

                        for( uint32 i = 0; i < interpolatedPath.mInterpolatedPointBuffer.size(); i++ )
                        {
                            FInterpolatedPoint* interpolatedPoint = &interpolatedPath.mInterpolatedPointBuffer[i];
                            ::ULIS::FVec2D* commitPosition = &interpolatedPath.mInterpolatedPointPositionBuffer[skippedOffset + i];
                            ::ULIS::FVec2D swapPosition = interpolatedPoint->mOriginalPoint->GetCoords();
                            ::ULIS::FVec2D transformedLocalPosition;
                            //double transformedLocalRadius;
                            //BLPoint transformedWorldPosition;

                            transformedLocalPosition = FOdysseyVector::MapPoint( inbetween->matrix, ::ULIS::FVec2D( commitPosition->x
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

            std::function<uint64(FOdysseyVectorObject*,FOdysseyVectorObject*,uint64)> postProcess = [ inbetweenIndex
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
                    FInbetweenerInbetween* inbetween = &inbetweenerTag->mChart.inbetweenBuffer[inbetweenIndex];

                    inbetween->matrix.reset();

                    // revert vertices coords after having copied the object.
                    // Coords were saved in the point position buffer
                    for( FInterpolatedPath& interpolatedPath : inbetweenerTag->mInterpolatedPathBuffer )
                    {
                        uint32 pointCount = interpolatedPath.mInterpolatedPointBuffer.size();
                        uint32 skippedOffset = ( inbetweenIndex * pointCount );

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
              | INVALIDATE_CELLS );
}
