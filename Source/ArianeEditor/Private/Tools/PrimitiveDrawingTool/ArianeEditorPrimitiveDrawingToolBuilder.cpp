// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor headers
#include "PrimitiveDrawingTool/ArianeEditorPrimitiveDrawingToolBuilder.h"
#include "PrimitiveDrawingTool/ArianeEditorPrimitiveDrawingTool.h"
#include "ArianeEditor.h"
#include "ArianeEditorStyle.h"

// Unreal headers
#include "InteractiveToolManager.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

UArianeEditorPrimitiveDrawingToolBuilder::UArianeEditorPrimitiveDrawingToolBuilder()
    : UArianeEditorToolBuilder( UArianeEditorPrimitiveDrawingTool::GetStaticType() )
{
}

UArianeEditorTool*
UArianeEditorPrimitiveDrawingToolBuilder::CreateTool( UInteractiveToolManager* ToolManager )
{
    Tool = NewObject<UArianeEditorPrimitiveDrawingTool>( ToolManager );

    return Tool;
}
