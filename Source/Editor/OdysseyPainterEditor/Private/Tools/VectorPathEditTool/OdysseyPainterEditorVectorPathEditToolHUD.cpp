#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorPathEditToolHUD::~FOdysseyPainterEditorVectorPathEditToolHUD()
{
}

FOdysseyPainterEditorVectorPathEditToolHUD::FOdysseyPainterEditorVectorPathEditToolHUD( UOdysseyPainterEditorVectorPathEditTool* iPathEditTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iPathEditTool )
    , mPathEditTool( iPathEditTool )
{
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::Reset( FOdysseyVectorScene* iScene )
{
    // Updates the selection box
    UpdateSelectionBox( iScene, mPathEditTool->GetEditor()->GetVectorEditionFlags() );
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::Load( FOdysseyVectorScene* iScene )
{
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::Unload( FOdysseyVectorScene* iScene )
{
}

// tells in which case an object is displayed by the tool
bool
FOdysseyPainterEditorVectorPathEditToolHUD::IsObjectDisplayed( FOdysseyVectorScene* iScene
                                                             , FOdysseyVectorObject* iObject
                                                             , uint64 iHUDFlags )
{
    if( mPathEditTool->RestrictToSelectedObjects == false )
    {
        return true;
    }

    if( ( mPathEditTool->RestrictToSelectedObjects == true ) && iObject->IsSelected() )
    {
        return true;
    }

    return false;
}

// tells in which case an object is altered by the tool
bool
FOdysseyPainterEditorVectorPathEditToolHUD::IsObjectAltered( FOdysseyVectorScene* iScene
                                                           , FOdysseyVectorObject* iObject
                                                           , uint64 iHUDFlags )
{
    if( mPathEditTool->RestrictToSelectedObjects == false )
    {
        return true;
    }

    if( ( mPathEditTool->RestrictToSelectedObjects == true ) && iObject->IsSelected() )
    {
        return true;
    }

    return false;
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::Draw( BLContext* iBLContext
                                                , FOdysseyVectorScene* iScene )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    ePathPickingMode pickingMode = mPathEditTool->GetPickingMode();
    uint64 vertexHandleFlag  = ( pickingMode == ePathPickingMode::VertexHandle  ) ? VIEW_PATH_VERTEX_HANDLE  : 0;
    uint64 segmentHandleFlag = ( pickingMode == ePathPickingMode::SegmentHandle ) ? VIEW_PATH_SEGMENT_HANDLE
                                                                                  | VIEW_PATH_VERTEX_ALIGNMENT : 0;
    uint64 hudFlags = mPathEditTool->GetEditor()->GetVectorEditionFlags();

    // draw object details in any mode (if statement is useles per-se but here for clarity)
    if( ( hudFlags & VIEW_MODE_VERTEX ) || ( hudFlags & VIEW_MODE_OBJECT ) )
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
                                      | vertexHandleFlag
                                      | segmentHandleFlag );
    }

    // draw selection box only if we restrict erasure to the selection 
/*
    if( mPathEditTool->RestrictToSelection )
    {
        DrawSelectionBox( iBLContext, iScene, fgColor, bgColor, hcColor, hudFlags );
    }
*/

    iBLContext->save();

    iBLContext->setStrokeStyle( hcColor );
    iBLContext->setStrokeWidth( 1.0f );
    iBLContext->strokeCircle( mX, mY, mPathEditTool->PickingRadius );

    iBLContext->restore();
}

bool
FOdysseyPainterEditorVectorPathEditToolHUD::SetCursorPosition( double iX, double iY )
{
    bool needsFullRedrawing = false;

    mX = iX;
    mY = iY;

    return needsFullRedrawing;
}
