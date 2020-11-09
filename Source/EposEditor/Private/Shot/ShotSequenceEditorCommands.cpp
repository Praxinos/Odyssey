// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Shot/ShotSequenceEditorCommands.h"

#include "Styles/EposEditorStyle.h"

#define LOCTEXT_NAMESPACE "ShotSequenceEditorCommands"

FShotSequenceEditorCommands::FShotSequenceEditorCommands()
    : TCommands<FShotSequenceEditorCommands>("ShotSequenceEditor" /* must match Set() parameter in style*/, LOCTEXT("ShotSequenceEditorStyle", "Epos Editor (Shot)"), NAME_None, FEposEditorStyle::Get()->GetStyleSetName() )
{
}

void
FShotSequenceEditorCommands::RegisterCommands()
{
    UI_COMMAND( NewShotWithSettings,    "New Shot", "Create a new shot with settings", EUserInterfaceActionType::Button, FInputChord() );

    UI_COMMAND( CreateCamera,           "Create Camera", "Create a new camera and set it as the current camera cut", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( CreatePlane,            "Create Plane", "Create a new plane in front of the current camera", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SnapCameraToViewport,   "Snap Camera to Viewport", "Snap the existing camera to the viewport", EUserInterfaceActionType::Button, FInputChord() );
}

#undef LOCTEXT_NAMESPACE
