// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor headers
#include "PathEditTool/ArianeEditorPathEditToolBuilder.h"
#include "PathEditTool/ArianeEditorPathEditTool.h"
#include "ArianeEditor.h"
#include "ArianeEditorStyle.h"

// Unreal headers
#include "InteractiveToolManager.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

UArianeEditorPathEditToolBuilder::UArianeEditorPathEditToolBuilder()
    : UArianeEditorToolBuilder( UArianeEditorPathEditTool::GetStaticType() )
{
}

UArianeEditorTool*
UArianeEditorPathEditToolBuilder::CreateTool( UInteractiveToolManager* ToolManager )
{
    Tool = NewObject<UArianeEditorPathEditTool>( ToolManager );

    return Tool;
}
