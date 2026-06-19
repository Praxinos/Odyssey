// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

/**
 * Holds the UI commands for the OdysseyPainterEditorToolkit widget.
 */
class FOdysseyLayerStackEditorCommands
    : public TCommands<FOdysseyLayerStackEditorCommands>
{
public:
    /**
     * Default constructor.
     */
    FOdysseyLayerStackEditorCommands();

public:
    // TCommands interface
    virtual void RegisterCommands() override;

public:
    /** Action merge the selected layers */
    TSharedPtr<FUICommandInfo> MergeSelectedLayers;

    TSharedPtr<FUICommandInfo> NavigateToNextLayer;
    TSharedPtr<FUICommandInfo> NavigateToPreviousLayer;
    TSharedPtr<FUICommandInfo> OpenFolderLayer;
    TSharedPtr<FUICommandInfo> CloseFolderLayer;

    /** Action for changing the current layer's blending mode */
    TArray<TSharedPtr<FUICommandInfo>> SetCurrentLayerBlendMode;
    TSharedPtr<FUICommandInfo> SetCurrentLayerBlendModeToNextBlendMode;
    TSharedPtr<FUICommandInfo> SetCurrentLayerBlendModeToPreviousBlendMode;
};
