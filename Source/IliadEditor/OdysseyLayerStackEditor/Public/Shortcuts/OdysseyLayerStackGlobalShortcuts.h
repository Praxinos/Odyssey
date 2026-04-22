// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once
#include "OdysseyEditorShortcuts.h"
#include "OdysseyBlendingMode.h"

class UOdysseyLayer;
class UOdysseyLayerStack;

class ODYSSEYLAYERSTACKEDITOR_API FOdysseyLayerStackGlobalShortcuts
    : public IOdysseyEditorShortcuts

{
public:
    virtual ~FOdysseyLayerStackGlobalShortcuts() {}
    FOdysseyLayerStackGlobalShortcuts(TAttribute<UOdysseyLayerStack*> iLayerStack, TAttribute<UOdysseyLayer*> iLayer=TAttribute<UOdysseyLayer*>());

public:
    //Common Shortcuts
    void Action_NavigateToNextLayer();
    void Action_NavigateToPreviousLayer();
    void Action_OpenFolderLayer();
    void Action_CloseFolderLayer();

    void Action_SetCurrentLayerBlendMode(EOdysseyBlendingMode iBlendMode);
    void Action_SetCurrentLayerBlendModeToNextBlendMode();
    void Action_SetCurrentLayerBlendModeToPreviousBlendMode();

    bool CanAction_AlterLayer();

public:
    //Shortcuts
    virtual void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList) override;

private:
    TAttribute<UOdysseyLayerStack*> mLayerStack;
    // It's not necessary the current layer (when modifying a parameter of a layer which is NOT selected/current)
    // Only "really" used by Action_SetCurrentLayerBlendMode() as this shortcut is called from the layer popup
    TAttribute<UOdysseyLayer*> mFocusedLayer;
};
