// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "EposSequenceEditorCommands.h"

#include "ISettingsModule.h"

#include "Misc/SAboutWindow.h"
#include "Styles/EposSequenceEditorStyle.h"

#define LOCTEXT_NAMESPACE "EposSequenceEditorCommands"

FEposSequenceEditorCommands::FEposSequenceEditorCommands()
    : TCommands<FEposSequenceEditorCommands>( "EposSequenceCommands" /* must match Set() parameter in style*/, LOCTEXT("EposSequenceEditorStyle", "Epos Editor"), NAME_None, FEposSequenceEditorStyle::Get()->GetStyleSetName() )
{
}

void
FEposSequenceEditorCommands::RegisterCommands()
{
    UI_COMMAND( NewStoryboardWithSettings,          "New Storyboard", "Create a new storyboard with settings", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( ToggleStoryboardViewportCommand,    "Storyboard Viewport", "A viewport layout tailored to storyboard preview", EUserInterfaceActionType::RadioButton, FInputChord() );

    //---

    UI_COMMAND( CreateCameraAtCurrentTime,          "Create Camera", "Create a new camera and set it as the current camera cut", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SnapCameraToViewportAtCurrentTime,  "Snap Camera to Viewport", "Snap the existing camera to the viewport", EUserInterfaceActionType::Button, FInputChord() );

    UI_COMMAND( CreatePlaneAtCurrentTime,           "Create Plane", "Create a new plane in front of the current camera", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( DetachPlaneAtCurrentTime,           "Detach Plane", "Detach the plane of the current camera", EUserInterfaceActionType::Button, FInputChord() );

    UI_COMMAND( CreateDrawingAtCurrentTime,         "Create Drawing", "Create a new drawing", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( GotoPreviousDrawing,                "Go to Previous Drawing", "Go to the previous drawing", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( GotoNextDrawing,                    "Go to Next Drawing", "Go to the next drawing", EUserInterfaceActionType::Button, FInputChord() );

    //---

    UI_COMMAND( OpenSequenceEditorSettings,         "Sequence Settings...", "Open sequence settings", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( OpenTrackEditorSettings,            "Track Settings...", "Open track settings", EUserInterfaceActionType::Button, FInputChord() );

    UI_COMMAND( GotoPraxinos,                       "Praxinos website...", "Go to Praxinos website", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( GotoForum,                          "Praxinos forum...", "Go to Praxinos forum", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( GotoDiscord,                        "Praxinos discord...", "Go to Praxinos discord", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( GotoUserDocumentation,              "Epos User Documentation...", "Go to User Documentation", EUserInterfaceActionType::Button, FInputChord() );

    UI_COMMAND( OpenAboutWindow,                    "About Epos...", "Open the About window", EUserInterfaceActionType::Button, FInputChord() );
}

//---

//static
void
FEposSequenceEditorActionCallbacks::OpenSequenceEditorSettings()
{
    FModuleManager::LoadModuleChecked<ISettingsModule>( "Settings" ).ShowViewer( "Editor", "Plugins", "EposSequenceEditorSettings" );
}

//static
void
FEposSequenceEditorActionCallbacks::OpenTrackEditorSettings()
{
    FModuleManager::LoadModuleChecked<ISettingsModule>( "Settings" ).ShowViewer( "Editor", "Plugins", "EposTracksEditorSettings" );
}

//static
void
FEposSequenceEditorActionCallbacks::GotoPraxinos()
{
    FPlatformProcess::LaunchURL( TEXT( "https://praxinos.coop" ), nullptr, nullptr );
}

//static
void
FEposSequenceEditorActionCallbacks::GotoForum()
{
    FPlatformProcess::LaunchURL( TEXT( "https://praxinos.coop/forum/" ), nullptr, nullptr );
}

//static
void
FEposSequenceEditorActionCallbacks::GotoDiscord()
{
    FPlatformProcess::LaunchURL( TEXT( "https://discord.gg/gEd6pj7" ), nullptr, nullptr );
}

//static
void
FEposSequenceEditorActionCallbacks::GotoUserDocumentation()
{
    FPlatformProcess::LaunchURL( TEXT( "https://praxinos.coop/Documentation/Epos/User/html/" ), nullptr, nullptr );
}

//static
void
FEposSequenceEditorActionCallbacks::OpenAboutWindow()
{
    TSharedPtr<SWindow> root = FGlobalTabmanager::Get()->GetRootWindow();
    SAboutWindow::Open( root );
}


#undef LOCTEXT_NAMESPACE
