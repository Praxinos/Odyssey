// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class FEposSequenceEditorCommands
    : public TCommands<FEposSequenceEditorCommands>
{
public:
    /** Default constructor. */
    FEposSequenceEditorCommands();

    /** Initialize commands */
    virtual void RegisterCommands() override;

public:
    TSharedPtr<FUICommandInfo> ToggleStoryboardViewportCommand;

    TSharedPtr<FUICommandInfo> GotoPraxinos;
    TSharedPtr<FUICommandInfo> GotoForum;
    TSharedPtr<FUICommandInfo> GotoUserDocumentation;
    TSharedPtr<FUICommandInfo> OpenAboutWindow;
};
