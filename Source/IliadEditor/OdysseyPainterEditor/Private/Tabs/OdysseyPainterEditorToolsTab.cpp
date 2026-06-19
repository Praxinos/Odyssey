// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorToolsTab.h"

#include "Toolkits/BaseToolkit.h"

#include "Widgets/Input/SButton.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorCommands.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "Widgets/Views/STileView.h"
#include "Widgets/Tools/SOdysseyPainterEditorToolsTileView.h"
#include "Widgets/Tools/SOdysseyPainterEditorToolOptions.h"
#include "OdysseyPainterEditorCommands.h"
#include "Widgets/Tab/SOdysseyPainterEditorTools.h"

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
    return SNew(SOdysseyPainterEditorTools)
        .Editor(mEditor);
}

void
FOdysseyPainterEditorToolsTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorToolsTab::__VA_ARGS__ ), FCanExecuteAction() );

    #undef MAP_ACTION
}

#undef LOCTEXT_NAMESPACE
