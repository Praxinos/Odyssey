// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class FEposTracksEditorCommands
    : public TCommands<FEposTracksEditorCommands>
{
public:
    /** Default constructor. */
    FEposTracksEditorCommands();

    /** Initialize commands */
    virtual void RegisterCommands() override;

public:
    TSharedPtr<FUICommandInfo> ArrangeShotsManually;
    TSharedPtr<FUICommandInfo> ArrangeShotsOnOneRow;
    TSharedPtr<FUICommandInfo> ArrangeShotsOnTwoRows;

    TSharedPtr<FUICommandInfo> NewSectionWithBoardAtCurrentFrame;
    TSharedPtr<FUICommandInfo> NewSectionWithShotAtCurrentFrame;

    TSharedPtr<FUICommandInfo> NewSectionWithNoteAtCurrentFrame;
};
