// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

/**
 * Holds the UI commands for the OdysseyTexture2DEditorToolkit widget.
 */
class FOdysseyTexture2DEditorCommands
    : public TCommands<FOdysseyTexture2DEditorCommands>
{
public:
    /**
     * Default constructor.
     */
    FOdysseyTexture2DEditorCommands();

public:
    // TCommands interface
    virtual void RegisterCommands() override;

public:
};
