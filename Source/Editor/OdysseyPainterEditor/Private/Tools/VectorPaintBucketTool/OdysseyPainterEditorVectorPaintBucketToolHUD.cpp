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
    uint64 hudFlags = GetViewingMode();

    mPickedCycleArray.clear();

    UpdateSelectionBox( iScene
                      , mPaintBucketTool->GetSelectedObjectList( iScene )
                      , false
                      , hudFlags );
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

//static
void
FOdysseyPainterEditorVectorPaintBucketToolHUD::RecursivePickCycles( FOdysseyVectorObject* iObject
                                                                  , double iWorldX
                                                                  , double iWorldY
                                                                  , std::vector<FOdysseyVectorCycle*>& oPickedCycleArray )
{
    std::list<FOdysseyVectorObject*>& childrenList = iObject->GetChildrenList();

    if( iObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(iObject);
        FOdysseyVectorCycle* pickedCycle = paintGroup->PickCycle( iWorldX, iWorldY );

        if( pickedCycle )
        {
            oPickedCycleArray.push_back( pickedCycle );
        }
    }

    for( FOdysseyVectorObject* child : childrenList )
    {
        RecursivePickCycles( child, iWorldX, iWorldY, oPickedCycleArray );
    }
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::PickCycles( FOdysseyVectorScene* iScene
                                                         , double iWorldX
                                                         , double iWorldY
                                                         , std::vector<FOdysseyVectorCycle*>& oPickedCycleArray )
{
    std::list<FOdysseyVectorObject*>& focusedObjectList = mPaintBucketTool->GetFocusedObjectList( iScene );

    oPickedCycleArray.clear();

    for( FOdysseyVectorObject* focusedObject : focusedObjectList )
    {
        RecursivePickCycles( focusedObject, iWorldX, iWorldY, oPickedCycleArray );
    }
}

FOdysseyVectorBucket*
FOdysseyPainterEditorVectorPaintBucketToolHUD::RecursivePickBucket( FOdysseyVectorObject* iObject
                                                                  , double iWorldX
                                                                  , double iWorldY )
{
    std::list<FOdysseyVectorObject*>& childrenList = iObject->GetChildrenList();
    FOdysseyVectorBucket *pickedBucket = nullptr;

    if( iObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(iObject);
        std::list<FOdysseyVectorBucket*>& bucketList = paintGroup->GetBucketList();

        for( FOdysseyVectorBucket *bucket : bucketList )
        {
            if( PickBucketArea( bucket, iWorldX, iWorldY ) )
            {
                return bucket;
            }
        }
    }

    for( FOdysseyVectorObject *child : childrenList )
    {
        pickedBucket = RecursivePickBucket( child, iWorldX, iWorldY );

        if( pickedBucket )
            break;
    }

    return pickedBucket;
}

FOdysseyVectorBucket*
FOdysseyPainterEditorVectorPaintBucketToolHUD::PickBucket( FOdysseyVectorScene* iScene
                                                         , double iWorldX
                                                         , double iWorldY )
{
    std::list<FOdysseyVectorObject*>& focusedObjectList = mPaintBucketTool->GetFocusedObjectList( iScene );
    FOdysseyVectorBucket* pickedBucket = nullptr;

    for( FOdysseyVectorObject* focusedObject : focusedObjectList )
    {
        pickedBucket = RecursivePickBucket( focusedObject, iWorldX, iWorldY );

        if( pickedBucket )
        {
            return pickedBucket;
        }
    }

    return nullptr;
}

// must be set at eache mouse hover event for memory safety issues.
void
FOdysseyPainterEditorVectorPaintBucketToolHUD::SetPickedCycles( std::vector<FOdysseyVectorCycle*>& pickedCycleArray )
{
    mPickedCycleArray = pickedCycleArray;
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::Draw( BLContext* iBLContext
                                                   , FOdysseyVectorScene* iScene )
{
    uint64 hudFlags = GetViewingMode();
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    uint64 viewBucketHandleFlag = mPaintBucketTool->GetShowControls() ? VIEW_GROUPPAINT_BUCKET_HANDLE : 0;

    if( hudFlags & VIEW_MODE_VERTEX )
    {
        // static call
        FOdysseyVectorHUD::DrawObjects( iBLContext
                                      , &mPaintBucketTool->GetFocusedObjectList( iScene )
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
                                      , &mPaintBucketTool->GetFocusedObjectList( iScene )
                                      , fgColor
                                      , bgColor
                                      , hcColor
                                      , hudFlags
                                      | VIEW_GROUPPAINT_BUCKET
                                      | viewBucketHandleFlag );
    }

    // draw selection box only if we restrict erasure to the selection 
    if( mPaintBucketTool->RestrictToSelectedObjects )
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
