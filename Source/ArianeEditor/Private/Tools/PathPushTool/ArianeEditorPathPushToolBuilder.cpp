// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor headers
#include "PathPushTool/ArianeEditorPathPushToolBuilder.h"
#include "PathPushTool/ArianeEditorPathPushTool.h"
#include "ArianeEditor.h"
#include "ArianeEditorStyle.h"

// Unreal headers
#include "InteractiveToolManager.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

UArianeEditorPathPushToolBuilder::UArianeEditorPathPushToolBuilder()
    : UArianeEditorToolBuilder( UArianeEditorPathPushTool::GetStaticType() )
{
}

UArianeEditorTool*
UArianeEditorPathPushToolBuilder::CreateTool( UInteractiveToolManager* ToolManager )
{
    Tool = NewObject<UArianeEditorPathPushTool>( ToolManager );

    return Tool;
}
