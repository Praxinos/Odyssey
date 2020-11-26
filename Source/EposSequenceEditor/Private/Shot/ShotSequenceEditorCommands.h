// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

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

    TSharedPtr<FUICommandInfo> CreateCamera;
    TSharedPtr<FUICommandInfo> CreatePlane;
    TSharedPtr<FUICommandInfo> SnapCameraToViewport;
};
