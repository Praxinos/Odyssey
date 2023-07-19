#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorPathEditToolHUD::~FOdysseyPainterEditorVectorPathEditToolHUD()
{
}

FOdysseyPainterEditorVectorPathEditToolHUD::FOdysseyPainterEditorVectorPathEditToolHUD( UOdysseyPainterEditorVectorPathEditTool* iPathEditTool )
    : mPathEditTool( iPathEditTool )
{
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::Reset( FOdysseyVectorScene* iScene )
{
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    uint64 pickingFlags = mPathEditTool->GetPickingFlags();

    blctx->save();

    blctx->setStrokeStyle( hcColor );
    blctx->strokeCircle( mX, mY, mPathEditTool->PickingRadius );

    for( std::list<FOdysseyVectorObject*>::iterator oit = selectedObjectList.begin(); oit != selectedObjectList.end(); ++oit )
    {
        FOdysseyVectorObject* selectedObject = *oit;

        if( selectedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(selectedObject);
            uint64 vertexHandleFlag  = ( pickingFlags & FOdysseyVectorPath::PICK_HANDLE_POINT   ) ? VIEW_VERTEX_HANDLE   : 0;
            uint64 segmentHandleFlag = ( pickingFlags & FOdysseyVectorPath::PICK_HANDLE_SEGMENT ) ? VIEW_SEGMENT_HANDLE  : 0;

            FOdysseyVectorHUD::DrawPath( path
                                       , fgColor
                                       , bgColor
                                       , hcColor
                                       , true // world
                                       , VIEW_VERTEX | VIEW_SEGMENT | vertexHandleFlag | segmentHandleFlag );
        }
    }

    blctx->restore();
}

bool
FOdysseyPainterEditorVectorPathEditToolHUD::SetCursorPosition( double iX, double iY )
{
    bool needsFullRedrawing = false;

    mX = iX;
    mY = iY;

    return needsFullRedrawing;
}
