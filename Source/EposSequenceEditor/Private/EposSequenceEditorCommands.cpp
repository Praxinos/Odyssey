// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "EposSequenceEditorCommands.h"

#include "ISettingsModule.h"

#include "Widgets/SAboutWindow.h"
#include "Styles/EposSequenceEditorStyle.h"

#define LOCTEXT_NAMESPACE "EposSequenceEditorCommands"

namespace
{
const FName ViewportRotationBundleName = "ViewportRotation";
}

FEposSequenceEditorCommands::FEposSequenceEditorCommands()
    : TCommands<FEposSequenceEditorCommands>( "EposSequenceCommands" /* must match Set() parameter in style*/, LOCTEXT("EposSequenceEditorStyle", "Epos Editor"), NAME_None, FEposSequenceEditorStyle::Get().GetStyleSetName() )
{
    AddBundle( ViewportRotationBundleName, LOCTEXT( "CommandsCategory.ViewportRotation", "Viewport Rotation" ) );
}

void
FEposSequenceEditorCommands::RegisterCommands()
{
    UI_COMMAND( NewStoryboardWithSettings,          "New Storyboard", "Create a new storyboard with settings", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( ToggleStoryboardViewportCommand,    "Storyboard Viewport", "A viewport layout tailored to storyboard preview", EUserInterfaceActionType::RadioButton, FInputChord() );

    //---

    UI_COMMAND( StoryboardViewportAdd10Rotate,      "Storyboard Viewport +10°", "+10° to the viewport rotation", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( StoryboardViewportSubstract10Rotate,"Storyboard Viewport -10°", "-10° to the viewport rotation", EUserInterfaceActionType::Button, FInputChord() );

    FTextFormat rotation_label_format( LOCTEXT( "storyboard-viewport-rotation-label", "Set Storyboard Viewport to {0}°" ) );
    FTextFormat rotation_tooltip_format( LOCTEXT( "storyboard-viewport-rotation-tooltip", "Set the viewport rotation to {0}°" ) );

    TArray<int32> angles = { -135, -90, -45, 0, 45, 90, 135, 180 };
    for( auto angle : angles )
    {
        StoryboardViewportSetRotationX.Add( angle,
                                            FUICommandInfoDecl(
                                                this->AsShared(),
                                                FName( *FString::Printf( TEXT( "StoryboardViewportRotate-%d" ), angle ) ),
                                                FText::Format( rotation_label_format, angle ),
                                                FText::Format( rotation_tooltip_format, angle ),
                                                ViewportRotationBundleName )
                                            .UserInterfaceType( EUserInterfaceActionType::Check )
                                            .DefaultChord( FInputChord() )
        );
    }

    //---

    UI_COMMAND( CreateCameraAtCurrentTime,          "Create Camera", "Create a new camera and set it as the current camera cut", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SnapCameraToViewportAtCurrentTime,  "Snap Camera to Viewport", "Snap the existing camera to the viewport", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( PilotCameraAtCurrentTime,           "Pilot Camera", "Pilot the existing camera", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( EjectCameraAtCurrentTime,           "Eject Camera", "Eject the existing camera", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( GotoPreviousCameraPosition,         "Go to Previous Camera Position", "Go to the previous camera position", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( GotoNextCameraPosition,             "Go to Next Camera Position", "Go to the next camera position", EUserInterfaceActionType::Button, FInputChord() );

    UI_COMMAND( CreatePlaneAtCurrentTime,           "Create Plane", "Create a new plane in front of the current camera", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( DetachPlaneAtCurrentTime,           "Detach Plane", "Detach the plane of the current camera", EUserInterfaceActionType::Button, FInputChord() );

    UI_COMMAND( CreateDrawingAtCurrentTime,         "Create Drawing", "Create a new drawing", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( GotoPreviousDrawing,                "Go to Previous Drawing", "Go to the previous drawing", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( GotoNextDrawing,                    "Go to Next Drawing", "Go to the next drawing", EUserInterfaceActionType::Button, FInputChord() );

    //---

    UI_COMMAND( OpenSequenceEditorSettings,         "Sequence Settings...", "Open sequence settings", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( OpenTrackEditorSettings,            "Track Settings...", "Open track settings", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( OpenNamingConventionEditorSettings, "Naming Convention Settings...", "Open naming convention settings", EUserInterfaceActionType::Button, FInputChord() );

    UI_COMMAND( GotoPraxinos,                       "Praxinos website...", "Go to Praxinos website", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( GotoForum,                          "Praxinos forum...", "Go to Praxinos forum", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( GotoDiscord,                        "Praxinos discord...", "Go to Praxinos discord", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( GotoUserDocumentation,              "Epos User Documentation...", "Go to User Documentation", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( GotoProjects,                       "Epos Samples...", "Download projects made by Epos", EUserInterfaceActionType::Button, FInputChord() );

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
FEposSequenceEditorActionCallbacks::OpenNamingConventionEditorSettings()
{
    FModuleManager::LoadModuleChecked<ISettingsModule>( "Settings" ).ShowViewer( "Editor", "Plugins", "NamingConventionSettings" );
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
    FPlatformProcess::LaunchURL( TEXT( "https://praxinos.coop/epos-user-doc" ), nullptr, nullptr );
}

//static
void
FEposSequenceEditorActionCallbacks::GotoProjects()
{
    FPlatformProcess::LaunchURL( TEXT( "https://praxinos.coop/epos-projects" ), nullptr, nullptr );
}

//static
void
FEposSequenceEditorActionCallbacks::OpenAboutWindow()
{
    TSharedPtr<SWindow> root = FGlobalTabmanager::Get()->GetRootWindow();
    SAboutWindow::Open( root );
}


#undef LOCTEXT_NAMESPACE
