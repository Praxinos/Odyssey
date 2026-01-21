// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane
#include "PathDrawingTool/ArianeEditorPathDrawingTool.h"
#include "ArianeEditor.h"

UArianeEditorPathDrawingTool::~UArianeEditorPathDrawingTool()
{
}

UArianeEditorPathDrawingTool::UArianeEditorPathDrawingTool()
    : UArianeEditorTool( FContextMenuFunc::CreateUObject( this, &UArianeEditorPathDrawingTool::ExtendContextMenu ) )
{
}

bool
UArianeEditorPathDrawingTool::OnMouseDown( const FVector2D& iViewportCoords, const FKey& iKey, bool iRepeat )
{
    if( iKey == EKeys::LeftMouseButton )
    {

        return true;
    }

    return false;
}

void
UArianeEditorPathDrawingTool::OnMouseHover( const FVector2D& iViewportCoords )
{

}

void
UArianeEditorPathDrawingTool::OnMouseDrag( const FVector2D& iViewportCoords )
{
}

bool
UArianeEditorPathDrawingTool::OnMouseUp( const FVector2D& iViewportCoords, const FKey& iKey )
{
    if( iKey == EKeys::LeftMouseButton )
    {

        return true;
    }

    if( iKey == EKeys::RightMouseButton )
    {
        return true;
    }

    return false;
}

TSharedPtr<SWidget>
UArianeEditorPathDrawingTool::ExtendContextMenu( FMenuBuilder& menu )
{
    return SNullWidget::NullWidget
}
