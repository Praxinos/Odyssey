// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorToolOptionsTab.h"

#include "Tools/RasterDrawingTool/Widgets/SOdysseyPainterEditorRasterDrawingToolOptions.h"
#include "Widgets/Layout/SWidgetSwitcher.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorToolOptionsTab"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorToolOptionsTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorToolOptionsTab::~FOdysseyPainterEditorToolOptionsTab()
{
}

FOdysseyPainterEditorToolOptionsTab::FOdysseyPainterEditorToolOptionsTab(FOdysseyPainterEditor* iEditor)
	: FOdysseyEditorTab(TEXT("OdysseyPainterEditor_ToolOptions"),
                            LOCTEXT( "OdysseyPainterEditorToolOptionsTab", "Tool Options" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.BrushExposedParameters16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorToolOptionsTab::CreateWidget()
{
    mWidgetSwitcher = SNew(SWidgetSwitcher)
        .WidgetIndex(this, &FOdysseyPainterEditorToolOptionsTab::WidgetIndex)
        +SWidgetSwitcher::Slot()
        [
            SNullWidget::NullWidget
        ];

    SetWidgetForTool(mEditor->GetRasterDrawingTool(), SNew(SOdysseyPainterEditorRasterDrawingToolOptions).Tool(mEditor->GetRasterDrawingTool()));
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
        [this](const FWidgetSlotForTool& iWidgetSlotForTool)
        {
            return mEditor->GetSelectedTool() == iWidgetSlotForTool.mTool;
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
