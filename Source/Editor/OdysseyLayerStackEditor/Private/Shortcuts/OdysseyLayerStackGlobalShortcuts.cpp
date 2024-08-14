// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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

    UOdysseyLayer* currentLayer = layerStack->CurrentLayer.Get();
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
                return !iLayer->DisplayChildren;
            }
        );
    }
    
    FOdysseyObjectEditorUtils::SetPropertyValue(layerStack, "CurrentLayer", TSoftObjectPtr<UOdysseyLayer>(layer));
}

void
FOdysseyLayerStackGlobalShortcuts::Action_NavigateToPreviousLayer()
{
    
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return;

    UOdysseyLayer* currentLayer = layerStack->CurrentLayer.Get();
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
                return !iLayer->DisplayChildren;
            }
        );
    }
    
    FOdysseyObjectEditorUtils::SetPropertyValue(layerStack, "CurrentLayer", TSoftObjectPtr<UOdysseyLayer>(layer));
}

void
FOdysseyLayerStackGlobalShortcuts::Action_OpenFolderLayer()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return;

    UOdysseyLayer* currentLayer = layerStack->CurrentLayer.Get();
    if (!currentLayer)
        return;

    if (currentLayer->GetChildren().Num() <= 0)
        return;
    
    FOdysseyObjectEditorUtils::SetPropertyValue(currentLayer, "DisplayChildren", true);
}

void
FOdysseyLayerStackGlobalShortcuts::Action_CloseFolderLayer()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return;

    UOdysseyLayer* currentLayer = layerStack->CurrentLayer.Get();
    if (!currentLayer)
        return;

    if (currentLayer->GetChildren().Num() <= 0)
        return;
    
    FOdysseyObjectEditorUtils::SetPropertyValue(currentLayer, "DisplayChildren", false);
}

#undef LOCTEXT_NAMESPACE