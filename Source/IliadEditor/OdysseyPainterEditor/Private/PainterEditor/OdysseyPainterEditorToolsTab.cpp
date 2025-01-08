// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyPainterEditorToolsTab.h"
#include "Widgets/Input/SButton.h"
#include "OdysseyPainterEditor.h"
#include "Models/OdysseyPainterEditorCommands.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "Widgets/Tools/SOdysseyPainterEditorToolsTileView.h"
#include "Widgets/Views/STileView.h"
#include "Widgets/Tools/SOdysseyPainterEditorToolOptions.h"
#include "Models/OdysseyPainterEditorCommands.h"
#include "Tools/RasterDrawingTool/Widgets/SOdysseyPainterEditorRasterDrawingToolBrushSelector.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

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
    : FOdysseyEditorTab(LOCTEXT( "tools-tab.name", "Tools" ), FSlateIcon( "OdysseyStyle", "PainterEditor.Tools" ))
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
        mEditor->GetRasterEraserTool(),
        mEditor->GetRasterSelectionTool(),
        mEditor->GetRasterTransformTool(),
        mEditor->GetRasterPrimitiveDrawingTool(),
        mEditor->GetRasterPaintBucketTool(),
        mEditor->GetVectorPathDrawingTool(),
        mEditor->GetVectorPathEditTool(),
        mEditor->GetVectorPrimitiveDrawingTool(),
        mEditor->GetVectorSelectionTool(),
        mEditor->GetVectorTransformTool(),
        mEditor->GetVectorScenePanTool(),
        mEditor->GetVectorPathPushTool(),
        mEditor->GetVectorPathSmoothTool(),
        mEditor->GetVectorPathStitchTool(),
        mEditor->GetVectorEraserTool(),
        mEditor->GetVectorPaintBucketTool(),
        mEditor->GetColorPickerTool(),
        mEditor->GetVectorGridTool(),
        mEditor->GetVectorMatchingTool(),
        mEditor->GetVectorChartTool(),
        mEditor->GetVectorTrajectoryTool()
    };

    tools = tools.FilterByPredicate([](UOdysseyPainterEditorTool* iTool){return !!iTool;});

    return SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SOdysseyPainterEditorRasterDrawingToolBrushSelector)
                .Visibility_Lambda(
                    [this]()
                    {
                        return mEditor->GetCurrentTool() == mEditor->GetRasterDrawingTool() ? EVisibility::Visible : EVisibility::Collapsed;
                    }
                )
                .Tool(mEditor->GetRasterDrawingTool())
            ]
            + SVerticalBox::Slot()
            [
                SNew(SOdysseyPainterEditorToolOptions)
                .Tool_Raw(this, &FOdysseyPainterEditorToolsTab::GetCurrentTool)
            ]
        ]

        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew( SOdysseyPainterEditorToolsTileView )
            .Tools(tools)
            .OnToolSelected(this, &FOdysseyPainterEditorToolsTab::OnToolSelected)
        ];
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

UOdysseyPainterEditorTool*
FOdysseyPainterEditorToolsTab::GetCurrentTool() const
{
    return mEditor->GetCurrentTool();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorToolsTab::OnToolSelected(UOdysseyPainterEditorTool* iTool)
{
    mEditor->ActivateMainTool(iTool);
}

#undef LOCTEXT_NAMESPACE
