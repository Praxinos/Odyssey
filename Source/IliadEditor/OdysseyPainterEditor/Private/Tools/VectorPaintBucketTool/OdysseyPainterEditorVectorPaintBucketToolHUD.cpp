// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPaintBucketTool/OdysseyPainterEditorVectorPaintBucketToolHUD.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVectorRoot.h"
#include "OdysseyVectorBucket.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorCycle.h"

FOdysseyPainterEditorVectorPaintBucketToolHUD::~FOdysseyPainterEditorVectorPaintBucketToolHUD()
{
}

FOdysseyPainterEditorVectorPaintBucketToolHUD::FOdysseyPainterEditorVectorPaintBucketToolHUD( UOdysseyPainterEditorVectorPaintBucketTool* iPaintBucketTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iPaintBucketTool )
    , mPaintBucketTool( iPaintBucketTool )
{
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::Reset( FOdysseyVectorGroupPaint* iScene )
{
    UpdateSelectionBox( iScene, false, mPaintBucketTool->GetEditor()->GetVectorHUDFlags() );

    UpdateWorkingPaintgroupList( iScene );
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::Load( FOdysseyVectorGroupPaint* iScene )
{
    Reset( iScene );
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::Unload(FOdysseyVectorGroupPaint* iScene)
{
}

std::list<FOdysseyVectorGroupPaint*>&
FOdysseyPainterEditorVectorPaintBucketToolHUD::GetWorkingPaintgroupList()
{
    return mWorkingPaintgroupList;
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::UpdateWorkingPaintgroupList( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    mWorkingPaintgroupList.clear();

    FOdysseyVectorObject::Traverse
    ( iScene
    , 0
    , [ this
      , iScene
      , vectorEngine ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
        {
            if( iScene->GetRoot()->ObjectHasFocus( object, traversalFlags ) )
            {
                if( object->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
                {
                    FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(object);

                    mWorkingPaintgroupList.push_back( paintGroup );
                }
                else
                {
                    FOdysseyVectorGroupPaint* ancestor = static_cast<FOdysseyVectorGroupPaint*>(object->GetAncestorByClass( FOdysseyVectorGroupPaint::StaticClass() ));

                    if( ancestor )
                    {
                        if( std::find( mWorkingPaintgroupList.begin()
                                     , mWorkingPaintgroupList.end()
                                     , ancestor ) == mWorkingPaintgroupList.end() )
                        {
                            mWorkingPaintgroupList.push_back( ancestor );
                        }
                    }
                }

                return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED;
            }

            return 0;
        } );
}

uint32
FOdysseyPainterEditorVectorPaintBucketToolHUD::PickBucketArea( FOdysseyVectorBucket* iBucket
                                                             , double iWorldX
                                                             , double iWorldY )
{
    ::ULIS::FVec2D bucketWorldCoords = GetBucketPosition( iBucket, true );
    ::ULIS::FVec2D pickDif = ::ULIS::FVec2D( iWorldX - bucketWorldCoords.x
                                           , iWorldY - bucketWorldCoords.y );

    if( mPaintBucketTool->GetShowControls() )
    {
        if( iBucket->GetColorMode() == eBucketColorMode::LinearGradient )
        {
            ::ULIS::FVec2D handleWorldCoords = bucketWorldCoords + ( GetBucketHandleVector( iBucket, true ) * HANDLE_DISTANCE );
            ::ULIS::FVec2D handleDif = ::ULIS::FVec2D( iWorldX - handleWorldCoords.x
                                                     , iWorldY - handleWorldCoords.y );

            if ( handleDif.Distance() < mPaintBucketTool->PickingRadius )
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

        if ( pickDif.Distance() < mPaintBucketTool->PickingRadius )
        {
            return PICK_PROPAGATE;
        }
    }
    else
    {
        if ( pickDif.Distance() < mPaintBucketTool->PickingRadius )
        {
            return PICK_BUCKET;
        }
    }

    return PICK_NONE;
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::SetCursorPosition( double iWorldX
                                                                , double iWorldY )
{
    mCursorAt.x = iWorldX;
    mCursorAt.y = iWorldY;
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::PickCycles( FOdysseyVectorGroupPaint* iScene
                                                         , double iWorldX
                                                         , double iWorldY
                                                         , std::vector<FOdysseyVectorCycle*>& oPickedCycleArray )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    oPickedCycleArray.clear();

    for( FOdysseyVectorGroupPaint* paintgroup : mWorkingPaintgroupList )
    {
        FOdysseyVectorCycle* pickedCycle = paintgroup->PickCycle( iWorldX, iWorldY );

        if( pickedCycle )
        {
            oPickedCycleArray.push_back( pickedCycle );
        }
    }
}

FOdysseyVectorBucket*
FOdysseyPainterEditorVectorPaintBucketToolHUD::PickBucket( FOdysseyVectorGroupPaint* iScene
                                                         , double iWorldX
                                                         , double iWorldY )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    FOdysseyVectorBucket* pickedBucket = nullptr;

    for( FOdysseyVectorGroupPaint* paintgroup : mWorkingPaintgroupList )
    {
        std::list<FOdysseyVectorBucket*>& bucketList = paintgroup->GetBucketList();

        for( FOdysseyVectorBucket *bucket : bucketList )
        {
            if( PickBucketArea( bucket, iWorldX, iWorldY ) )
            {
                pickedBucket = bucket;

                return pickedBucket; // return now;
            }
        }
    }

    return nullptr;
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::Draw( BLContext* iBLContext
                                                   , FOdysseyVectorGroupPaint* iScene )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    uint64 viewBucketHandleFlag = mPaintBucketTool->GetShowControls() ? HUD_GROUPPAINT_BUCKET_HANDLE : 0;
    uint64 hudFlags = mPaintBucketTool->GetEditor()->GetVectorHUDFlags();
    std::vector<FOdysseyVectorCycle*> pickedCycleArray;

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX )
    {
        DrawObjects( iBLContext
                   , iScene
                   , fgColor
                   , bgColor
                   , hcColor
                   , hudFlags
                   | HUD_PATH_VERTEX
                   | HUD_PATH_SEGMENT
                   | HUD_GROUPPAINT_BUCKET
                   | viewBucketHandleFlag );
    }

    // Draw object details only in vertex mode
    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT )
    {
        DrawObjects( iBLContext
                   , iScene
                   , fgColor
                   , bgColor
                   , hcColor
                   , hudFlags
                   | HUD_GROUPPAINT_BUCKET
                   | viewBucketHandleFlag );
    }

    // draw selection box only if we restrict erasure to the selection
    if( iScene->GetRoot()->GetSelectedObjectList().size() )
    {
        DrawSelectionBox( iBLContext, iScene, fgColor, bgColor, hcColor, hudFlags );
    }

    iBLContext->save();
    iBLContext->resetMatrix();

    if( mPaintBucketTool->GetShowControls() == false )
    {
        PickCycles( iScene, mCursorAt.x, mCursorAt.y, pickedCycleArray );

        for( FOdysseyVectorCycle* cycle : pickedCycleArray)
        {
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
    }

    iBLContext->restore();
}
