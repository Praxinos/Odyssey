// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorToolsTab.h"
#include "Widgets/Input/SButton.h"
#include "OdysseyPainterEditor.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "Widgets/Tools/SOdysseyPainterEditorToolsTileView.h"
#include "Widgets/Views/STileView.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorToolsTab"

const FName&
FOdysseyPainterEditorToolsTab::StaticId()
{
    static FName Id = TEXT("OdysseyPainterEditor_Tools"); //Keep ColorSelector instead of ColorWheel because changing that ID would show an empty panel to users who already opened the previous ColorSelector Panel
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyPainterEditorToolsTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorToolsTab::~FOdysseyPainterEditorToolsTab()
{
}

FOdysseyPainterEditorToolsTab::FOdysseyPainterEditorToolsTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab(LOCTEXT( "OdysseyPainterEditorToolsTab", "Tools" ), FSlateIcon( "OdysseyStyle", "PainterEditor.Tools16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FOdysseyEditorTab interface

const FName&
FOdysseyPainterEditorToolsTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyPainterEditorToolsTab::CreateWidget()
{
    TArray<UOdysseyPainterEditorTool*> tools = {
        mEditor->GetRasterDrawingTool(),
        mEditor->GetRasterTransformTool(),
        mEditor->GetRasterPrimitiveDrawingTool(),
        mEditor->GetVectorPathDrawingTool(),
        mEditor->GetVectorPathEditTool(),
        mEditor->GetVectorPathCutTool(),
        mEditor->GetVectorPrimitiveDrawingTool(),
        mEditor->GetVectorPickTool(),
        mEditor->GetVectorTransformTool(),
        mEditor->GetVectorScenePanTool(),
        mEditor->GetVectorPathPushTool(),
        mEditor->GetVectorPathSmoothTool(),
        mEditor->GetVectorPathStitchTool(),
        mEditor->GetVectorEraserTool(),
        mEditor->GetPaintBucketTool(),
        mEditor->GetColorPickerTool(),
        mEditor->GetVectorGridTool()
    };

    tools = tools.FilterByPredicate([](UOdysseyPainterEditorTool* iTool){return !!iTool;});

    return SNew( SOdysseyPainterEditorToolsTileView )
        .Tools(tools)
        .OnToolSelected(this, &FOdysseyPainterEditorToolsTab::OnToolSelected);
}

void
FOdysseyPainterEditorToolsTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorToolsTab::__VA_ARGS__ ), FCanExecuteAction() );

    #undef MAP_ACTION
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorToolsTab::OnToolSelected(UOdysseyPainterEditorTool* iTool)
{
    mEditor->SetSelectedTool(iTool);
}

#undef LOCTEXT_NAMESPACE
