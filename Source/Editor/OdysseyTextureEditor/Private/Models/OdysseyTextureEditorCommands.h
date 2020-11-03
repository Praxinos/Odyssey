// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "EditorStyleSet.h"
#include "Framework/Commands/Commands.h"

/**
 * Holds the UI commands for the OdysseyTextureEditorToolkit widget.
 */
class FOdysseyTextureEditorCommands
    : public TCommands<FOdysseyTextureEditorCommands>
{
public:
    /**
     * Default constructor.
     */
    FOdysseyTextureEditorCommands();

public:
    // TCommands interface
    virtual void RegisterCommands() override;
};
