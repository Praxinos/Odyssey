// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class FBoardSequenceEditorCommands
    : public TCommands<FBoardSequenceEditorCommands>
{
public:

    /** Default constructor. */
    FBoardSequenceEditorCommands();

    /** Initialize commands */
    virtual void RegisterCommands() override;

public:

    TSharedPtr<FUICommandInfo> NewBoardWithSettings;

    TSharedPtr<FUICommandInfo> ArrangeShotsManually;
    TSharedPtr<FUICommandInfo> ArrangeShotsOnOneRow;
    TSharedPtr<FUICommandInfo> ArrangeShotsOnTwoRows;

    TSharedPtr<FUICommandInfo> NewSectionWithBoardAtCurrentFrame;
    TSharedPtr<FUICommandInfo> NewSectionWithShotAtCurrentFrame;
};
