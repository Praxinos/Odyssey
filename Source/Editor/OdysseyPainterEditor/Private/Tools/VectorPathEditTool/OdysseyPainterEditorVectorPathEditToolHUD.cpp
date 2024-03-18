#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditToolHUD.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyPainterEditor.h"

FOdysseyPainterEditorVectorPathEditToolHUD::~FOdysseyPainterEditorVectorPathEditToolHUD()
{
}

FOdysseyPainterEditorVectorPathEditToolHUD::FOdysseyPainterEditorVectorPathEditToolHUD( UOdysseyPainterEditorVectorPathEditTool* iPathEditTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iPathEditTool )
    , mPathEditTool( iPathEditTool )
{
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::Reset( FOdysseyVectorGroupPaint* iScene )
{
    uint64 hudFlags = mPathEditTool->GetEditor()->GetVectorHUDFlags();

    // Updates the selection box
    UpdateSelectionBox( iScene, false, hudFlags );

    //MakePointQuadTree( iScene, hudFlags );
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::Load( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    uint32 width = vectorEngine->GetPreferredWidth();
    uint32 height = vectorEngine->GetPreferredHeight();

    mBLSelectionMask.create( width, height, BL_FORMAT_A8 );

    mBLSelectionContext.begin( mBLSelectionMask );
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::Unload( FOdysseyVectorGroupPaint* iScene )
{
    mBLSelectionContext.end();
}

BLImage*
FOdysseyPainterEditorVectorPathEditToolHUD::GetMask()
{
    return &mBLSelectionMask;
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::ClearMask()
{
    mBLSelectionContext.save();

    mBLSelectionContext.setCompOp( BL_COMP_OP_SRC_COPY );
    mBLSelectionContext.setFillAlpha( 0.0f );
    mBLSelectionContext.clearAll();
    mBLSelectionContext.flush( BL_CONTEXT_FLUSH_SYNC );

    mBLSelectionContext.restore();
}

::ULIS::FRectD
FOdysseyPainterEditorVectorPathEditToolHUD::GenerateMask( double iX
                                                        , double iY
                                                        , double iRadius )
{
    mBLSelectionContext.save();

    mBLSelectionContext.setCompOp( BL_COMP_OP_SRC_COPY );
    mBLSelectionContext.setFillAlpha( 1.0f );
    mBLSelectionContext.fillCircle( iX, iY, iRadius );
    mBLSelectionContext.flush( BL_CONTEXT_FLUSH_SYNC );

    mBLSelectionContext.restore();

    return ::ULIS::FRectD::FromMinMax( iX - iRadius, iY - iRadius
                                     , iX + iRadius, iY + iRadius );
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::Draw( BLContext* iBLContext
                                                , FOdysseyVectorGroupPaint* iScene )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    ePathPickingMode pickingMode = mPathEditTool->GetPickingMode();
    uint64 vertexHandleFlag  = ( pickingMode == ePathPickingMode::VertexHandle  ) ? HUD_PATH_VERTEX_HANDLE  : 0;
    uint64 segmentHandleFlag = ( pickingMode == ePathPickingMode::SegmentHandle ) ? HUD_PATH_SEGMENT_HANDLE
                                                                                  | HUD_PATH_VERTEX_ALIGNMENT : 0;
    uint64 hudFlags = mPathEditTool->GetEditor()->GetVectorHUDFlags();

    // draw object details in any mode (if statement is useles per-se but here for clarity)
    if( ( hudFlags & HUD_MODE_VERTEX ) || ( hudFlags & HUD_MODE_OBJECT ) )
    {
        DrawObjects( iBLContext
                   , iScene
                   , fgColor
                   , bgColor
                   , hcColor
                   , hudFlags
                   | HUD_PATH_VERTEX
                   | HUD_PATH_SEGMENT
                   | vertexHandleFlag
                   | segmentHandleFlag );
    }

    // draw selection box only if we restrict erasure to the selection 
    if( iScene->GetEngine()->GetSelectedObjectList().size() )
//    {
//        DrawSelectionBox( iBLContext, iScene, fgColor, bgColor, hcColor, hudFlags );
//    }

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
