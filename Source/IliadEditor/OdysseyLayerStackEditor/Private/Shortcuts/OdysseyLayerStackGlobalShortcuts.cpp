// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Shortcuts/OdysseyLayerStackGlobalShortcuts.h"

#include "ScopedTransaction.h"

#include "Commands/OdysseyLayerStackEditorCommands.h"
#include "OdysseyLayer.h"
#include "OdysseyLayerStack.h"
#include "OdysseyLayerStackSelection.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "LayerStackEditor"

FOdysseyLayerStackGlobalShortcuts::FOdysseyLayerStackGlobalShortcuts(TAttribute<UOdysseyLayerStack*> iLayerStack, TAttribute<UOdysseyLayer*> iLayer)
    : mLayerStack(iLayerStack)
    , mFocusedLayer( iLayer )
{
}

UOdysseyLayer*
FOdysseyLayerStackGlobalShortcuts::GetFocusedLayer() const
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return nullptr;

    return mFocusedLayer.IsBound() ? mFocusedLayer.Get() : layerStack->GetCurrentLayer();
}

void
FOdysseyLayerStackGlobalShortcuts::GetSelectedLayers(TArray<UOdysseyLayer*>& OutSelectedLayers) const
{
    OutSelectedLayers.Empty();
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return;

    OutSelectedLayers = OdysseyLayerStackSelection::GetSelectedLayers(layerStack, true);

    if (mFocusedLayer.IsBound())
    {
        UOdysseyLayer* FocusedLayer = mFocusedLayer.Get();

        /**
         * If the FocuseLayer is not selected, we consider shortcuts should only
         * modify the Focused layer and not the selected layers
         */
        if (!OutSelectedLayers.Contains(FocusedLayer))
        {
            OutSelectedLayers.Empty();
            OutSelectedLayers.Add( FocusedLayer );
        }
    }
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

    //---

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().ActivateLayer,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::Action_ActivateLayer)
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().InactivateLayer,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::Action_InactivateLayer)
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().ToggleLayerActivated,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::Action_ToggleLayerActivated)
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().LockLayer,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::Action_LockLayer)
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().UnlockLayer,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::Action_UnlockLayer)
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().ToggleLayerLocked,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::Action_ToggleLayerLocked)
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().ActivateLayerInheritsAlpha,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::Action_ActivateLayerInheritsAlpha)
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().InactivateLayerInheritsAlpha,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::Action_InactivateLayerInheritsAlpha)
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().ToggleLayerInheritsAlpha,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::Action_ToggleLayerInheritsAlpha)
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().ActivateLayerLighttable,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::Action_ActivateLayerLighttable)
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().InactivateLayerLighttable,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::Action_InactivateLayerLighttable)
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().ToggleLayerLighttable,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::Action_ToggleLayerLighttable)
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().CollapseLayer,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::Action_CollapseLayer)
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().UncollapseLayer,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::Action_UncollapseLayer)
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().ToggleLayerCollapsed,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::Action_ToggleLayerCollapsed)
    );

    //---

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().LockAllLayers,
        FExecuteAction::CreateRaw( this, &FOdysseyLayerStackGlobalShortcuts::Action_LockAllLayers )
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().UnlockAllLayers,
        FExecuteAction::CreateRaw( this, &FOdysseyLayerStackGlobalShortcuts::Action_UnlockAllLayers )
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().ActivateAllLayers,
        FExecuteAction::CreateRaw( this, &FOdysseyLayerStackGlobalShortcuts::Action_ActivateAllLayers )
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().InactivateAllLayers,
        FExecuteAction::CreateRaw( this, &FOdysseyLayerStackGlobalShortcuts::Action_InactivateAllLayers )
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().DisplayOnlyCurrentLayer,
        FExecuteAction::CreateRaw( this, &FOdysseyLayerStackGlobalShortcuts::Action_DisplayOnlyCurrentLayer ),
        FCanExecuteAction::CreateRaw( this, &FOdysseyLayerStackGlobalShortcuts::CanAction_DisplayOnlyCurrentLayer ),
        FIsActionChecked::CreateRaw( this, &FOdysseyLayerStackGlobalShortcuts::IsActionChecked_DisplayOnlyCurrentLayer )
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().CollapseAllLayers,
        FExecuteAction::CreateRaw( this, &FOdysseyLayerStackGlobalShortcuts::Action_CollapseAllLayers )
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().UncollapseAllLayers,
        FExecuteAction::CreateRaw( this, &FOdysseyLayerStackGlobalShortcuts::Action_UncollapseAllLayers )
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().OpenAllFolderLayers,
        FExecuteAction::CreateRaw( this, &FOdysseyLayerStackGlobalShortcuts::Action_OpenAllFolderLayers )
    );

    iCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().CloseAllFolderLayers,
        FExecuteAction::CreateRaw( this, &FOdysseyLayerStackGlobalShortcuts::Action_CloseAllFolderLayers )
    );

    //---

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

    for (TPair<TSharedPtr<FUICommandInfo>, EOdysseyBlendMode> Pair : FOdysseyLayerStackEditorCommands::Get().SetCurrentLayerBlendMode)
    {
        iCommandList->MapAction(
            Pair.Key,
            FExecuteAction::CreateRaw(this, &FOdysseyLayerStackGlobalShortcuts::Action_SetCurrentLayerBlendMode, Pair.Value),
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
    UOdysseyLayer* FocusedLayer = GetFocusedLayer();
    if (!FocusedLayer)
        return;

    TArray<UOdysseyLayer*> selectedLayers;
    GetSelectedLayers(selectedLayers);

    if (!FocusedLayer || selectedLayers.IsEmpty())
        return;

    if (FocusedLayer->GetChildren().Num() <= 0)
        return;

    for( UOdysseyLayer* layer : selectedLayers )
    {
        if (layer->GetChildren().Num() <= 0)
            continue;

        layer->SetDisplayChildren( true );
    }
}

void
FOdysseyLayerStackGlobalShortcuts::Action_CloseFolderLayer()
{
    UOdysseyLayer* FocusedLayer = GetFocusedLayer();
    if (!FocusedLayer)
        return;

    TArray<UOdysseyLayer*> selectedLayers;
    GetSelectedLayers(selectedLayers);

    if (!FocusedLayer || selectedLayers.IsEmpty())
        return;

    if (FocusedLayer->GetChildren().Num() <= 0)
        return;

    for( UOdysseyLayer* layer : selectedLayers )
    {
        if (layer->GetChildren().Num() <= 0)
            continue;

        layer->SetDisplayChildren( false );
    }
}

void
FOdysseyLayerStackGlobalShortcuts::SetLayerIsActivated(bool InValue)
{
    TArray<UOdysseyLayer*> selectedLayers;
    GetSelectedLayers(selectedLayers);

    if (selectedLayers.IsEmpty())
        return;

    for( UOdysseyLayer* layer : selectedLayers )
    {
        layer->SetIsActivated(InValue);
    }

    OdysseyLayerStackSelection::RegisterUndo(selectedLayers, selectedLayers);
}

void
FOdysseyLayerStackGlobalShortcuts::SetLayerIsLocked(bool InValue)
{
    TArray<UOdysseyLayer*> selectedLayers;
    GetSelectedLayers(selectedLayers);

    if (selectedLayers.IsEmpty())
        return;

    for( UOdysseyLayer* layer : selectedLayers )
    {
        layer->SetIsLocked(InValue);
    }

    OdysseyLayerStackSelection::RegisterUndo(selectedLayers, selectedLayers);
}

void
FOdysseyLayerStackGlobalShortcuts::SetLayerAlphaInheritance(bool InValue)
{
    TArray<UOdysseyLayer*> selectedLayers;
    GetSelectedLayers(selectedLayers);

    if (selectedLayers.IsEmpty())
        return;

    for( UOdysseyLayer* layer : selectedLayers )
    {
        layer->SetInheritsAlpha(InValue);
    }
    OdysseyLayerStackSelection::RegisterUndo(selectedLayers, selectedLayers);
}

void
FOdysseyLayerStackGlobalShortcuts::SetLayerLighttable(bool InValue)
{
    TArray<UOdysseyLayer*> selectedLayers;
    GetSelectedLayers(selectedLayers);

    if (selectedLayers.IsEmpty())
        return;

    for( UOdysseyLayer* layer : selectedLayers )
    {
        FOdysseyLighttable LT = layer->GetLighttable();
        LT.bIsActivated = InValue;
        layer->SetLighttable(LT);
    }
}

void
FOdysseyLayerStackGlobalShortcuts::SetLayerCollapsed(bool InValue)
{
    TArray<UOdysseyLayer*> selectedLayers;
    GetSelectedLayers(selectedLayers);

    if (selectedLayers.IsEmpty())
        return;

    for( UOdysseyLayer* layer : selectedLayers )
    {
        layer->SetDisplayOptions(InValue);
    }
}

void
FOdysseyLayerStackGlobalShortcuts::Action_ActivateLayer()
{
    FScopedTransaction ScopedTransaction( LOCTEXT( "global-layers-shortcuts.transaction.activate-layer", "Activate Selected Layers" ) );
    SetLayerIsActivated(true);
}

void
FOdysseyLayerStackGlobalShortcuts::Action_InactivateLayer()
{
    FScopedTransaction ScopedTransaction( LOCTEXT( "global-layers-shortcuts.transaction.inactivate-layer", "Inactivate Selected Layers" ) );
    SetLayerIsActivated(false);
}

void
FOdysseyLayerStackGlobalShortcuts::Action_ToggleLayerActivated()
{
    UOdysseyLayer* FocusedLayer = GetFocusedLayer();
    if (!FocusedLayer)
        return;

    bool Value = !FocusedLayer->IsActivated();
    FScopedTransaction ScopedTransaction( LOCTEXT( "global-layers-shortcuts.transaction.toggle-layer-activated", "Activate/Inactivate Selected Layers" ) );
    SetLayerIsActivated(Value);
}

void
FOdysseyLayerStackGlobalShortcuts::Action_LockLayer()
{
    FScopedTransaction ScopedTransaction( LOCTEXT( "global-layers-shortcuts.transaction.lock-layers", "Lock Selected Layers" ) );
    SetLayerIsLocked(true);
}

void
FOdysseyLayerStackGlobalShortcuts::Action_UnlockLayer()
{
    FScopedTransaction ScopedTransaction( LOCTEXT( "global-layers-shortcuts.transaction.unlock-layer", "Unlock Selected Layers" ) );
    SetLayerIsLocked(false);
}

void
FOdysseyLayerStackGlobalShortcuts::Action_ToggleLayerLocked()
{
    UOdysseyLayer* FocusedLayer = GetFocusedLayer();
    if (!FocusedLayer)
        return;

    bool Value = !FocusedLayer->IsLocked();
    FScopedTransaction ScopedTransaction( LOCTEXT( "global-layers-shortcuts.transaction.toggle-layer-locked", "Lock/Unlock Selected Layers" ) );
    SetLayerIsLocked(Value);
}

void
FOdysseyLayerStackGlobalShortcuts::Action_ActivateLayerInheritsAlpha()
{
    FScopedTransaction ScopedTransaction( LOCTEXT( "global-layers-shortcuts.transaction.activate-layer-inherits-alpha", "Activate Selected Layers' Alpha Inheritance" ) );
    SetLayerAlphaInheritance(true);
}

void
FOdysseyLayerStackGlobalShortcuts::Action_InactivateLayerInheritsAlpha()
{
    FScopedTransaction ScopedTransaction( LOCTEXT( "global-layers-shortcuts.transaction.inactivate-layer-inherits-alpha", "Inactivate Selected Layers' Alpha Inheritance" ) );
    SetLayerAlphaInheritance(false);
}

void
FOdysseyLayerStackGlobalShortcuts::Action_ToggleLayerInheritsAlpha()
{
    UOdysseyLayer* FocusedLayer = GetFocusedLayer();
    if (!FocusedLayer)
        return;

    bool Value = !FocusedLayer->GetInheritsAlpha();
    FScopedTransaction ScopedTransaction( LOCTEXT( "global-layers-shortcuts.transaction.toggle-layer-inherits-alpha", "Toggle Selected Layers' Alpha Inheritance" ) );
    SetLayerAlphaInheritance(Value);
}

void
FOdysseyLayerStackGlobalShortcuts::Action_ActivateLayerLighttable()
{
    SetLayerLighttable(true);
}

void
FOdysseyLayerStackGlobalShortcuts::Action_InactivateLayerLighttable()
{
    SetLayerLighttable(false);
}

void
FOdysseyLayerStackGlobalShortcuts::Action_ToggleLayerLighttable()
{
    UOdysseyLayer* FocusedLayer = GetFocusedLayer();
    if (!FocusedLayer)
        return;

    bool Value = !FocusedLayer->GetLighttable().bIsActivated;
    SetLayerLighttable(Value);
}

void
FOdysseyLayerStackGlobalShortcuts::Action_CollapseLayer()
{
    SetLayerCollapsed(false);
}

void
FOdysseyLayerStackGlobalShortcuts::Action_UncollapseLayer()
{
    SetLayerCollapsed(true);
}

void
FOdysseyLayerStackGlobalShortcuts::Action_ToggleLayerCollapsed()
{
    UOdysseyLayer* FocusedLayer = GetFocusedLayer();
    if (!FocusedLayer)
        return;

    bool Value = !FocusedLayer->ShouldDisplayOptions();
    SetLayerCollapsed(Value);
}

void
FOdysseyLayerStackGlobalShortcuts::Action_LockAllLayers()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if( !layerStack )
        return;

    FScopedTransaction ScopedTransaction( LOCTEXT( "global-layers-shortcuts.transaction.lock-all-layers", "Lock All Layers" ) );

    for( UOdysseyLayer* layer : layerStack->GetLayers() )
    {
        layer->SetIsLocked( true );
    }
}
void
FOdysseyLayerStackGlobalShortcuts::Action_UnlockAllLayers()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if( !layerStack )
        return;

    FScopedTransaction ScopedTransaction( LOCTEXT( "global-layers-shortcuts.transaction.unlock-all-layers", "Unock All Layers" ) );

    for( UOdysseyLayer* layer : layerStack->GetLayers() )
    {
        layer->SetIsLocked( false );
    }
}
void
FOdysseyLayerStackGlobalShortcuts::Action_ActivateAllLayers()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if( !layerStack )
        return;

    FScopedTransaction ScopedTransaction( LOCTEXT( "global-layers-shortcuts.transaction.activate-all-layers", "Activate All Layers" ) );

    for( UOdysseyLayer* layer : layerStack->GetLayers() )
    {
        layer->SetIsActivated( true );
    }
}
void
FOdysseyLayerStackGlobalShortcuts::Action_InactivateAllLayers()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if( !layerStack )
        return;

    FScopedTransaction ScopedTransaction( LOCTEXT( "global-layers-shortcuts.transaction.inactivate-all-layers", "Inactivate All Layers" ) );

    for( UOdysseyLayer* layer : layerStack->GetLayers() )
    {
        layer->SetIsActivated( false );
    }
}
void
FOdysseyLayerStackGlobalShortcuts::Action_DisplayOnlyCurrentLayer()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if( !layerStack )
        return;

    FScopedTransaction ScopedTransaction( LOCTEXT( "global-layers-shortcuts.transaction.diaply-only-current-layer", "Display Only Current layer" ) );

    layerStack->ToggleDisplayOnlyCurrentLayer();
}
bool
FOdysseyLayerStackGlobalShortcuts::CanAction_DisplayOnlyCurrentLayer()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if( !layerStack )
        return false;

    return true;
}
bool
FOdysseyLayerStackGlobalShortcuts::IsActionChecked_DisplayOnlyCurrentLayer()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if( !layerStack )
        return false;

    return layerStack->GetDisplayOnlyCurrentLayer();
}
void
FOdysseyLayerStackGlobalShortcuts::Action_CollapseAllLayers()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if( !layerStack )
        return;

    FScopedTransaction ScopedTransaction( LOCTEXT( "global-layers-shortcuts.transaction.collapse-all-layers", "Collapse All Layers" ) );

    for( UOdysseyLayer* layer : layerStack->GetLayers() )
    {
        layer->SetDisplayOptions( false );
    }
}
void
FOdysseyLayerStackGlobalShortcuts::Action_UncollapseAllLayers()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if( !layerStack )
        return;

    FScopedTransaction ScopedTransaction( LOCTEXT( "global-layers-shortcuts.transaction.uncollapse-all-layers", "Uncollapse All Layers" ) );

    for( UOdysseyLayer* layer : layerStack->GetLayers() )
    {
        layer->SetDisplayOptions( true );
    }
}
void
FOdysseyLayerStackGlobalShortcuts::Action_OpenAllFolderLayers()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if( !layerStack )
        return;

    FScopedTransaction ScopedTransaction( LOCTEXT( "global-layers-shortcuts.transaction.open-all-folder-layers", "Open All Folder Layers" ) );

    for( UOdysseyLayer* layer : layerStack->GetLayers() )
    {
        layer->SetDisplayChildren( true );
    }
}
void
FOdysseyLayerStackGlobalShortcuts::Action_CloseAllFolderLayers()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if( !layerStack )
        return;

    FScopedTransaction ScopedTransaction( LOCTEXT( "global-layers-shortcuts.transaction.close-all-folder-layers", "Close All Folder Layers" ) );

    for( UOdysseyLayer* layer : layerStack->GetLayers() )
    {
        layer->SetDisplayChildren( false );
    }
}

