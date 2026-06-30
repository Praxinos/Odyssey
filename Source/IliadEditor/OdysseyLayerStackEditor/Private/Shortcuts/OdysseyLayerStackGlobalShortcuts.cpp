// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Shortcuts/OdysseyLayerStackGlobalShortcuts.h"

#include "ScopedTransaction.h"

#include "Commands/OdysseyLayerStackEditorCommands.h"
#include "OdysseyLayer.h"
#include "OdysseyLayerStack.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "LayerStackEditor"

FOdysseyLayerStackGlobalShortcuts::FOdysseyLayerStackGlobalShortcuts(TAttribute<UOdysseyLayerStack*> iLayerStack, TAttribute<UOdysseyLayer*> iLayer)
    : mLayerStack(iLayerStack)
    , mFocusedLayer( iLayer )
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

    TSet<UOdysseyLayer*> selected_layers;
    for( UOdysseyLayer* layer : layerStack->GetLayers() )
    {
        if( layerStack->IsLayerSelected( layer ) )
            selected_layers.Add( layer );
    }
    // Generally, the current layer is selected except when the layer stack is created (before any click interactions in layer stack header)
    // But too much interrogations to fix it (as many callbacks can be called.
    // (add a flag in SetCurrentLayer() to deselect all and select only the new current layer or in FOdysseyLayerSelection or ...)
    // So, at least for now, just always add it.
    //check( selected_layers.Contains( layerStack->GetCurrentLayer() ) );
    selected_layers.Add( layerStack->GetCurrentLayer() );

    for( UOdysseyLayer* layer : selected_layers )
    {
        layer->SetDisplayChildren( true );
    }
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

    TSet<UOdysseyLayer*> selected_layers;
    for( UOdysseyLayer* layer : layerStack->GetLayers() )
    {
        if( layerStack->IsLayerSelected( layer ) )
            selected_layers.Add( layer );
    }
    // Generally, the current layer is selected except when the layer stack is created (before any click interactions in layer stack header)
    // But too much interrogations to fix it (as many callbacks can be called.
    // (add a flag in SetCurrentLayer() to deselect all and select only the new current layer or in FOdysseyLayerSelection or ...)
    // So, at least for now, just always add it.
    //check( selected_layers.Contains( layerStack->GetCurrentLayer() ) );
    selected_layers.Add( layerStack->GetCurrentLayer() );

    for( UOdysseyLayer* layer : selected_layers )
    {
        layer->SetDisplayChildren( false );
    }
}

void
FOdysseyLayerStackGlobalShortcuts::Action_LockAllLayers()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if( !layerStack )
        return;

    FScopedTransaction ScopedTransaction( LOCTEXT( "global-layers-shortcuts.transaction.lock-all-layers", "Lock All Layers" ) );

    TSet<UOdysseyLayer*> selected_layers;
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

    TSet<UOdysseyLayer*> selected_layers;
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

    TSet<UOdysseyLayer*> selected_layers;
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

    TSet<UOdysseyLayer*> selected_layers;
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

    TSet<UOdysseyLayer*> selected_layers;
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

    TSet<UOdysseyLayer*> selected_layers;
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

    TSet<UOdysseyLayer*> selected_layers;
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

    TSet<UOdysseyLayer*> selected_layers;
    for( UOdysseyLayer* layer : layerStack->GetLayers() )
    {
        layer->SetDisplayChildren( false );
    }
}

void
FOdysseyLayerStackGlobalShortcuts::Action_SetCurrentLayerBlendModeToNextBlendMode()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if ( !layerStack )
        return;

    // For the moment, mFocusedLayer should not be used in Prev/Next blend mode as it is not used in the blend mode popup
    // But it's already done if it is the case one day
    UOdysseyLayer* currentLayer = mFocusedLayer.IsBound() ? mFocusedLayer.Get() : layerStack->GetCurrentLayer();
    if ( !layerStack->GetCurrentLayer() )
        return;

    if ( !layerStack->GetCurrentLayer()->IsEditable() )
        return;

    TSet<UOdysseyLayer*> selected_layers;
    for( UOdysseyLayer* layer : layerStack->GetLayers() )
    {
        if( layerStack->IsLayerSelected( layer ) )
            selected_layers.Add( layer );
    }
    // Generally, the current layer is selected except when the layer stack is created (before any click interactions in layer stack header)
    // But too much interrogations to fix it (as many callbacks can be called.
    // (add a flag in SetCurrentLayer() to deselect all and select only the new current layer or in FOdysseyLayerSelection or ...)
    // So, at least for now, just always add it.
    //check( selected_layers.Contains( layerStack->GetCurrentLayer() ) );
    selected_layers.Add( layerStack->GetCurrentLayer() );

    // If the currentLayer is not in the selected layer list, just use it
    // - if currentLayer == GetCurrentLayer(): always inside the list so never go there
    // - if currentLayer == mFocusedLayer: if outside the selection, only modify it
    if( !selected_layers.Contains( currentLayer ) )
    {
        selected_layers.Empty();
        selected_layers.Add( currentLayer );
    }

    FScopedTransaction ScopedTransaction(LOCTEXT("global-layers-shortcuts.transaction.set-current-layer-blend-mode-to-next-blend-mode", "Set Current Layer Blend Mode To Next Blend Mode"));

    for( UOdysseyLayer* layer : selected_layers )
    {
        EOdysseyBlendingMode currentBlendMode = layer->GetBlendMode();
        int8 nextBlendingModeInt = ( static_cast<int8>( currentBlendMode ) + 1 ) % static_cast<int8>( EOdysseyBlendingMode::kBlendingMode_Count );
        EOdysseyBlendingMode nextBlendMode = static_cast<EOdysseyBlendingMode>( nextBlendingModeInt );

        layer->SetBlendMode( nextBlendMode );
    }
}

