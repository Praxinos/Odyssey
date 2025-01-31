// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "EposSequenceEditorCommands.h"

#include "Framework/Docking/TabManager.h"
#include "ISettingsModule.h"
#include "Modules/ModuleManager.h"

#include "Widgets/SAboutWindow.h"
#include "Styles/EposSequenceEditorStyle.h"

#define LOCTEXT_NAMESPACE "EposSequenceEditorCommands"

namespace
{
const FName ViewportRotationBundleName = "ViewportRotation";
const FName ViewportZoomBundleName = "ViewportZoom";
}

FEposSequenceEditorCommands::FEposSequenceEditorCommands()
    : TCommands<FEposSequenceEditorCommands>( "EposSequenceCommands" /* must match Set() parameter in style*/, LOCTEXT("EposSequenceEditorStyle", "Epos Editor"), NAME_None, FEposSequenceEditorStyle::Get().GetStyleSetName() )
{
    AddBundle( ViewportRotationBundleName, LOCTEXT( "CommandsCategory.ViewportRotation", "Viewport Rotation" ) );
    AddBundle( ViewportZoomBundleName, LOCTEXT( "CommandsCategory.ViewportZoom", "Viewport Zoom" ) );
}

void
FEposSequenceEditorCommands::RegisterCommands()
{
    UI_COMMAND( NewStoryboardWithSettings,          "Start Storyboarding", "Create a new storyboard with settings", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( NewStoryboardImportImageSequence,   "Import Image Sequence", "Create a new storyboard from a sequence of images", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( ToggleStoryboardViewportCommand,    "Odyssey Viewport", "A viewport layout tailored to storyboard/animation preview", EUserInterfaceActionType::RadioButton, FInputChord() );

    //---

    UI_COMMAND( StoryboardViewportAdd10Rotate,      "Odyssey Viewport Rotation +10°", "+10° to the viewport rotation", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( StoryboardViewportSubstract10Rotate,"Odyssey Viewport Rotation -10°", "-10° to the viewport rotation", EUserInterfaceActionType::Button, FInputChord() );

    UI_COMMAND( StoryboardViewportAdd10Zoom,      "Odyssey Viewport Zoom +10%", "+10% to the viewport Zoom", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( StoryboardViewportSubstract10Zoom,"Odyssey Viewport Zoom -10%", "-10% to the viewport Zoom", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( StoryboardViewportResetPanZoomRotate, "Odyssey Viewport Reset Pan, Zoom and Rotation ", "Resets the viewport Pan, Zoom and Rotation values", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control | EModifierKey::Shift, EKeys::M) );

    FTextFormat rotation_label_format( LOCTEXT( "odyssey-viewport-rotation-label", "Set Odyssey Viewport to {0}°" ) );
    FTextFormat rotation_tooltip_format( LOCTEXT( "odyssey-viewport-rotation-tooltip", "Set the viewport rotation to {0}°" ) );

    TArray<int32> angles = { -135, -90, -45, 0, 45, 90, 135, 180 };
    for( auto angle : angles )
    {
        StoryboardViewportSetRotationX.Add( angle,
                                            FUICommandInfoDecl(
                                                this->AsShared(),
                                                FName( *FString::Printf( TEXT( "OdysseyViewportRotate-%d" ), angle ) ),
                                                FText::Format( rotation_label_format, angle ),
                                                FText::Format( rotation_tooltip_format, angle ),
                                                ViewportRotationBundleName )
                                            .UserInterfaceType( EUserInterfaceActionType::Check )
                                            .DefaultChord( FInputChord() )
        );
    }

    FTextFormat zoom_label_format( LOCTEXT( "odyssey-viewport-zoom-label", "Set Odyssey Viewport to {0}%" ) );
    FTextFormat zoom_tooltip_format( LOCTEXT( "odyssey-viewport-zoom-tooltip", "Set the viewport zoom to {0}%" ) );

    TArray<float> zooms = { 0.25f, 0.5f, 1.f, 2.f, 4.f };
    for( auto zoom : zooms )
    {
        StoryboardViewportSetZoomX.Add( zoom,
                                            FUICommandInfoDecl(
                                                this->AsShared(),
                                                FName( *FString::Printf( TEXT( "OdysseyViewportRotate-%d" ), FMath::RoundToInt32(zoom * 100.f) ) ),
                                                FText::Format( zoom_label_format, FMath::RoundToInt32(zoom * 100.f) ),
                                                FText::Format( zoom_tooltip_format, FMath::RoundToInt32(zoom * 100.f) ),
                                                ViewportZoomBundleName )
                                            .UserInterfaceType( EUserInterfaceActionType::Check )
                                            .DefaultChord( FInputChord() )
        );
    }

    UI_COMMAND( StoryboardViewportHoldToPanZoom, "Odyssey Viewport Pan/Zoom (Hold)", "Hold this key to pan (left mouse button) or zoom (right mouse button) the viewport", EUserInterfaceActionType::Button, FInputChord(EKeys::M) );
    UI_COMMAND( StoryboardViewportHoldToRotate, "Odyssey Viewport Rotate (Hold)", "Hold this key to to rotate the viewport using the left mouse button", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::M) );

    //---

    UI_COMMAND( FixActorReferences,                 "Fix Actor References", "Try to automatically fix up broken actor bindings", EUserInterfaceActionType::Button, FInputChord());

    //---

    UI_COMMAND( StepToPreviousShot,                 "Step to Previous Shot", "Step to previous shot", EUserInterfaceActionType::Button, FInputChord( EKeys::Y ) );
    UI_COMMAND( StepToNextShot,                     "Step to Next Shot", "Step to next shot", EUserInterfaceActionType::Button, FInputChord( EKeys::U ) );

    UI_COMMAND( DeactivateAllLighttables,           "Deactivate All Lighttables", "Deactivate all lighttables recursively from the root sequence", EUserInterfaceActionType::Button, FInputChord() );

    //---

    UI_COMMAND( CreateCameraAtCurrentTime,          "Create Camera", "Create a new camera and set it as the current camera cut", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SnapCameraToViewportAtCurrentTime,  "Snap Camera to Viewport", "Snap the existing camera to the viewport", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( PilotCameraAtCurrentTime,           "Pilot Camera", "Pilot the existing camera", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( EjectCameraAtCurrentTime,           "Eject Camera", "Eject the existing camera", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( GotoPreviousCameraPosition,         "Go to Previous Camera Position", "Go to the previous camera position", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( GotoNextCameraPosition,             "Go to Next Camera Position", "Go to the next camera position", EUserInterfaceActionType::Button, FInputChord() );

    UI_COMMAND( CreatePlaneAtCurrentTime,           "Create Plane", "Create a new plane in front of the current camera", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( CreateAnimationAtCurrentTime,       "Create Animation", "Create a new animation in front of the current camera", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( DetachPlaneAtCurrentTime,           "Detach Plane", "Detach the plane of the current camera", EUserInterfaceActionType::Button, FInputChord() );

    UI_COMMAND( CreateDrawingAtCurrentTime,         "Create Drawing", "Create a new drawing", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( GotoPreviousDrawing,                "Go to Previous Drawing", "Go to the previous drawing", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( GotoNextDrawing,                    "Go to Next Drawing", "Go to the next drawing", EUserInterfaceActionType::Button, FInputChord() );

    //---

    UI_COMMAND( OpenSequenceEditorSettings,         "Sequence Settings...", "Open sequence settings", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( OpenTrackEditorSettings,            "Track Settings...", "Open track settings", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( OpenNamingConventionEditorSettings, "Naming Convention Settings...", "Open naming convention settings", EUserInterfaceActionType::Button, FInputChord() );
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

//---

//static
void
FEposSequenceEditorActionCallbacks::MapActions( TSharedPtr<FUICommandList> ioCommandList )
{
    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().OpenSequenceEditorSettings,
        FExecuteAction::CreateStatic( &OpenSequenceEditorSettings )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().OpenTrackEditorSettings,
        FExecuteAction::CreateStatic( &OpenTrackEditorSettings )
    );

    ioCommandList->MapAction(
        FEposSequenceEditorCommands::Get().OpenNamingConventionEditorSettings,
        FExecuteAction::CreateStatic( &OpenNamingConventionEditorSettings )
    );
}

//static
void
FEposSequenceEditorActionCallbacks::UnmapActions( TSharedPtr<FUICommandList> ioCommandList )
{
    ioCommandList->UnmapAction( FEposSequenceEditorCommands::Get().OpenSequenceEditorSettings );
    ioCommandList->UnmapAction( FEposSequenceEditorCommands::Get().OpenTrackEditorSettings );
    ioCommandList->UnmapAction( FEposSequenceEditorCommands::Get().OpenNamingConventionEditorSettings );
}

#undef LOCTEXT_NAMESPACE
