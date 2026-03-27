// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Headers
#include "SArianeEditorLayerStackPanel.h"
#include "SArianeEditorLayerStack.h"
#include "ArianeEditor.h"
#include "ArianePainting3DComponent.h"
#include "ArianeLayerStack.h"
/* Gary
#include "Widgets/Tools/SArianeEditorToolPanelTileView.h"
#include "Widgets/Tools/SArianeEditorToolOptions.h"
*/

#define LOCTEXT_NAMESPACE "ArianeEditor"

SArianeEditorLayerStackPanel::~SArianeEditorLayerStackPanel()
{}

SArianeEditorLayerStackPanel::SArianeEditorLayerStackPanel()
{}

void
SArianeEditorLayerStackPanel::Construct(const FArguments& InArgs, FArianeEditor* InEditor)
{
    Editor = InEditor;

    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        [
            SNew( SButton )
                .Text( LOCTEXT("layer-stack-panel-new-layer","New Layer") )
                .OnClicked( FOnClicked::CreateSP( this, &SArianeEditorLayerStackPanel::NewLayer ) )
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SScrollBox)
            + SScrollBox::Slot()
            .AutoSize()
            [
                SNew( SArianeEditorLayerStack, Editor )
            ]
        ]
    ];
}

FReply
SArianeEditorLayerStackPanel::NewLayer()
{
    UArianePainting3DComponent* CurrentPainting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( CurrentPainting3DComponent )
    {
        CurrentPainting3DComponent->GetLayerStack()->CreateDrawingLayer( nullptr );
    }

    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
