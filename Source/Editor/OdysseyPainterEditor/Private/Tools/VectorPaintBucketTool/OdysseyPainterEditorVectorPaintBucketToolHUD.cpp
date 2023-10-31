#include "Tools/VectorPaintBucketTool/OdysseyPainterEditorVectorPaintBucketToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorPaintBucketToolHUD::~FOdysseyPainterEditorVectorPaintBucketToolHUD()
{
}

FOdysseyPainterEditorVectorPaintBucketToolHUD::FOdysseyPainterEditorVectorPaintBucketToolHUD( UOdysseyPainterEditorVectorPaintBucketTool* iPaintBucketTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iPaintBucketTool )
    , mPaintBucketTool( iPaintBucketTool )
{
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::Reset( FOdysseyVectorScene* iScene )
{
    mPickedCycleArray.clear();

    UpdateSelectionBox( iScene, false, mPaintBucketTool->GetEditor()->GetVectorEditionFlags() );
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::Load(FOdysseyVectorScene* iScene)
{
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::Unload(FOdysseyVectorScene* iScene)
{
}

uint32
FOdysseyPainterEditorVectorPaintBucketToolHUD::PickBucketArea( FOdysseyVectorBucket* iBucket
                                                             , double iWorldX
                                                             , double iWorldY )
{
    ::ULIS::FVec2D bucketWorldCoords = GetBucketPosition( iBucket, true );

    if( mPaintBucketTool->GetShowControls() )
    {
        if( iBucket->GetColorMode() == eBucketColorMode::LinearGradient )
        {
            ::ULIS::FVec2D handleWorldCoords = bucketWorldCoords + ( GetBucketHandleVector( iBucket, true ) * HANDLE_DISTANCE );
            ::ULIS::FVec2D pickDif = ::ULIS::FVec2D( iWorldX - handleWorldCoords.x
                                                   , iWorldY - handleWorldCoords.y );

            if ( pickDif.Distance() < mPaintBucketTool->PickingRadius )
            {
                return PICK_HANDLE;
            }
        }

        if( iBucket->GetColorMode() == eBucketColorMode::RadialGradient )
        {
            ::ULIS::FVec2D radialHandleWorldCoords = GetBucketRadialHandlePosition( iBucket, true );
            ::ULIS::FVec2D radialHandleDif = ::ULIS::FVec2D( iWorldX - radialHandleWorldCoords.x
                                                           , iWorldY - radialHandleWorldCoords.y );
            ::ULIS::FVec2D radialWorldCoords = GetBucketRadialPosition( iBucket, true );
            ::ULIS::FVec2D radialDif = ::ULIS::FVec2D( iWorldX - radialWorldCoords.x
                                                     , iWorldY - radialWorldCoords.y );

            if ( radialHandleDif.Distance() < mPaintBucketTool->PickingRadius )
            {
                return PICK_RADIAL_HANDLE;
            }

            if ( radialDif.Distance() < RADIAL_AREA_RADIUS )
            {
                return PICK_RADIAL_AREA;
            }
        }
    }
    else
    {
        ::ULIS::FVec2D pickDif = ::ULIS::FVec2D( iWorldX - bucketWorldCoords.x
                                               , iWorldY - bucketWorldCoords.y );

        if ( pickDif.Distance() < mPaintBucketTool->PickingRadius )
        {
            return PICK_BUCKET;
        }
    }

    return PICK_NONE;
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::PickCycles( FOdysseyVectorScene* iScene
                                                         , double iWorldX
                                                         , double iWorldY
                                                         , std::vector<FOdysseyVectorCycle*>& oPickedCycleArray )
{
    oPickedCycleArray.clear();

    Traverse( iScene
            , iScene
            , mPaintBucketTool->GetEditor()->GetVectorEditionFlags()
            , [ &iWorldX
              , &iWorldY
              , &oPickedCycleArray ]( FOdysseyVectorObject* object ) -> bool
              {
                  if( object->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
                  {
                      FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(object);
                      FOdysseyVectorCycle* pickedCycle = paintGroup->PickCycle( iWorldX, iWorldY );

                      if( pickedCycle )
                      {
                          oPickedCycleArray.push_back( pickedCycle );
                      }
                  }

                  return false; // keep traversing
              } );
}

FOdysseyVectorBucket*
FOdysseyPainterEditorVectorPaintBucketToolHUD::PickBucket( FOdysseyVectorScene* iScene
                                                         , double iWorldX
                                                         , double iWorldY )
{
    FOdysseyVectorBucket* pickedBucket = nullptr;

    Traverse( iScene
            , iScene
            , mPaintBucketTool->GetEditor()->GetVectorEditionFlags()
            , [ this
              , &iWorldX
              , &iWorldY
              , &pickedBucket ]( FOdysseyVectorObject* object ) -> bool
              {
                  if( object->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
                  {
                      FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(object);
                      std::list<FOdysseyVectorBucket*>& bucketList = paintGroup->GetBucketList();

                      for( FOdysseyVectorBucket *bucket : bucketList )
                      {
                          if( PickBucketArea( bucket, iWorldX, iWorldY ) )
                          {
                              pickedBucket = bucket;

                              return true; // stop traversing
                          }
                      }
                  }

                  return false; // keep traversing
              } );

    return pickedBucket;
}

// must be set at eache mouse hover event for memory safety issues.
void
FOdysseyPainterEditorVectorPaintBucketToolHUD::SetPickedCycles( std::vector<FOdysseyVectorCycle*>& pickedCycleArray )
{
    mPickedCycleArray = pickedCycleArray;
}

// tells in which case an object is altered by the tool
bool
FOdysseyPainterEditorVectorPaintBucketToolHUD::IsObjectDisplayed( FOdysseyVectorScene* iScene
                                                                , FOdysseyVectorObject* iObject
                                                                , uint64 iHUDFlags )
{
    if( iHUDFlags & VIEW_MODE_VERTEX )
    {
        if( iScene->GetSelectedObjectList().size() == false )
        {
            return true;
        }
        else
        {
            if ( iObject->IsSelected()  || IsPaintedPath( iObject, true ) )
            {
                return true;
            }
        }
    }

    if( iHUDFlags & VIEW_MODE_OBJECT )
    {
        if( iScene->GetSelectedObjectList().size() == false )
        {
            return true;
        }
        else
        {
            if ( iObject->IsSelected()  || IsPaintedPath( iObject, true ) )
            {
                return true;
            }
        }
    }

    return false;
}

// tells in which case an object is altered by the tool
bool
FOdysseyPainterEditorVectorPaintBucketToolHUD::IsObjectAltered( FOdysseyVectorScene* iScene
                                                              , FOdysseyVectorObject* iObject
                                                              , uint64 iHUDFlags )
{
    if( iScene->GetSelectedObjectList().size() == false )
    {
        return true;
    }
    else
    {
        if ( iObject->IsSelected() )
        {
            if( iObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
            {
                return true;
            }
        }
    }

    return false;
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::Draw( BLContext* iBLContext
                                                   , FOdysseyVectorScene* iScene )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    uint64 viewBucketHandleFlag = mPaintBucketTool->GetShowControls() ? VIEW_GROUPPAINT_BUCKET_HANDLE : 0;
    uint64 hudFlags = mPaintBucketTool->GetEditor()->GetVectorEditionFlags();

    if( hudFlags & VIEW_MODE_VERTEX )
    {
        // static call
        FOdysseyVectorHUD::DrawObjects( iBLContext
                                      , iScene
                                      , fgColor
                                      , bgColor
                                      , hcColor
                                      , hudFlags
                                      | VIEW_PATH_VERTEX
                                      | VIEW_PATH_SEGMENT
                                      | VIEW_GROUPPAINT_BUCKET
                                      | viewBucketHandleFlag );
    }

    // Draw object details only in vertex mode
    if( hudFlags & VIEW_MODE_OBJECT )
    {
        // static call
        FOdysseyVectorHUD::DrawObjects( iBLContext
                                      , iScene
                                      , fgColor
                                      , bgColor
                                      , hcColor
                                      , hudFlags
                                      | VIEW_GROUPPAINT_BUCKET
                                      | viewBucketHandleFlag );
    }

    // draw selection box only if we restrict erasure to the selection 
    if( iScene->GetSelectedObjectList().size() )
    {
        DrawSelectionBox( iBLContext, iScene, fgColor, bgColor, hcColor, hudFlags );
    }

    iBLContext->save();
    iBLContext->resetMatrix();

    for( int i = 0; i < mPickedCycleArray.size(); i++ )
    {
        FOdysseyVectorCycle* cycle = mPickedCycleArray[i];
        FOdysseyVectorObject* owner = cycle->GetOwner();
        BLMatrix2D& worldMatrix = owner->GetWorldMatrix();

        iBLContext->setMatrix( worldMatrix );

        iBLContext->setCompOp( BL_COMP_OP_SRC_OVER );
        iBLContext->setStrokeStyle( bgColor );
        iBLContext->setStrokeWidth( 4.0f );

        cycle->StrokePath( iBLContext, true );

        iBLContext->setStrokeStyle( hcColor );
        iBLContext->setStrokeWidth( 3.0f );

        cycle->StrokePath( iBLContext, true );
    }

    iBLContext->restore();
}