void
FOdysseyLayerStackGlobalShortcuts::Action_SetCurrentLayerBlendModeToNextBlendMode()
{
    UOdysseyLayer* FocusedLayer = GetFocusedLayer();
    if (!FocusedLayer)
        return;

    TArray<UOdysseyLayer*> selectedLayers;
    GetSelectedLayers(selectedLayers);

    if ( !FocusedLayer->IsEditable() )
        return;

    FScopedTransaction ScopedTransaction(LOCTEXT("global-layers-shortcuts.transaction.set-current-layer-blend-mode-to-next-blend-mode", "Set Current Layer Blend Mode To Next Blend Mode"));

    for( UOdysseyLayer* layer : selectedLayers )
    {
        if ( !layer->IsEditable() )
            continue;

        EOdysseyBlendMode currentBlendMode = layer->GetBlendMode();
        int8 nextBlendModeInt = ( static_cast<int8>( currentBlendMode ) + 1 ) % static_cast<int8>( EOdysseyBlendMode::BlendMode_Count );
        EOdysseyBlendMode nextBlendMode = static_cast<EOdysseyBlendMode>( nextBlendModeInt );

        layer->SetBlendMode( nextBlendMode );
    }

    OdysseyLayerStackSelection::RegisterUndo(selectedLayers, selectedLayers);
}

