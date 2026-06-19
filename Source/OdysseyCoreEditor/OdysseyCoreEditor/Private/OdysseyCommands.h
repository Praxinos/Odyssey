// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class FOdysseyCommands
    : public TCommands<FOdysseyCommands>
{
public:
    /** Default constructor. */
    FOdysseyCommands();

    /** Initialize commands */
    virtual void RegisterCommands() override;

public:
    TSharedPtr<FUICommandInfo> OpenOdysseyDocumentation;
    TSharedPtr<FUICommandInfo> OpenOdysseyAboutWindow;
};
