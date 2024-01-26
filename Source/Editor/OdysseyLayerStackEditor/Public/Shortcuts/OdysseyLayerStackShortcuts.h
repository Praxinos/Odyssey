// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

class SOdysseyLayerStackTreeView;
class UOdysseyLayerStack;
class FUICommandList;

class ODYSSEYLAYERSTACKEDITOR_API FOdysseyLayerStackShortcuts
{
public:
    FOdysseyLayerStackShortcuts(TSharedPtr<SOdysseyLayerStackTreeView> iTreeView, UOdysseyLayerStack* iLayerStack);

public:
    TSharedRef<FUICommandList> GetCommandList() const;

    //Common Shortcuts
    void Action_Rename();
    void Action_Copy();
    void Action_Cut();
    void Action_Paste();
    void Action_SelectAll();
    void Action_Delete();
    void Action_Duplicate();

    bool CanAction_Rename();
    bool CanAction_Copy();
    bool CanAction_Cut();
    bool CanAction_Paste();
    bool CanAction_SelectAll();
    bool CanAction_Delete();
    bool CanAction_Duplicate();

    //Layers Specific Shortcuts
    void Action_MergeSelectedLayers();
    void Action_FlattenSelectedLayers();

    bool CanAction_MergeSelectedLayers();
    bool CanAction_FlattenSelectedLayers();

private:
    //Shortcuts
    void MapActionsToCommandList(); 

private:
    TSharedRef<FUICommandList> mCommandList;
    TWeakPtr<SOdysseyLayerStackTreeView> mTreeView;
    UOdysseyLayerStack* mLayerStack;
};