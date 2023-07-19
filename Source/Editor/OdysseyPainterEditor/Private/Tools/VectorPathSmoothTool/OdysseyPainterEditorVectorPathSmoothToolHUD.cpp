#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorPathSmoothToolHUD::~FOdysseyPainterEditorVectorPathSmoothToolHUD()
{
}

FOdysseyPainterEditorVectorPathSmoothToolHUD::FOdysseyPainterEditorVectorPathSmoothToolHUD( UOdysseyPainterEditorVectorPathSmoothTool* iPathSmoothTool )
    : mPathSmoothTool( iPathSmoothTool )
    , mX( 0.0f )
    , mY( 0.0f )
{
}

void
FOdysseyPainterEditorVectorPathSmoothToolHUD::Reset( FOdysseyVectorScene* iScene )
{
    mPickedPointArray.clear();
    mPickedPointArray.reserve( 50 );

    MakePointQuadTree( iScene, mPathSmoothTool->RestrictToSelection );
}

void
FOdysseyPainterEditorVectorPathSmoothToolHUD::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    blctx->save();
    blctx->resetMatrix();

    if( mPathSmoothTool->RestrictToSelection )
    {
        std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();

        for( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
        {
            FOdysseyVectorObject* selectedObject = (*it);

            FOdysseyVectorHUD::DrawObjectRecursive( selectedObject, blctx );
        }
    }
    else
    {
        FOdysseyVectorHUD::DrawObjectRecursive( iScene, blctx );
    }

    blctx->setStrokeStyle( hcColor );
    blctx->setStrokeWidth( 1.0f );
    blctx->strokeCircle( mX, mY, mPathSmoothTool->PickingRadius );

    blctx->restore();
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
