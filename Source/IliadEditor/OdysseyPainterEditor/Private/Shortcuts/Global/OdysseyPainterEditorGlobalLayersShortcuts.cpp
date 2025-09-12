// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Shortcuts/Global/OdysseyPainterEditorGlobalLayersShortcuts.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationPlayer.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyAnimationCellImageRaster.h"
#include "OdysseyLayer.h"
#include "OdysseyLayerStack.h"
#include "OdysseyPainterEditorCommands.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseyTextureLayerStack.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyAnimationCurrentFrameMutator.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

FOdysseyPainterEditorGlobalLayersShortcuts::FOdysseyPainterEditorGlobalLayersShortcuts(FOdysseyPainterEditor* iEditor)
    : mEditor(iEditor)
{
}

void
FOdysseyPainterEditorGlobalLayersShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    if (!mEditor)
        return;

    //Generic tool shortcuts

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().CreateNewLayer,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalLayersShortcuts::Action_CreateNewLayer),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalLayersShortcuts::CanAction_CreateNewLayer)
    );

    for (int i = 0; i < FOdysseyPainterEditorCommands::Get().ChangeLayerOpacity.Num(); i++)
    {
        iCommandList->MapAction(
            FOdysseyPainterEditorCommands::Get().ChangeLayerOpacity[i],
            FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalLayersShortcuts::Action_ChangeLayerOpacity, i * 10.f),
            FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalLayersShortcuts::CanAction_ChangeLayerOpacity)
        );
    }

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().SetCurrentLayerBlendModeToNextBlendMode,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalLayersShortcuts::Action_SetCurrentLayerBlendModeToNextBlendMode)
      , FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalLayersShortcuts::CanAction_AlterLayer)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().SetCurrentLayerBlendModeToPreviousBlendMode,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalLayersShortcuts::Action_SetCurrentLayerBlendModeToPreviousBlendMode)
      , FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalLayersShortcuts::CanAction_AlterLayer)
    );

    for (int i = 0; EOdysseyBlendingMode blendMode : TEnumRange<EOdysseyBlendingMode>())
    {
        iCommandList->MapAction(
            FOdysseyPainterEditorCommands::Get().SetCurrentLayerBlendMode[i++],
            FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalLayersShortcuts::Action_SetCurrentLayerBlendMode, blendMode),
            FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalLayersShortcuts::CanAction_AlterLayer)
        );
    }
}

void
FOdysseyPainterEditorGlobalLayersShortcuts::Action_CreateNewLayer()
{
    UOdysseyLayerStack* layerStack = mEditor->LayerStack();
    if ( !layerStack )
        return;

    UOdysseyLayer* layer = nullptr;
    {
    #if WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("global-layers-shortcuts.transaction.create-new-layer", "Add Layer"));
    #endif
        layerStack->Modify();

        UOdysseyLayer* currentLayer = layerStack->GetCurrentLayer();
        UOdysseyLayer* parent = nullptr;
        int index = INDEX_NONE;
        if (currentLayer)
        {
            parent = currentLayer;
            index = INDEX_NONE;
            if (!currentLayer->CanHaveChildren() || !currentLayer->ShouldDisplayChildren())
            {
                parent = currentLayer->GetParent();
                index = currentLayer->GetIndexInParent();
            }
        }

        if (layerStack->IsA<UOdysseyAnimationLayerStack>())
        {
            layer = layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass(), parent, index);

            UOdysseyAnimationLayerImageRaster* animLayer = Cast<UOdysseyAnimationLayerImageRaster>(layer);
            if (!animLayer)
                return;

            UOdysseyAnimation* animation = animLayer->GetAnimation();
            if (!animation)
                return;

            UOdysseyAnimationPlayer* player = mEditor->GetAnimationPlayer();
            if (!player)
                return;

            animLayer->AddCell(UOdysseyAnimationCellImageRaster::StaticClass());
            animLayer->SetCellsOffset(player->GetCurrentFrame().FrameNumber.Value);

            FOdysseyAnimationCurrentFrameMutator currentFrameMutator(player);
            currentFrameMutator.Set(player->GetCurrentFrame().FrameNumber.Value);
            currentFrameMutator.Commit();
        }
        else if (layerStack->IsA<UOdysseyTextureLayerStack>())
        {
            layer = layerStack->AddLayer(UOdysseyTextureLayerImageRaster::StaticClass(), parent, index);
        }
    }

    layerStack->SetCurrentLayer(layer);
}

void
FOdysseyPainterEditorGlobalLayersShortcuts::Action_ChangeLayerOpacity(float iOpacity)
{
    UOdysseyLayerStack* layerStack = mEditor->LayerStack();
    if ( !layerStack )
        return;

    if ( !layerStack->GetCurrentLayer() )
        return;

    if ( !layerStack->GetCurrentLayer()->IsEditable() )
        return;


#if WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("global-layers-shortcuts.transaction.set-layer-opacity", "Change Layer Opacity"));
#endif
    layerStack->GetCurrentLayer()->SetOpacity(FMath::Clamp(iOpacity, 0.f, 1.f));
}

bool
FOdysseyPainterEditorGlobalLayersShortcuts::CanAction_CreateNewLayer()
{
    UOdysseyLayerStack* layerStack = mEditor->LayerStack();
    if ( !layerStack )
        return false;

    return true;
}

bool
FOdysseyPainterEditorGlobalLayersShortcuts::CanAction_ChangeLayerOpacity()
{
    UOdysseyLayerStack* layerStack = mEditor->LayerStack();
    if ( !layerStack )
        return false;

    if ( !layerStack->GetCurrentLayer() )
        return false;

    if ( !layerStack->GetCurrentLayer()->IsEditable() )
        return false;

    return true;
}

void
FOdysseyPainterEditorGlobalLayersShortcuts::Action_SetCurrentLayerBlendModeToNextBlendMode()
{
    UOdysseyLayerStack* layerStack = mEditor->LayerStack();

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
FOdysseyPainterEditorGlobalLayersShortcuts::Action_SetCurrentLayerBlendModeToPreviousBlendMode()
{
    UOdysseyLayerStack* layerStack = mEditor->LayerStack();


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
FOdysseyPainterEditorGlobalLayersShortcuts::Action_SetCurrentLayerBlendMode(EOdysseyBlendingMode iBlendMode)
{
    UOdysseyLayerStack* layerStack = mEditor->LayerStack();
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
FOdysseyPainterEditorGlobalLayersShortcuts::CanAction_AlterLayer()
{
    UOdysseyLayerStack* layerStack = mEditor->LayerStack();
    if ( !layerStack )
        return false;

    if ( !layerStack->GetCurrentLayer() )
        return false;

    return layerStack->GetCurrentLayer()->IsEditable() ? true : false;
}

#undef LOCTEXT_NAMESPACE
