// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "PathDrawingTool/ArianeEditorPathDrawingToolBuilder.h"
#include "PathDrawingTool/ArianeEditorPathDrawingTool.h"
#include "ArianeEditor.h"
// Odyssey
#include "OdysseyStyle.h"
// Unreal headers
#include "InteractiveToolManager.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

UArianeEditorPathDrawingToolBuilder::UArianeEditorPathDrawingToolBuilder()
    : UArianeEditorToolBuilder( UArianeEditorPathDrawingTool::GetStaticType() )
{
}

UArianeEditorTool*
UArianeEditorPathDrawingToolBuilder::CreateTool( UInteractiveToolManager* ToolManager )
{
    Tool = NewObject<UArianeEditorPathDrawingTool>( ToolManager );

    return Tool;
}
