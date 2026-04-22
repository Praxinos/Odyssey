// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SArianeEditorMasterPanel.h"
#include "SArianeEditorToolPanel.h"
#include "SArianeEditorLayerStackPanel.h"
#include "SArianeEditorColorSelectorPanel.h"
#include "ArianeEditor.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

void
SArianeEditorMasterPanel::Construct( const FArguments& InArgs, FArianeEditor* InEditor )
{
    ChildSlot
    [
        SNew(SVerticalBox)
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SArianeEditorToolPanel, InEditor)
        ]
/*
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SArianeEditorLayerStackPanel, InEditor)
            .Visibility_Lambda( [InEditor]() -> EVisibility
                {
                    return InEditor->GetCurrentPainting3DComponent() ? EVisibility::Visible : EVisibility::Hidden;
                } )
        ]
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SArianeEditorColorSelectorPanel, InEditor)
        ]
*/
    ];
}

#undef LOCTEXT_NAMESPACE
