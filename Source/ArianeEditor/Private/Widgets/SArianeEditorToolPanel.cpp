// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/SArianeEditorToolPanel.h"
#include "Widgets/SArianeEditorToolSelector.h"
#include "Widgets/SArianeEditorToolOptions.h"
#include "ArianeEditor.h"
/* Gary
#include "Widgets/Tools/SArianeEditorToolPanelTileView.h"
#include "Widgets/Tools/SArianeEditorToolOptions.h"
*/

SArianeEditorToolPanel::~SArianeEditorToolPanel()
{}

SArianeEditorToolPanel::SArianeEditorToolPanel()
{}

void
SArianeEditorToolPanel::Construct(const FArguments& InArgs, FArianeEditor* InEditor)
{
    Editor = InEditor;

    ChildSlot
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        [
            SNew(SScrollBox)
            + SScrollBox::Slot()
            .FillSize(1.0f)
            [
                SNew( SArianeEditorToolOptions, Editor )
            ]
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SScrollBox)
            + SScrollBox::Slot()
            .AutoSize()
            [
                SNew( SArianeEditorToolSelector, Editor )
            ]
        ]
    ];
}
