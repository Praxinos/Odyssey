// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorBrushSelectorTab.h"
#include "OdysseyBrushAssetBase.h"
#include "OdysseyPainterEditor.h"
#include "SOdysseyPaintModifiers.h"
#include "Models/OdysseyPainterEditorCommands.h"
#include "Tools/RasterDrawingTool/Widgets/SOdysseyPainterEditorRasterDrawingToolBrushSelector.h"
#include "ObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorBrushSelectorTab"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorBrushSelectorTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorBrushSelectorTab::~FOdysseyPainterEditorBrushSelectorTab()
{
}

FOdysseyPainterEditorBrushSelectorTab::FOdysseyPainterEditorBrushSelectorTab(FOdysseyPainterEditor* iEditor)
	: FOdysseyEditorTab(TEXT("OdysseyPainterEditor_BrushSelector"),
                            LOCTEXT( "OdysseyPainterEditorBrushSelectorTab", "Brush Selector" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.BrushSelector16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorBrushSelectorTab::CreateWidget()
{
    mWidgetSwitcher = SNew(SWidgetSwitcher)
        .WidgetIndex(this, &FOdysseyPainterEditorBrushSelectorTab::WidgetIndex)
        + SWidgetSwitcher::Slot()
        [
            SNullWidget::NullWidget
        ];

    SetWidgetForTool(mEditor->GetRasterDrawingTool(), SNew(SOdysseyPainterEditorRasterDrawingToolBrushSelector).Tool(mEditor->GetRasterDrawingTool()));

    return mWidgetSwitcher;
}

void
FOdysseyPainterEditorBrushSelectorTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorBrushSelectorTab::__VA_ARGS__ ), FCanExecuteAction() );

    #undef MAP_ACTION
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

void
FOdysseyPainterEditorBrushSelectorTab::SetWidgetForTool(UOdysseyPainterEditorTool* iTool, TSharedPtr<SWidget> iWidget)
{
    FWidgetSlotForTool* widgetSlotForTool = mWidgetSlotForTool.FindByPredicate(
        [this](const FWidgetSlotForTool& iWidgetSlotForTool)
        {
            return mEditor->GetSelectedTool() == iWidgetSlotForTool.mTool;
        }
    );

    if ( !widgetSlotForTool )
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
FOdysseyPainterEditorBrushSelectorTab::WidgetIndex() const
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
