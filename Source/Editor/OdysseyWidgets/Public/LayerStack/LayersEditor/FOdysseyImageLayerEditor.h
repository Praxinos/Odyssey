// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "LayerStack/LayersEditor/IOdysseyLayerEditor.h"

/**
 * Implements menu GUI for the Image Layer type
 */
class FOdysseyImageLayerEditor : public IOdysseyLayerEditor
{
    using IOdysseyLayerEditor::IOdysseyLayerEditor;

public: //Construction/Destruction
    static TSharedRef<IOdysseyLayerEditor> CreateLayerEditor(TSharedRef<FOdysseyLayerStackModel> InLayerStackModel);

public: //Implementation of IOdysseyLayerEditor

    virtual FOdysseyImageLayer* AddLayer(IOdysseyLayer* FocusedLayer) override;
    virtual void BuildAddLayerMenu(FMenuBuilder& MenuBuilder) override;
    virtual const FSlateBrush* GetIconBrush() const override;
    virtual bool IsResizable(FOdysseyImageLayer* InLayer) const override;
    virtual void Resize(float NewSize, FOdysseyImageLayer* InTrack) override;
    virtual bool GetDefaultExpansionState(FOdysseyImageLayer* InTrack) const override;

private: //Callbacks

    /** Callback for executing the "Add Image Layer" menu entry. */
    void HandleAddImageLayerMenuEntryExecute();
    bool HandleAddImageLayerMenuEntryCanExecute() const;

};

