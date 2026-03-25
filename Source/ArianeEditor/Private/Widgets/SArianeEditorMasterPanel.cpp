// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SArianeEditorMasterPanel.h"
#include "SArianeEditorToolPanel.h"
#include "SArianeEditorLayerStackPanel.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

void
SArianeEditorMasterPanel::Construct( const FArguments& InArgs, FArianeEditor* iEditor )
{
    ChildSlot
    [
        SNew(SVerticalBox)
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SArianeEditorToolPanel, iEditor)
        ]
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SArianeEditorLayerStackPanel, iEditor)
        ]
    ];
}

#undef LOCTEXT_NAMESPACE