void
FOdysseyLayerStackGlobalShortcuts::Action_SetCurrentLayerBlendModeToPreviousBlendMode()
{
    UOdysseyLayer* FocusedLayer = GetFocusedLayer();
    if (!FocusedLayer)
        return;

    TArray<UOdysseyLayer*> selectedLayers;
    GetSelectedLayers(selectedLayers);

    if ( !FocusedLayer->IsEditable() )
        return;

    FScopedTransaction ScopedTransaction(LOCTEXT("global-layers-shortcuts.transaction.set-current-layer-blend-mode-to-previous-blend-mode", "Set Current Layer Blend Mode To Previous Blend Mode"));

    for( UOdysseyLayer* layer : selectedLayers )
    {
        if ( !layer->IsEditable() )
            continue;

        EOdysseyBlendMode currentBlendMode = layer->GetBlendMode();
        int8 prevBlendModeInt = ( static_cast<int8>( currentBlendMode ) - 1 + static_cast<int8>( EOdysseyBlendMode::BlendMode_Count ) ) % static_cast<int8>( EOdysseyBlendMode::BlendMode_Count );
        EOdysseyBlendMode prevBlendMode = static_cast<EOdysseyBlendMode>( prevBlendModeInt );

        layer->SetBlendMode( prevBlendMode );
    }

    OdysseyLayerStackSelection::RegisterUndo(selectedLayers, selectedLayers);
}

