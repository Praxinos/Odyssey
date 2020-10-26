// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class FShotSequenceEditorCommands
	: public TCommands<FShotSequenceEditorCommands>
{
public:

	/** Default constructor. */
    FShotSequenceEditorCommands();

	/** Initialize commands */
	virtual void RegisterCommands() override;

public:

	TSharedPtr<FUICommandInfo> NewShotWithSettings;

	TSharedPtr<FUICommandInfo> CreateCamera;
	TSharedPtr<FUICommandInfo> CreatePlane;
	TSharedPtr<FUICommandInfo> SnapCameraToViewport;
};