void
FOdysseyLayerStackGlobalShortcuts::Action_SetCurrentLayerBlendModeToPreviousBlendMode()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if ( !layerStack )
        return;

    // For the moment, mFocusedLayer should not be used in Prev/Next blend mode as it is not used in the blend mode popup
    // But it's already done if it is the case one day
    UOdysseyLayer* currentLayer = mFocusedLayer.IsBound() ? mFocusedLayer.Get() : layerStack->GetCurrentLayer();
    if ( !currentLayer )
        return;

    if ( !currentLayer->IsEditable() )
        return;

    TSet<UOdysseyLayer*> selected_layers;
    for( UOdysseyLayer* layer : layerStack->GetLayers() )
    {
        if( layerStack->IsLayerSelected( layer ) )
            selected_layers.Add( layer );
    }
    // Generally, the current layer is selected except when the layer stack is created (before any click interactions in layer stack header)
    // But too much interrogations to fix it (as many callbacks can be called.
    // (add a flag in SetCurrentLayer() to deselect all and select only the new current layer or in FOdysseyLayerSelection or ...)
    // So, at least for now, just always add it.
    //check( selected_layers.Contains( layerStack->GetCurrentLayer() ) );
    selected_layers.Add( layerStack->GetCurrentLayer() );

    // If the currentLayer is not in the selected layer list, just use it
    // - if currentLayer == GetCurrentLayer(): always inside the list so never go there
    // - if currentLayer == mFocusedLayer: if outside the selection, only modify it
    if( !selected_layers.Contains( currentLayer ) )
    {
        selected_layers.Empty();
        selected_layers.Add( currentLayer );
    }

    FScopedTransaction ScopedTransaction(LOCTEXT("global-layers-shortcuts.transaction.set-current-layer-blend-mode-to-previous-blend-mode", "Set Current Layer Blend Mode To Previous Blend Mode"));

    for( UOdysseyLayer* layer : selected_layers )
    {
        EOdysseyBlendingMode currentBlendMode = layer->GetBlendMode();
        int8 prevBlendingModeInt = ( static_cast<int8>( currentBlendMode ) - 1 + static_cast<int8>( EOdysseyBlendingMode::kBlendingMode_Count ) ) % static_cast<int8>( EOdysseyBlendingMode::kBlendingMode_Count );
        EOdysseyBlendingMode prevBlendMode = static_cast<EOdysseyBlendingMode>( prevBlendingModeInt );

        layer->SetBlendMode( prevBlendMode );
    }
}

void
FOdysseyLayerStackGlobalShortcuts::Action_SetCurrentLayerBlendMode(EOdysseyBlendingMode iBlendMode)
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if ( !layerStack )
        return;

    UOdysseyLayer* currentLayer = mFocusedLayer.IsBound() ? mFocusedLayer.Get() : layerStack->GetCurrentLayer();
    if ( !currentLayer )
        return;

    if ( !currentLayer->IsEditable() )
        return;

    TSet<UOdysseyLayer*> selected_layers;
    for( UOdysseyLayer* layer : layerStack->GetLayers() )
    {
        if( layerStack->IsLayerSelected( layer ) )
            selected_layers.Add( layer );
    }
    // Generally, the current layer is selected except when the layer stack is created (before any click interactions in layer stack header)
    // But too much interrogations to fix it (as many callbacks can be called.
    // (add a flag in SetCurrentLayer() to deselect all and select only the new current layer or in FOdysseyLayerSelection or ...)
    // So, at least for now, just always add it.
    //check( selected_layers.Contains( layerStack->GetCurrentLayer() ) );
    selected_layers.Add( layerStack->GetCurrentLayer() );

    // If the currentLayer is not in the selected layer list, just use it
    // - if currentLayer == GetCurrentLayer(): always inside the list so never go there
    // - if currentLayer == mFocusedLayer: if outside the selection, only modify it
    if( !selected_layers.Contains( currentLayer ) )
    {
        selected_layers.Empty();
        selected_layers.Add( currentLayer );
    }

    FScopedTransaction ScopedTransaction(LOCTEXT("global-layers-shortcuts.transaction.set-current-layer-blend-mode", "Set Current Layer Blend Mode"));

    for( UOdysseyLayer* layer : selected_layers )
    {
        layer->SetBlendMode( iBlendMode );
    }
}

bool
FOdysseyLayerStackGlobalShortcuts::CanAction_AlterLayer()
{
    UOdysseyLayerStack* layerStack = mLayerStack.Get();
    if ( !layerStack )
        return false;

    UOdysseyLayer* currentLayer = mFocusedLayer.IsBound() ? mFocusedLayer.Get() : layerStack->GetCurrentLayer();
    if ( !currentLayer )
        return false;

    return currentLayer->IsEditable() ? true : false;
}

#undef LOCTEXT_NAMESPACE
