// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorToolCollectionTab.h"
#include "Widgets/Input/SButton.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorCommands.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "Widgets/Views/STileView.h"
#include "Widgets/Tools/SOdysseyPainterEditorToolsTileView.h"
#include "Widgets/Tools/SOdysseyPainterEditorToolOptions.h"
#include "Tools/RasterDrawingTool/Widgets/SOdysseyPainterEditorRasterDrawingToolBrushSelector.h"
#include "OdysseyPainterEditorCommands.h"
#include "Widgets/Tab/SOdysseyPainterEditorToolCollection.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

const FName&
FOdysseyPainterEditorToolCollectionTab::StaticId()
{
    static FName Id = TEXT("OdysseyPainterEditor_ToolCollection");
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyPainterEditorToolCollectionTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorToolCollectionTab::~FOdysseyPainterEditorToolCollectionTab()
{
}

FOdysseyPainterEditorToolCollectionTab::FOdysseyPainterEditorToolCollectionTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab(LOCTEXT( "tool-collection-tab.name", "ToolCollection" ), FSlateIcon( "OdysseyStyle", "PainterEditor.Tools" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FOdysseyEditorTab interface

const FName&
FOdysseyPainterEditorToolCollectionTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyPainterEditorToolCollectionTab::CreateWidget()
{
    return SNew(SOdysseyPainterEditorToolCollection)
        .Editor(mEditor);
}

void
FOdysseyPainterEditorToolCollectionTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorToolCollectionTab::__VA_ARGS__ ), FCanExecuteAction() );

    #undef MAP_ACTION
}

#undef LOCTEXT_NAMESPACE
