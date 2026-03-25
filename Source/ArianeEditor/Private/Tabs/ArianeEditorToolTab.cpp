// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeEditorToolTab.h"
#include "Widgets/Input/SButton.h"
#include "ArianeEditor.h"
#include "SArianeEditorToolPanel.h"
#include "ArianeEditorTool.h"

/* Gary
#include "ArianeEditorCommands.h"
*/

#define LOCTEXT_NAMESPACE "PainterEditor"

const FName&
FArianeEditorToolTab::StaticId()
{
    static FName Id = TEXT("ArianeEditor_ToolTab");

    return Id;
}

FArianeEditorToolTab::~FArianeEditorToolTab()
{
}

FArianeEditorToolTab::FArianeEditorToolTab( FArianeEditor* InEditor )
    : FArianeEditorTab( LOCTEXT( "tool-tab.name", "ToolTab" ), FSlateIcon( "OdysseyStyle", "PainterEditor.Tools" ))
    , Editor(InEditor)
{
}

const FName&
FArianeEditorToolTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FArianeEditorToolTab::CreateWidget()
{
    return SNew( SArianeEditorToolPanel, Editor );
}

void
FArianeEditorToolTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
/* Gary
    const FArianeEditorCommands& painterEditorCommands = FArianeEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FArianeEditorToolTab::__VA_ARGS__ ), FCanExecuteAction() );

    #undef MAP_ACTION
*/
}

#undef LOCTEXT_NAMESPACE
