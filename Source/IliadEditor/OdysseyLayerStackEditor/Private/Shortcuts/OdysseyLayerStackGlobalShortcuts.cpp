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

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().SetCurrentLayerBlendModeToNextBlendMode,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::Action_SetCurrentLayerBlendModeToNextBlendMode)
      , FCanExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::CanAction_AlterLayer)
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().SetCurrentLayerBlendModeToPreviousBlendMode,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::Action_SetCurrentLayerBlendModeToPreviousBlendMode)
      , FCanExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::CanAction_AlterLayer)
    );

    for (int i = 0; EOdysseyBlendingMode blendMode : TEnumRange<EOdysseyBlendingMode>())
    {
        iCommandList->MapAction(
            FOdysseyLayerStackEditorCommands::Get().SetCurrentLayerBlendMode[i++],
            FExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::Action_SetCurrentLayerBlendMode, blendMode),
            FCanExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::CanAction_AlterLayer)
        );
    }
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

void
FOdysseyLayerStackGlobalShortcuts::Action_SetCurrentLayerBlendModeToNextBlendMode()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();

    if ( !layerStack )
        return;

    if ( !layerStack->GetCurrentLayer() )
        return;

    if ( !layerStack->GetCurrentLayer()->IsEditable() )
        return;

    EOdysseyBlendingMode currentBlendMode = layerStack->GetCurrentLayer()->GetBlendMode();
    int8 nextBlendingModeInt = ( static_cast<int8>(currentBlendMode) + 1 ) % static_cast<int8>(EOdysseyBlendingMode::kBlendingMode_Count);
    EOdysseyBlendingMode nextBlendMode = static_cast<EOdysseyBlendingMode>(nextBlendingModeInt);

#if WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("global-layers-shortcuts.transaction.set-current-layer-blend-mode-to-next-blend-mode", "Set Current Layer Blend Mode To Next Blend Mode"));
#endif
    layerStack->GetCurrentLayer()->SetBlendMode( nextBlendMode );
}

void
FOdysseyLayerStackGlobalShortcuts::Action_SetCurrentLayerBlendModeToPreviousBlendMode()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();

    if ( !layerStack )
        return;

    if ( !layerStack->GetCurrentLayer() )
        return;

    if ( !layerStack->GetCurrentLayer()->IsEditable() )
        return;

    EOdysseyBlendingMode currentBlendMode = layerStack->GetCurrentLayer()->GetBlendMode();
    int8 prevBlendingModeInt = ( static_cast<int8>(currentBlendMode) - 1 + static_cast<int8>(EOdysseyBlendingMode::kBlendingMode_Count) ) % static_cast<int8>(EOdysseyBlendingMode::kBlendingMode_Count);
    EOdysseyBlendingMode prevBlendMode = static_cast<EOdysseyBlendingMode>(prevBlendingModeInt);

#if WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("global-layers-shortcuts.transaction.set-current-layer-blend-mode-to-previous-blend-mode", "Set Current Layer Blend Mode To Previous Blend Mode"));
#endif
    layerStack->GetCurrentLayer()->SetBlendMode( prevBlendMode );
}

void
FOdysseyLayerStackGlobalShortcuts::Action_SetCurrentLayerBlendMode(EOdysseyBlendingMode iBlendMode)
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if ( !layerStack )
        return;

    if ( !layerStack->GetCurrentLayer() )
        return;

    if ( !layerStack->GetCurrentLayer()->IsEditable() )
        return;


#if WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("global-layers-shortcuts.transaction.set-current-layer-blend-mode", "Set Current Layer Blend Mode"));
#endif
    layerStack->GetCurrentLayer()->SetBlendMode( iBlendMode );
}

bool
FOdysseyLayerStackGlobalShortcuts::CanAction_AlterLayer()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if ( !layerStack )
        return false;

    if ( !layerStack->GetCurrentLayer() )
        return false;

    return layerStack->GetCurrentLayer()->IsEditable() ? true : false;
}

#undef LOCTEXT_NAMESPACE
