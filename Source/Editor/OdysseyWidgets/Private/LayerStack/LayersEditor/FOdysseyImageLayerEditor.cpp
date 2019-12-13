// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "LayerStack/LayersEditor/FOdysseyImageLayerEditor.h"

#include "LayerStack/FOdysseyLayerStackModel.h"
#include "LayerStack/SOdysseyLayerStackView.h"
#include "LayerStack/SOdysseyLayerStackTreeView.h"

#include "Framework/MultiBox/MultiBoxBuilder.h" //For FMenuBuilder
#include "EditorStyleSet.h"
#include "OdysseyLayerStack.h"


#define LOCTEXT_NAMESPACE "FOdysseyImageLayerEditor"


//Construction/Destruction---------------------------------------

TSharedRef<IOdysseyLayerEditor> FOdysseyImageLayerEditor::CreateLayerEditor(TSharedRef<FOdysseyLayerStackModel> InLayerStack)
{
    return MakeShareable(new FOdysseyImageLayerEditor(InLayerStack));
}


//Implementation of IOdysseyLayerEditor--------------------------

FOdysseyImageLayer* FOdysseyImageLayerEditor::AddLayer(IOdysseyLayer* FocusedLayer)
{
    return nullptr;
}

void FOdysseyImageLayerEditor::BuildAddLayerMenu(FMenuBuilder& MenuBuilder)
{
    MenuBuilder.AddMenuEntry(
        LOCTEXT("AddImageLayer", "Add Image Layer"),
        LOCTEXT("AddImageLayerTooltip", "Adds a new image layer"),
        FSlateIcon(FEditorStyle::GetStyleSetName(), ""),
        FUIAction(
            FExecuteAction::CreateRaw(this, &FOdysseyImageLayerEditor::HandleAddImageLayerMenuEntryExecute),
            FCanExecuteAction::CreateRaw(this, &FOdysseyImageLayerEditor::HandleAddImageLayerMenuEntryCanExecute)
        )
    );
}

const FSlateBrush* FOdysseyImageLayerEditor::GetIconBrush() const
{
    return nullptr;
}

bool FOdysseyImageLayerEditor::IsResizable(FOdysseyImageLayer* InLayer) const
{
    return false;
}

void FOdysseyImageLayerEditor::Resize(float NewSize, FOdysseyImageLayer* InTrack)
{

}

bool FOdysseyImageLayerEditor::GetDefaultExpansionState(FOdysseyImageLayer* InTrack) const
{
    return false;
}

//Callbacks------------------------------------------------------

void FOdysseyImageLayerEditor::HandleAddImageLayerMenuEntryExecute()
{
    FOdysseyLayerStackModel* model = static_cast<FOdysseyLayerStackModel*>(&LayerStackRef.Get());
    model->GetLayerStackData()->AddLayer();
    model->GetLayerStackWidget()->GetTreeView()->Refresh();
}

bool FOdysseyImageLayerEditor::HandleAddImageLayerMenuEntryCanExecute() const
{
    return true;
}

//---------------------------------------------------------------

#undef LOCTEXT_NAMESPACE

