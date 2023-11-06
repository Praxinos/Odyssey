#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorPathSmoothToolHUD::~FOdysseyPainterEditorVectorPathSmoothToolHUD()
{
}

FOdysseyPainterEditorVectorPathSmoothToolHUD::FOdysseyPainterEditorVectorPathSmoothToolHUD( UOdysseyPainterEditorVectorPathSmoothTool* iPathSmoothTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iPathSmoothTool )
    , mPathSmoothTool( iPathSmoothTool )
    , mX( 0.0f )
    , mY( 0.0f )
{
}

void
FOdysseyPainterEditorVectorPathSmoothToolHUD::Reset( FOdysseyVectorScene* iScene )
{
    uint64 hudFlags = mPathSmoothTool->GetEditor()->GetVectorHUDFlags();

    mPickedPointArray.clear();
    mPickedPointArray.reserve( 50 );

    MakePointQuadTree( iScene, hudFlags );

    UpdateSelectionBox( iScene, false, hudFlags );
}

void
FOdysseyPainterEditorVectorPathSmoothToolHUD::Load( FOdysseyVectorScene* iScene )
{
}

void
FOdysseyPainterEditorVectorPathSmoothToolHUD::Unload( FOdysseyVectorScene* iScene )
{
}

void
FOdysseyPainterEditorVectorPathSmoothToolHUD::Draw( BLContext* iBLContext
                                                  , FOdysseyVectorScene* iScene )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    uint64 hudFlags = mPathSmoothTool->GetEditor()->GetVectorHUDFlags();

    // Draw object details only in vertex mode
    if( hudFlags & HUD_MODE_VERTEX )
    {
        DrawObjects( iBLContext
                   , iScene
                   , fgColor
                   , bgColor
                   , hcColor
                   , hudFlags | HUD_PATH_VERTEX | HUD_PATH_SEGMENT );
    }

    // draw selection box only if we restrict erasure to the selection 
    if( mPathSmoothTool->RestrictToSelectedObjects  && iScene->GetSelectedObjectList().size() )
    {
        DrawSelectionBox( iBLContext, iScene, fgColor, bgColor, hcColor, hudFlags );
    }

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    iBLContext->save();

    iBLContext->setStrokeStyle( hcColor );
    iBLContext->setStrokeWidth( 1.0f );
    iBLContext->strokeCircle( mX, mY, mPathSmoothTool->PickingRadius );

    iBLContext->restore();
}

void
FOdysseyPainterEditorVectorPathSmoothToolHUD::SetCursorPosition( double iWorldX, double iWorldY )
{
    mX = iWorldX;
    mY = iWorldY;

    mPickedPointArray.clear();

    PickPoints( iWorldX, iWorldY, mPathSmoothTool->PickingRadius, mPickedPointArray );
}

std::vector<FOdysseyVectorPoint*>&
FOdysseyPainterEditorVectorPathSmoothToolHUD::GetPickedPointArray()
{
    return mPickedPointArray;
}
