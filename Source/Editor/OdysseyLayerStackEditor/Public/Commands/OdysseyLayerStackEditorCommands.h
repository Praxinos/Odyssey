// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "EditorStyleSet.h"
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
    /** Action flatten the selected layers */
    TSharedPtr<FUICommandInfo> FlattenSelectedLayers;
};
