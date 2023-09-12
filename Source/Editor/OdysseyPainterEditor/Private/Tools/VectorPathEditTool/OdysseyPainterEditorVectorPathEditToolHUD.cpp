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
FOdysseyPainterEditorVectorPathEditToolHUD::Load( FOdysseyVectorScene* iScene )
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
    ePathPickingMode pickingMode = mPathEditTool->GetPickingMode();
    uint64 vertexHandleFlag  = ( pickingMode == ePathPickingMode::VertexHandle  ) ? VIEW_VERTEX_HANDLE  : 0;
    uint64 segmentHandleFlag = ( pickingMode == ePathPickingMode::SegmentHandle ) ? VIEW_SEGMENT_HANDLE
                                                                                  | VIEW_VERTEX_ALIGNMENT : 0;

    blctx->save();

    blctx->setStrokeStyle( hcColor );
    blctx->strokeCircle( mX, mY, mPathEditTool->PickingRadius );

    for( std::list<FOdysseyVectorObject*>::iterator oit = selectedObjectList.begin(); oit != selectedObjectList.end(); ++oit )
    {
        FOdysseyVectorObject* selectedObject = *oit;

        if( selectedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(selectedObject);

            FOdysseyVectorHUD::DrawPath( path
                                       , fgColor
                                       , bgColor
                                       , hcColor
                                       , true // world
                                       , VIEW_VERTEX | VIEW_SEGMENT | vertexHandleFlag | segmentHandleFlag );
        }

        if( selectedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);

            FOdysseyVectorHUD::DrawPaintGroup( paintGroup
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
