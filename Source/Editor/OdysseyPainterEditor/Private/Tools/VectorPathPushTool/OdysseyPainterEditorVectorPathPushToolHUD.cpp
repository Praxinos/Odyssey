#include "Tools/VectorPathPushTool/OdysseyPainterEditorVectorPathPushToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorPathPushToolHUD::~FOdysseyPainterEditorVectorPathPushToolHUD()
{
}

FOdysseyPainterEditorVectorPathPushToolHUD::FOdysseyPainterEditorVectorPathPushToolHUD( UOdysseyPainterEditorVectorPathPushTool* iPathPushTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iPathPushTool )
    , mPathPushTool( iPathPushTool )
    , mX( 0.0f )
    , mY( 0.0f )
{

}

void
FOdysseyPainterEditorVectorPathPushToolHUD::Reset(FOdysseyVectorScene* iScene)
{
    // Updates the selection box
    FOdysseyPainterEditorVectorBaseToolHUD::Reset( iScene );
}

void
FOdysseyPainterEditorVectorPathPushToolHUD::Load(FOdysseyVectorScene* iScene)
{
}

void
FOdysseyPainterEditorVectorPathPushToolHUD::Unload( FOdysseyVectorScene* iScene )
{
}

void
FOdysseyPainterEditorVectorPathPushToolHUD::Draw( BLContext* iBLContext
                                                , FOdysseyVectorScene* iScene
                                                , uint64 iDrawingFlags )
{
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );

    // Draw scene in object or vertex mode
    FOdysseyPainterEditorVectorBaseToolHUD::Draw( iBLContext, iScene, iDrawingFlags );

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    iBLContext->save();
/*
    iBLContext->resetMatrix();

    if( mPathPushTool->RestrictToSelection )
    {
        std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();

        for( FOdysseyVectorObject* selectedObject : selectedObjectList )
        {
            FOdysseyVectorHUD::DrawObjectRecursive( iBLContext, selectedObject );
        }
    }
    else
    {
        FOdysseyVectorHUD::DrawObjectRecursive( iBLContext, iScene );
    }
*/
    iBLContext->setStrokeStyle( hcColor );
    iBLContext->setStrokeWidth( 1.0f );
    iBLContext->strokeCircle( mX, mY, mPathPushTool->Radius );

    iBLContext->restore();
}

void
FOdysseyPainterEditorVectorPathPushToolHUD::SetCursorPosition( double iX, double iY )
{
    mX = iX;
    mY = iY;
}
