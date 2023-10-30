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

    return painterEditor->GetVectorEditionFlags() & FOdysseyVectorHUD::VIEW_MODE_ALL;
}

bool
FOdysseyPainterEditorVectorBaseToolHUD::IsTargetObject( FOdysseyVectorScene* iScene
                                                      , FOdysseyVectorObject* iObject
                                                      , uint64 iHUDFlags )
{
    // if scene is empty, any object is a target for modification
    if( iScene->GetSelectedObjectList().size() == 0 )
    {
        return ( iHUDFlags & VIEW_RESTRICTTOSELECTION ) ? false : true;
    }
    else
    {
        if( iHUDFlags & VIEW_MODE_VERTEX )
        {
            if(  ( ( iHUDFlags & VIEW_RESTRICTTOSELECTION ) && iObject->IsSelected() )
              || ( ( iHUDFlags & VIEW_RESTRICTTOSELECTION ) == 0 )  )
            {
                return true;
            }
        }

        if( iHUDFlags & VIEW_MODE_OBJECT )
        {
            if(  ( ( iHUDFlags & VIEW_RESTRICTTOSELECTION ) && iObject->IsSelected() )
              || ( ( iHUDFlags & VIEW_RESTRICTTOSELECTION ) == 0 )  )
            {
                return true;
            }
        }
    }

    return false;
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