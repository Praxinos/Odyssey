// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Shortcuts/OdysseyLayerStackGlobalShortcuts.h"
#include "Commands/OdysseyLayerStackEditorCommands.h"
#include "OdysseyLayer.h"
#include "OdysseyLayerStack.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "LayerStackEditor"

FOdysseyLayerStackGlobalShortcuts::FOdysseyLayerStackGlobalShortcuts(TAttribute<UOdysseyLayerStack*> iLayerStack)
    : mLayerStack(iLayerStack)
{
}

void
FOdysseyLayerStackGlobalShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().NavigateToNextLayer,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::Action_NavigateToNextLayer)
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().NavigateToPreviousLayer,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::Action_NavigateToPreviousLayer)
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().OpenFolderLayer,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::Action_OpenFolderLayer)
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().CloseFolderLayer,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::Action_CloseFolderLayer)
    );
}

void
FOdysseyLayerStackGlobalShortcuts::Action_NavigateToNextLayer()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return;

    UOdysseyLayer* currentLayer = layerStack->GetCurrentLayer();
    if (!currentLayer)
        return;

    TArray<UOdysseyLayer*> layers = layerStack->GetLayers();
    int index = layers.Find(currentLayer);
    if (index == INDEX_NONE || index == layers.Num() - 1)
        return;

    UOdysseyLayer* layer = nullptr;
    bool isHidden = true;
    while(isHidden)
    {
        index++;
        if (index >= layers.Num())
            return;

        layer = layers[index];

        TArray<UOdysseyLayer*> parents = layer->GetParents();
        isHidden = parents.ContainsByPredicate(
            [](UOdysseyLayer* iLayer)
            {
                return !iLayer->ShouldDisplayChildren();
            }
        );
    }

    layerStack->SetCurrentLayer(layer);
}

void
FOdysseyLayerStackGlobalShortcuts::Action_NavigateToPreviousLayer()
{

    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return;

    UOdysseyLayer* currentLayer = layerStack->GetCurrentLayer();
    if (!currentLayer)
        return;

    TArray<UOdysseyLayer*> layers = layerStack->GetLayers();
    int index = layers.Find(currentLayer);
    if (index == INDEX_NONE || index == 0)
        return;

    UOdysseyLayer* layer = nullptr;
    bool isHidden = true;
    while(isHidden)
    {
        index--;
        if (index < 0)
            return;

        layer = layers[index];

        TArray<UOdysseyLayer*> parents = layer->GetParents();
        isHidden = parents.ContainsByPredicate(
            [](UOdysseyLayer* iLayer)
            {
                return !iLayer->ShouldDisplayChildren();
            }
        );
    }

    layerStack->SetCurrentLayer(layer);
}

void
FOdysseyLayerStackGlobalShortcuts::Action_OpenFolderLayer()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return;

    UOdysseyLayer* currentLayer = layerStack->GetCurrentLayer();
    if (!currentLayer)
        return;

    if (currentLayer->GetChildren().Num() <= 0)
        return;

    currentLayer->SetDisplayChildren(true);
}

void
FOdysseyLayerStackGlobalShortcuts::Action_CloseFolderLayer()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return;

    UOdysseyLayer* currentLayer = layerStack->GetCurrentLayer();
    if (!currentLayer)
        return;

    if (currentLayer->GetChildren().Num() <= 0)
        return;

    currentLayer->SetDisplayChildren(false);
}

#undef LOCTEXT_NAMESPACE
