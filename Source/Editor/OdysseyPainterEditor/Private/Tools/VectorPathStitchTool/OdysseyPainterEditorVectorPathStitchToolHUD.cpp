#include "Tools/VectorPathStitchTool/OdysseyPainterEditorVectorPathStitchToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorPathStitchToolHUD::~FOdysseyPainterEditorVectorPathStitchToolHUD()
{
}

FOdysseyPainterEditorVectorPathStitchToolHUD::FOdysseyPainterEditorVectorPathStitchToolHUD( UOdysseyPainterEditorVectorPathStitchTool* iPathStitchTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iPathStitchTool )
    , mPathStitchTool( iPathStitchTool )
{
}

void
FOdysseyPainterEditorVectorPathStitchToolHUD::Reset( FOdysseyVectorScene* iScene )
{
    uint64 hudFlags = GetViewingMode();

    mPickedPointArray.clear();

    MakePointQuadTree( iScene, mPathStitchTool->RestrictToSelectedObjects );

    UpdateSelectionBox( iScene
                      , mPathStitchTool->GetSelectedObjectList( iScene )
                      , false
                      , hudFlags );
}

void
FOdysseyPainterEditorVectorPathStitchToolHUD::Load( FOdysseyVectorScene* iScene )
{
}

void
FOdysseyPainterEditorVectorPathStitchToolHUD::Unload( FOdysseyVectorScene* iScene )
{
}

std::vector<FOdysseyVectorPoint*>&
FOdysseyPainterEditorVectorPathStitchToolHUD::GetPickedPointArray()
{
    return mPickedPointArray;
}

void
FOdysseyPainterEditorVectorPathStitchToolHUD::SetPosition( double iWorldX, double iWorldY )
{
    mPickedPointArray.clear();

    mX = iWorldX;
    mY = iWorldY;

    PickPoints( iWorldX, iWorldY, mPathStitchTool->PickingRadius, mPickedPointArray );
}

void
FOdysseyPainterEditorVectorPathStitchToolHUD::Draw( BLContext* iBLContext
                                                  , FOdysseyVectorScene* iScene )
{
    uint64 hudFlags = GetViewingMode();
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );

    // Draw object details only in vertex mode
    if( hudFlags & VIEW_MODE_VERTEX )
    {
        // static call
        FOdysseyVectorHUD::DrawObjects( iBLContext
                                      , &mPathStitchTool->GetFocusedObjectList( iScene )
                                      , fgColor
                                      , bgColor
                                      , hcColor
                                      , hudFlags | VIEW_PATH_VERTEX | VIEW_PATH_SEGMENT );
    }

    // draw selection box only if we restrict erasure to the selection 
    if( mPathStitchTool->RestrictToSelectedObjects )
    {
        DrawSelectionBox( iBLContext, iScene, fgColor, bgColor, hcColor, hudFlags );
    }

    iBLContext->save();
    iBLContext->resetMatrix();

    iBLContext->setCompOp( BL_COMP_OP_SRC_COPY );

    iBLContext->setStrokeStyle( hcColor );
    iBLContext->setStrokeWidth( 1.0f );
    iBLContext->strokeCircle( mX, mY, mPathStitchTool->PickingRadius );

    if( mPickedPointArray.size() > 1 )
    {
        FOdysseyVectorVertex* vertex0 = static_cast<FOdysseyVectorVertex*>(mPickedPointArray[0]);
        FOdysseyVectorVertex* vertex1 = static_cast<FOdysseyVectorVertex*>(mPickedPointArray[1]);

        if( ( vertex0->GetSegmentCount() == 1 ) && ( vertex1->GetSegmentCount() == 1 ) )
        {
            DrawPath( iBLContext, vertex0->GetPath(), fgColor, bgColor, hcColor, true, VIEW_PATH_SEGMENT );
            DrawPath( iBLContext, vertex1->GetPath(), fgColor, bgColor, hcColor, true, VIEW_PATH_SEGMENT );

            DrawVertex( iBLContext, vertex0, fgColor, bgColor, hcColor, true, 0 );
            DrawVertex( iBLContext, vertex1, fgColor, bgColor, hcColor, true, 0 );
        }
    }

    iBLContext->restore();
}
