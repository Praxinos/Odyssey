#include "Tools/VectorPathPushTool/OdysseyPainterEditorVectorPathPushToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorPathPushToolHUD::~FOdysseyPainterEditorVectorPathPushToolHUD()
{
}

FOdysseyPainterEditorVectorPathPushToolHUD::FOdysseyPainterEditorVectorPathPushToolHUD( UOdysseyPainterEditorVectorPathPushTool* iPathPushTool )
    : mPathPushTool( iPathPushTool )
    , mX( 0.0f )
    , mY( 0.0f )
{

}

void
FOdysseyPainterEditorVectorPathPushToolHUD::Reset(FOdysseyVectorScene* iScene)
{
}

void
FOdysseyPainterEditorVectorPathPushToolHUD::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    blctx->save();
    blctx->resetMatrix();

    if( mPathPushTool->RestrictToSelection )
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

    blctx->setStrokeStyle( fgColor );
    blctx->setStrokeWidth( 1.0f );
    blctx->strokeCircle( mX, mY, mPathPushTool->Radius );

    blctx->restore();
}

void
FOdysseyPainterEditorVectorPathPushToolHUD::SetCursorPosition( double iX, double iY )
{
    mX = iX;
    mY = iY;
}
