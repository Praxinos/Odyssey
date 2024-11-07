// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "OdysseyStyleSet.h"

class FOdysseyViewportDrawingEditorCommands : public TCommands<FOdysseyViewportDrawingEditorCommands>
{
public:
    FOdysseyViewportDrawingEditorCommands();

    /**
    * Initialize commands
    */
    virtual void RegisterCommands() override;

public:
};
