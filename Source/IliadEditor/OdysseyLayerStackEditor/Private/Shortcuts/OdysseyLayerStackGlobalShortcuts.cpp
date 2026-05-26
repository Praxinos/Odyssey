// IDDN.FR.001.060015.014.S.X.2019.000.00000
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
