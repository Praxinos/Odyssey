// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

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

