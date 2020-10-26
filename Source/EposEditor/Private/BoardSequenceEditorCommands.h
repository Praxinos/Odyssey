// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

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

	TSharedPtr<FUICommandInfo> ArrangeShots;
	TSharedPtr<FUICommandInfo> ArrangeShotsOnOneRow;
	TSharedPtr<FUICommandInfo> ArrangeShotsOnTwoRows;
};
