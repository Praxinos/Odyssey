// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeEditorColorSelectorTab.h"
#include "Widgets/Input/SButton.h"
#include "ArianeEditor.h"
#include "SArianeEditorColorSelectorPanel.h"
#include "ArianeEditorTool.h"

/* Gary
#include "ArianeEditorCommands.h"
*/

#define LOCTEXT_NAMESPACE "PainterEditor"

const FName&
FArianeEditorColorSelectorTab::StaticId()
{
    static FName Id = TEXT("ArianeEditor_ColorSelectorTab");

    return Id;
}

FArianeEditorColorSelectorTab::~FArianeEditorColorSelectorTab()
{
}

FArianeEditorColorSelectorTab::FArianeEditorColorSelectorTab( FArianeEditor* InEditor )
    : FArianeEditorTab( LOCTEXT( "color-selector-tab.name", "ColorSelectorTab" ), FSlateIcon( "OdysseyStyle", "PainterEditor.Tools" ))
    , Editor(InEditor)
{
}

const FName&
FArianeEditorColorSelectorTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FArianeEditorColorSelectorTab::CreateWidget()
{
    return SNew( SArianeEditorColorSelectorPanel, Editor );
}

void
FArianeEditorColorSelectorTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
/* Gary
    const FArianeEditorCommands& painterEditorCommands = FArianeEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FArianeEditorColorSelectorTab::__VA_ARGS__ ), FCanExecuteAction() );

    #undef MAP_ACTION
*/
}

#undef LOCTEXT_NAMESPACE
