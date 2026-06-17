// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyViewportCommands.h"
#include "OdysseyStyle.h"
#include "Command/OdysseyCommandMacros.h"

#define LOCTEXT_NAMESPACE "OdysseyWidgets"

namespace
{
    const FName ViewportShortcuts = "Viewport Shortcuts";
}

FOdysseyViewportCommands::FOdysseyViewportCommands()
    : TCommands<FOdysseyViewportCommands>( "OdysseyViewportCommands", LOCTEXT( "viewport-commands.name", "Odyssey Viewport" ), NAME_None, FOdysseyStyle::GetStyleSetName() )
{
    AddBundle(ViewportShortcuts, LOCTEXT("editor-commands.category.viewport-shortcuts", "Viewport Shortcuts"));
}

void
FOdysseyViewportCommands::RegisterCommands()
{
// Viewport Shortcuts Category
    UI_BUNDLE_COMMAND( PanZoomViewport, ViewportShortcuts,  "Pan / Zoom Viewport", "Hold the key to Pan (Left Mouse Button) or Zoom (Right Mouse Button) the viewport",                                        EUserInterfaceActionType::None, FInputChord(EKeys::M));
    UI_BUNDLE_COMMAND( RotateViewport, ViewportShortcuts,  "Rotate Viewport", "Hold the key to Rotate the viewport",                                        EUserInterfaceActionType::None, FInputChord(EModifierKey::Control, EKeys::M), FInputChord(EModifierKey::Shift, EKeys::M));

    UI_BUNDLE_COMMAND( ResetViewport1On1, ViewportShortcuts, "Reset Viewport 1:1", "Reset viewport to: 100% + 0° + centered",                               EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( ResetViewportFit, ViewportShortcuts, "Reset Viewport Fit", "Reset viewport to: view all the canvas + centered",                      EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::R ) );
    UI_BUNDLE_COMMAND( ResetViewportAll, ViewportShortcuts, "Reset Viewport All", "Reset viewport to: view all the canvas + 0° + centered + no flip",       EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( ResetViewportRotation, ViewportShortcuts, "Reset Viewport Rotation", "Reset viewport rotation",                                      EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( ResetViewportPosition, ViewportShortcuts, "Reset Viewport Position", "Reset viewport position",                                      EUserInterfaceActionType::Button, FInputChord( EKeys::Escape ) );
    UI_BUNDLE_COMMAND( ResetViewportZoom, ViewportShortcuts, "Reset Viewport Zoom", "Reset viewport zoom",                                                  EUserInterfaceActionType::Button, FInputChord() );

    UI_BUNDLE_COMMAND( RotateViewportLeft, ViewportShortcuts, "Rotate Viewport Left", "Rotate viewport left",                                               EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( RotateViewportRight, ViewportShortcuts, "Rotate Viewport Right", "Rotate viewport right",                                            EUserInterfaceActionType::Button, FInputChord() );

    UI_BUNDLE_COMMAND( FlipViewportHorizontally, ViewportShortcuts, "Flip the viewport horizontally", "Flip the viewport horizontally",                     EUserInterfaceActionType::ToggleButton, FInputChord() );
    UI_BUNDLE_COMMAND( FlipViewportVertically, ViewportShortcuts, "Flip the viewport vertically", "Flip the viewport vertically",                           EUserInterfaceActionType::ToggleButton, FInputChord() );

    UI_BUNDLE_COMMAND( SetRotationMinus135, ViewportShortcuts, "Set Rotation -135°", "Set rotation at -135°",                                               EUserInterfaceActionType::Check, FInputChord() );
    UI_BUNDLE_COMMAND( SetRotationMinus90, ViewportShortcuts, "Set Rotation -90°", "Set rotation at -90°",                                                  EUserInterfaceActionType::Check, FInputChord() );
    UI_BUNDLE_COMMAND( SetRotationMinus45, ViewportShortcuts, "Set Rotation -45°", "Set rotation at -45°",                                                  EUserInterfaceActionType::Check, FInputChord() );
    UI_BUNDLE_COMMAND( SetRotation0, ViewportShortcuts, "Set Rotation 0°", "Set rotation at 0°",                                                            EUserInterfaceActionType::Check, FInputChord() );
    UI_BUNDLE_COMMAND( SetRotation45, ViewportShortcuts, "Set Rotation 45°", "Set rotation at 45°",                                                         EUserInterfaceActionType::Check, FInputChord() );
    UI_BUNDLE_COMMAND( SetRotation90, ViewportShortcuts, "Set Rotation 90°", "Set rotation at 90°",                                                         EUserInterfaceActionType::Check, FInputChord() );
    UI_BUNDLE_COMMAND( SetRotation135, ViewportShortcuts, "Set Rotation 135°", "Set rotation at 135°",                                                      EUserInterfaceActionType::Check, FInputChord() );
    UI_BUNDLE_COMMAND( SetRotation180, ViewportShortcuts, "Set Rotation 180°", "Set rotation at 180°",                                                      EUserInterfaceActionType::Check, FInputChord() );

    UI_BUNDLE_COMMAND( SetZoom10Percent, ViewportShortcuts, "Set Zoom 10%", "Set zoom at 10%",                                                              EUserInterfaceActionType::Check, FInputChord() );
    UI_BUNDLE_COMMAND( SetZoom25Percent, ViewportShortcuts, "Set Zoom 25%", "Set zoom at 25%",                                                              EUserInterfaceActionType::Check, FInputChord() );
    UI_BUNDLE_COMMAND( SetZoom50Percent, ViewportShortcuts, "Set Zoom 50%", "Set zoom at 50%",                                                              EUserInterfaceActionType::Check, FInputChord() );
    UI_BUNDLE_COMMAND( SetZoom75Percent, ViewportShortcuts, "Set Zoom 75%", "Set zoom at 75%",                                                              EUserInterfaceActionType::Check, FInputChord() );
    UI_BUNDLE_COMMAND( SetZoom100Percent, ViewportShortcuts, "Set Zoom 100%", "Set zoom at 100%",                                                           EUserInterfaceActionType::Check, FInputChord( EModifierKey::Shift, EKeys::Z ) );
    UI_BUNDLE_COMMAND( SetZoom200Percent, ViewportShortcuts, "Set Zoom 200%", "Set zoom at 200%",                                                           EUserInterfaceActionType::Check, FInputChord() );
    UI_BUNDLE_COMMAND( SetZoom400Percent, ViewportShortcuts, "Set Zoom 400%", "Set zoom at 400%",                                                           EUserInterfaceActionType::Check, FInputChord() );
    UI_BUNDLE_COMMAND( SetZoom800Percent, ViewportShortcuts, "Set Zoom 800%", "Set zoom at 800%",                                                           EUserInterfaceActionType::Check, FInputChord() );

    UI_BUNDLE_COMMAND( SetZoomFitScreen, ViewportShortcuts, "Set Zoom Fit Screen", "Set zoom fit screen",                                                   EUserInterfaceActionType::ToggleButton, FInputChord( EKeys::S ) );
    UI_BUNDLE_COMMAND( ZoomInExponential, ViewportShortcuts, "Zoom In (exponential)", "Zoom in (exponential)",                                              EUserInterfaceActionType::Button, FInputChord( EKeys::Multiply ) );
    UI_BUNDLE_COMMAND( ZoomOutExponential, ViewportShortcuts, "Zoom Out (exponential)", "Zoom out (exponential)",                                           EUserInterfaceActionType::Button, FInputChord( EKeys::Divide ) );
}

#undef LOCTEXT_NAMESPACE
