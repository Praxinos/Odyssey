// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane
#include "ArianeEditorToolBuilder.h"
#include "ArianeEditor.h"
#include "ArianeEditorTool.h"
// Unreal


UArianeEditorToolBuilder::~UArianeEditorToolBuilder()
{
    Tool = nullptr; // for Garbage collection
}

UArianeEditorToolBuilder::UArianeEditorToolBuilder()
    : Tool ( nullptr )
    , Editor ( nullptr )
{
}

UArianeEditorToolBuilder::UArianeEditorToolBuilder( const FString& InType )
    : UArianeEditorToolBuilder()
{
    Type = InType;
}

void
UArianeEditorToolBuilder::Init( FArianeEditor* InEditor )
{
    Editor = InEditor;
}

UInteractiveTool*
UArianeEditorToolBuilder::BuildTool( const FToolBuilderState& SceneState ) const
{
    return Tool;
}

UArianeEditorTool*
UArianeEditorToolBuilder::CreateTool( UInteractiveToolManager* ToolManager )
{
    return nullptr;
}

FString
UArianeEditorToolBuilder::GetType()
{
    return Type;
}
