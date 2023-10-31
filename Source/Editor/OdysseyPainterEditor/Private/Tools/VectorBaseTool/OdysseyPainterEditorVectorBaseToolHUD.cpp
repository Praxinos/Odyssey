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

void
FOdysseyPainterEditorVectorBaseToolHUD::GetAlteredObjectList( FOdysseyVectorScene* iScene
                                                            , std::list<FOdysseyVectorObject*>& oObjectList )
{
    Traverse( iScene
            , iScene
            , mBaseTool->GetEditor()->GetVectorEditionFlags()
            , [ &oObjectList ]( FOdysseyVectorObject* object ) -> bool
              {
                  oObjectList.push_back( object);

                  return false; // keep traversing
              } );
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