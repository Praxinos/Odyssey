// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "LayerStack/FOdysseyLayerStackModel.h"

class FOdysseyLayerStackModel;

class FOdysseyLayerAddMenu
{
public:
    FOdysseyLayerAddMenu( TSharedRef<FOdysseyLayerStackModel> iLayerStack );

public:
    void BuildAddLayerMenu(FMenuBuilder& iMenuBuilder);

private: //Callbacks

    /** Callback for executing the "Add Image Layer" menu entry. */
    void HandleAddImageLayerMenuEntryExecute();
    bool HandleAddImageLayerMenuEntryCanExecute() const;
    
    /** Callback for executing the "Add folderr" menu entry. */
    void HandleAddFolderLayerMenuEntryExecute();
    bool HandleAddFolderLayerMenuEntryCanExecute() const;

private:
    TSharedRef<FOdysseyLayerStackModel> mLayerStackRef;
};

