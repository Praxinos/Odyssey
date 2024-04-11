// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorToolOptionsTab.h"

#include "Widgets/Tools/SOdysseyPainterEditorToolOptions.h"
#include "Widgets/Layout/SWidgetSwitcher.h"

#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

const FName&
FOdysseyPainterEditorToolOptionsTab::StaticId()
{
    static FName Id = TEXT("OdysseyPainterEditor_ToolOptions"); //Keep ColorSelector instead of ColorWheel because changing that ID would show an empty panel to users who already opened the previous ColorSelector Panel
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyPainterEditorToolOptionsTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorToolOptionsTab::~FOdysseyPainterEditorToolOptionsTab()
{
}

FOdysseyPainterEditorToolOptionsTab::FOdysseyPainterEditorToolOptionsTab(FOdysseyPainterEditor* iEditor)
	: FOdysseyEditorTab(LOCTEXT( "tool-options-tab.name", "Tool Options" ), FSlateIcon( "OdysseyStyle", "PainterEditor.BrushExposedParameters16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

const FName&
FOdysseyPainterEditorToolOptionsTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyPainterEditorToolOptionsTab::CreateWidget()
{
    mWidgetSwitcher = SNew(SWidgetSwitcher)
        .WidgetIndex(this, &FOdysseyPainterEditorToolOptionsTab::WidgetIndex)
        +SWidgetSwitcher::Slot()
        [
            SNullWidget::NullWidget
        ];

    SetWidgetForTool(mEditor->GetRasterDrawingTool(), SNew(SOdysseyPainterEditorToolOptions).Tool(mEditor->GetRasterDrawingTool()));
    SetWidgetForTool(mEditor->GetRasterEraserTool(), SNew(SOdysseyPainterEditorToolOptions).Tool(mEditor->GetRasterEraserTool()));
    SetWidgetForTool(mEditor->GetRasterTransformTool(), SNew(SOdysseyPainterEditorToolOptions).Tool(mEditor->GetRasterTransformTool()));
    SetWidgetForTool(mEditor->GetRasterPrimitiveDrawingTool(), SNew(SOdysseyPainterEditorToolOptions).Tool(mEditor->GetRasterPrimitiveDrawingTool()));
    SetWidgetForTool(mEditor->GetRasterPaintBucketTool(), SNew(SOdysseyPainterEditorToolOptions).Tool(mEditor->GetRasterPaintBucketTool()));
    SetWidgetForTool(mEditor->GetVectorPathDrawingTool(),SNew(SOdysseyPainterEditorToolOptions).Tool(mEditor->GetVectorPathDrawingTool()));
    SetWidgetForTool(mEditor->GetVectorPathEditTool(),SNew(SOdysseyPainterEditorToolOptions).Tool(mEditor->GetVectorPathEditTool()));
    SetWidgetForTool(mEditor->GetVectorPrimitiveDrawingTool(), SNew(SOdysseyPainterEditorToolOptions).Tool(mEditor->GetVectorPrimitiveDrawingTool()));
    SetWidgetForTool(mEditor->GetVectorPathStitchTool(),SNew(SOdysseyPainterEditorToolOptions).Tool(mEditor->GetVectorPathStitchTool()));
    SetWidgetForTool(mEditor->GetVectorPathSmoothTool(), SNew(SOdysseyPainterEditorToolOptions).Tool(mEditor->GetVectorPathSmoothTool()));
    SetWidgetForTool(mEditor->GetVectorSelectionTool(), SNew(SOdysseyPainterEditorToolOptions).Tool(mEditor->GetVectorSelectionTool()));
    SetWidgetForTool(mEditor->GetVectorScenePanTool(), SNew(SOdysseyPainterEditorToolOptions).Tool(mEditor->GetVectorScenePanTool()));
    SetWidgetForTool(mEditor->GetVectorEraserTool(), SNew(SOdysseyPainterEditorToolOptions).Tool(mEditor->GetVectorEraserTool()));
    SetWidgetForTool(mEditor->GetVectorPathPushTool(), SNew(SOdysseyPainterEditorToolOptions).Tool(mEditor->GetVectorPathPushTool()));
    SetWidgetForTool(mEditor->GetVectorPaintBucketTool(), SNew(SOdysseyPainterEditorToolOptions).Tool(mEditor->GetVectorPaintBucketTool()));
    SetWidgetForTool(mEditor->GetVectorGridTool(), SNew(SOdysseyPainterEditorToolOptions).Tool(mEditor->GetVectorGridTool()));
    SetWidgetForTool(mEditor->GetVectorTransformTool(), SNew(SOdysseyPainterEditorToolOptions).Tool(mEditor->GetVectorTransformTool()));

    //mToolsTab->GetToolsTab()->SetWidgetForTool(mEditor->GetVectorDrawingTool(), SNew(SOdysseyPainterVectorDrawingTool, mEditor->GetVectorDrawingTool()));

    return mWidgetSwitcher;
}

void
FOdysseyPainterEditorToolOptionsTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorToolOptionsTab::__VA_ARGS__ ), FCanExecuteAction() );

    #undef MAP_ACTION
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

void
FOdysseyPainterEditorToolOptionsTab::SetWidgetForTool(UOdysseyPainterEditorTool* iTool, TSharedPtr<SWidget> iWidget)
{
    FWidgetSlotForTool* widgetSlotForTool = mWidgetSlotForTool.FindByPredicate(
        [this, iTool](const FWidgetSlotForTool& iWidgetSlotForTool)
        {
            return iTool == iWidgetSlotForTool.mTool;
        }
    );

    if (!widgetSlotForTool)
    {
        mWidgetSlotForTool.Add({ iTool, nullptr });
        widgetSlotForTool = &mWidgetSlotForTool.Last();
        mWidgetSwitcher->AddSlot().Expose(widgetSlotForTool->mSlot);
    }
    
    widgetSlotForTool->mSlot->DetachWidget();
    widgetSlotForTool->mSlot->AttachWidget(iWidget.ToSharedRef());
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

int
FOdysseyPainterEditorToolOptionsTab::WidgetIndex() const
{
    //Add +1 because the widget 0 is a NullWidget in case of tool having no widget defined
    return 1 + mWidgetSlotForTool.IndexOfByPredicate(
        [this](const FWidgetSlotForTool& iWidgetSlotForTool)
        {
            return mEditor->GetSelectedTool() == iWidgetSlotForTool.mTool;
        }
    );
}

#undef LOCTEXT_NAMESPACE
