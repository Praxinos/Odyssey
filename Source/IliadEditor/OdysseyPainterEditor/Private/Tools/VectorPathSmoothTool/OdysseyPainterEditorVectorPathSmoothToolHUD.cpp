// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothToolHUD.h"
#include "OdysseyPainterEditor.h"
// Vector engine
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroupPaint.h"

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
FOdysseyPainterEditorVectorPathSmoothToolHUD::Reset( FOdysseyVectorGroupPaint* iScene )
{
    uint64 hudFlags = mPathSmoothTool->GetEditor()->GetVectorHUDFlags();

    mPickedPointArray.clear();
    mPickedPointArray.reserve( 50 );

    MakePointQuadTree( iScene, false, hudFlags );

    UpdateSelectionBox( iScene, false, hudFlags );
}

void
FOdysseyPainterEditorVectorPathSmoothToolHUD::Load( FOdysseyVectorGroupPaint* iScene )
{
}

void
FOdysseyPainterEditorVectorPathSmoothToolHUD::Unload( FOdysseyVectorGroupPaint* iScene )
{
}

void
FOdysseyPainterEditorVectorPathSmoothToolHUD::Draw( BLContext* iBLContext
                                                  , FOdysseyVectorGroupPaint* iScene )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    uint64 hudFlags = mPathSmoothTool->GetEditor()->GetVectorHUDFlags();

    // Draw default
    // -> nothing in object mode.
    // -> vertices and segments in vertex mode.
    // -> inbetweens in inbetween mode.
    FOdysseyPainterEditorVectorBaseToolHUD::Draw( iBLContext, iScene );

    // draw selection box only if we restrict erasure to the selection
    if( mPathSmoothTool->RestrictToSelectedObjects  && iScene->GetEngine()->GetSelectedObjectList().size() )
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
