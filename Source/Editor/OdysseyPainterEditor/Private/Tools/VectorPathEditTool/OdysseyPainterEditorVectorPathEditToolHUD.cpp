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
FOdysseyPainterEditorVectorPathEditToolHUD::Unload( FOdysseyVectorScene* iScene )
{
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::Draw( BLContext* iBLContext
                                                , FOdysseyVectorScene* iScene
                                                , uint64 iFlags )
{
    std::list<FOdysseyVectorObject*>& focusedObjectList = mPathEditTool->GetFocusedObjectList( iScene );
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

    iBLContext->save();

    iBLContext->setStrokeStyle( hcColor );
    iBLContext->strokeCircle( mX, mY, mPathEditTool->PickingRadius );

    for( FOdysseyVectorObject* focusedObject : focusedObjectList )
    {
        if( focusedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(focusedObject);

            FOdysseyVectorHUD::DrawPath( iBLContext
                                       , path
                                       , fgColor
                                       , bgColor
                                       , hcColor
                                       , true // world
                                       , VIEW_VERTEX | VIEW_SEGMENT | vertexHandleFlag | segmentHandleFlag );
        }

        if( focusedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(focusedObject);

            FOdysseyVectorHUD::DrawPaintGroup( iBLContext
                                             , paintGroup
                                             , fgColor
                                             , bgColor
                                             , hcColor
                                             , true // world
                                             , VIEW_VERTEX | VIEW_SEGMENT | vertexHandleFlag | segmentHandleFlag );
        }
    }

    iBLContext->restore();
}

bool
FOdysseyPainterEditorVectorPathEditToolHUD::SetCursorPosition( double iX, double iY )
{
    bool needsFullRedrawing = false;

    mX = iX;
    mY = iY;

    return needsFullRedrawing;
}
