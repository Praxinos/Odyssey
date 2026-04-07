// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "EraserTool/ArianeEditorEraserToolBuilder.h"
#include "EraserTool/ArianeEditorEraserTool.h"
// Unreal headers
#include "InteractiveToolManager.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

UArianeEditorEraserToolBuilder::UArianeEditorEraserToolBuilder()
    : UArianeEditorToolBuilder( UArianeEditorEraserTool::GetStaticType() )
{
}

UArianeEditorTool*
UArianeEditorEraserToolBuilder::CreateTool( UInteractiveToolManager* ToolManager )
{
    Tool = NewObject<UArianeEditorEraserTool>( ToolManager );

    return Tool;
}

#undef LOCTEXT_NAMESPACE
