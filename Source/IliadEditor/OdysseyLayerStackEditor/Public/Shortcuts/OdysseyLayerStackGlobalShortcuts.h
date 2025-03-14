// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once
#include "OdysseyEditorShortcuts.h"

class UOdysseyLayerStack;

class ODYSSEYLAYERSTACKEDITOR_API FOdysseyLayerStackGlobalShortcuts
    : public IOdysseyEditorShortcuts

{
public:
    virtual ~FOdysseyLayerStackGlobalShortcuts() {}
    FOdysseyLayerStackGlobalShortcuts(TAttribute<UOdysseyLayerStack*> iLayerStack);

public:
    //Common Shortcuts
    void Action_NavigateToNextLayer();
    void Action_NavigateToPreviousLayer();
    void Action_OpenFolderLayer();
    void Action_CloseFolderLayer();

public:
    //Shortcuts
    virtual void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList) override;

private:
    TAttribute<UOdysseyLayerStack*> mLayerStack;
};
