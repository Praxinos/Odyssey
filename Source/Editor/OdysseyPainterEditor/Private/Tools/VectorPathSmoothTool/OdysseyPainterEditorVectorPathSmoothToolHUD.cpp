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
    uint64 hudFlags = mPathSmoothTool->GetEditor()->GetVectorEditionFlags();

    mPickedPointArray.clear();
    mPickedPointArray.reserve( 50 );

    MakePointQuadTree( iScene, hudFlags );

    UpdateSelectionBox( iScene, hudFlags );
}

void
FOdysseyPainterEditorVectorPathSmoothToolHUD::Load( FOdysseyVectorScene* iScene )
{
}

void
FOdysseyPainterEditorVectorPathSmoothToolHUD::Unload( FOdysseyVectorScene* iScene )
{
}

// tells in which case an object is displayed by the tool
bool
FOdysseyPainterEditorVectorPathSmoothToolHUD::IsObjectDisplayed( FOdysseyVectorScene* iScene
                                                               , FOdysseyVectorObject* iObject
                                                               , uint64 iHUDFlags )
{
    if( mPathSmoothTool->RestrictToSelectedObjects == false )
    {
        return true;
    }

    if( ( mPathSmoothTool->RestrictToSelectedObjects == true ) && iObject->IsSelected() )
    {
        return true;
    }

    return false;
}

// tells in which case an object is altered by the tool
bool
FOdysseyPainterEditorVectorPathSmoothToolHUD::IsObjectAltered( FOdysseyVectorScene* iScene
                                                             , FOdysseyVectorObject* iObject
                                                             , uint64 iHUDFlags )
{
    if( mPathSmoothTool->RestrictToSelectedObjects == false )
    {
        return true;
    }

    if( ( mPathSmoothTool->RestrictToSelectedObjects == true ) && iObject->IsSelected() )
    {
        return true;
    }

    return false;
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
    uint64 hudFlags = mPathSmoothTool->GetEditor()->GetVectorEditionFlags();

    // Draw object details only in vertex mode
    if( hudFlags & VIEW_MODE_VERTEX )
    {
        // static call
        FOdysseyVectorHUD::DrawObjects( iBLContext
                                      , iScene
                                      , fgColor
                                      , bgColor
                                      , hcColor
                                      , hudFlags | VIEW_PATH_VERTEX | VIEW_PATH_SEGMENT );
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