void
FOdysseyLayerStackGlobalShortcuts::Action_SetCurrentLayerBlendMode(EOdysseyBlendMode iBlendMode)
{
    UOdysseyLayer* FocusedLayer = GetFocusedLayer();
    if (!FocusedLayer)
        return;

    TArray<UOdysseyLayer*> selectedLayers;
    GetSelectedLayers(selectedLayers);

    if ( !FocusedLayer->IsEditable() )
        return;

    FScopedTransaction ScopedTransaction(LOCTEXT("global-layers-shortcuts.transaction.set-current-layer-blend-mode", "Set Current Layer Blend Mode"));

    for( UOdysseyLayer* layer : selectedLayers )
    {
        if ( !layer->IsEditable() )
            continue;

        layer->SetBlendMode( iBlendMode );
    }

    OdysseyLayerStackSelection::RegisterUndo(selectedLayers, selectedLayers);
}

bool
FOdysseyLayerStackGlobalShortcuts::CanAction_AlterLayer()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if ( !layerStack )
        return false;

    UOdysseyLayer* FocusedLayer = mFocusedLayer.IsBound() ? mFocusedLayer.Get() : layerStack->GetCurrentLayer();
    if ( !FocusedLayer )
        return false;

    return FocusedLayer->IsEditable();
}

#undef LOCTEXT_NAMESPACE
