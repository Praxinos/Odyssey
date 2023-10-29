#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyPainterEditor.h"

FOdysseyPainterEditorVectorBaseToolHUD::~FOdysseyPainterEditorVectorBaseToolHUD()
{
}

FOdysseyPainterEditorVectorBaseToolHUD::FOdysseyPainterEditorVectorBaseToolHUD( UOdysseyPainterEditorVectorBaseTool* iBaseTool )
{
    mBaseTool = iBaseTool;
}

uint64
FOdysseyPainterEditorVectorBaseToolHUD::GetViewingMode()
{
    FOdysseyPainterEditor* painterEditor = mBaseTool->GetEditor();

    return ( painterEditor->GetVectorEditionMode() == eVectorEditionMode::Object ) ? VIEW_MODE_OBJECT
                                                                                   : VIEW_MODE_VERTEX;
}

/*
void
FOdysseyPainterEditorVectorBaseToolHUD::Draw( BLContext* iBLContext
                                            , FOdysseyVectorScene* iScene
                                            , uint64 iDrawingFlags )
{
    if( iDrawingFlags & VIEW_MODE_VERTEX )
    {
        FOdysseyVectorHUD::DrawObjets( iBLContext
                                     , iScene
                                     , iDrawingFlags );
    }

    iBLContext->save();
    iBLContext->resetMatrix();

    DrawSelectionBox( iBLContext, iScene, iDrawingFlags );

    iBLContext->restore();
}
*/