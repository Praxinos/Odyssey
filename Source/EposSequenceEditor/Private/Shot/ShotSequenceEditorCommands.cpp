// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Shot/ShotSequenceEditorCommands.h"

#include "Styles/EposSequenceEditorStyle.h"

#define LOCTEXT_NAMESPACE "ShotSequenceEditorCommands"

FShotSequenceEditorCommands::FShotSequenceEditorCommands()
    : TCommands<FShotSequenceEditorCommands>("ShotSequenceEditor" /* must match Set() parameter in style*/, LOCTEXT("ShotSequenceEditorStyle", "Epos Editor (Shot)"), NAME_None, FEposSequenceEditorStyle::Get()->GetStyleSetName() )
{
}

void
FShotSequenceEditorCommands::RegisterCommands()
{
    UI_COMMAND( CreateCameraAtCurrentTime,  "Create Camera", "Create a new camera and set it as the current camera cut", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SnapCameraToViewport,       "Snap Camera to Viewport", "Snap the existing camera to the viewport", EUserInterfaceActionType::Button, FInputChord() );

    UI_COMMAND( CreatePlane,                "Create Plane", "Create a new plane in front of the current camera", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( DetachPlane,                "Detach Plane", "Detach the plane of the current camera", EUserInterfaceActionType::Button, FInputChord() );

    UI_COMMAND( CreateDrawing,              "Create Drawing", "Create a new drawing", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( GotoPreviousDrawing,        "Go to Previous Drawing", "Go to the previous drawing", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( GotoNextDrawing,            "Go to Next Drawing", "Go to the next drawing", EUserInterfaceActionType::Button, FInputChord() );
}

#undef LOCTEXT_NAMESPACE
