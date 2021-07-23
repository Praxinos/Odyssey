// IDDN.FR.001.220036.000.S.P.2021.000.00000
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

    TSharedPtr<FUICommandInfo> CreateCameraAtCurrentTime;
    TSharedPtr<FUICommandInfo> SnapCameraToViewportAtCurrentTime;

    TSharedPtr<FUICommandInfo> CreatePlaneAtCurrentTime;
    TSharedPtr<FUICommandInfo> DetachPlane;

    TSharedPtr<FUICommandInfo> CreateDrawing;
    TSharedPtr<FUICommandInfo> GotoPreviousDrawing;
    TSharedPtr<FUICommandInfo> GotoNextDrawing;
};
