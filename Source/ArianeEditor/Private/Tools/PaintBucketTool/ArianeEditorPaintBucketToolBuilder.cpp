// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "PaintBucketTool/ArianeEditorPaintBucketToolBuilder.h"
#include "PaintBucketTool/ArianeEditorPaintBucketTool.h"
// Unreal headers
#include "InteractiveToolManager.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

UArianeEditorPaintBucketToolBuilder::UArianeEditorPaintBucketToolBuilder()
    : UArianeEditorToolBuilder( UArianeEditorPaintBucketTool::GetStaticType() )
{
}

UArianeEditorTool*
UArianeEditorPaintBucketToolBuilder::CreateTool( UInteractiveToolManager* ToolManager )
{
    Tool = NewObject<UArianeEditorPaintBucketTool>( ToolManager );

    return Tool;
}

#undef LOCTEXT_NAMESPACE
