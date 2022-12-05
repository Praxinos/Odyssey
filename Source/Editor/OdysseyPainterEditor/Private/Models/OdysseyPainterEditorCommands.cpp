// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Models/OdysseyPainterEditorCommands.h"
#include "OdysseyStyleSet.h"
#include "Command/OdysseyCommandMacros.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorCommands"

namespace
{
    const FName BrushBlendingModeShortcuts = "Brush Blending Mode Shortcuts";
    const FName BrushAlphaModeShortcuts = "Brush Alpha Mode Shortcuts";
    const FName ViewportShortcuts = "Viewport Shortcuts";
    const FName BrushStrokesShortcuts = "Brushes & Strokes Shortcuts";
    const FName HelpShortcuts = "Help Shortcuts";
    const FName UncategorizedYetShortcuts = "Uncategorized Yet Shortcuts";
}

FOdysseyPainterEditorCommands::FOdysseyPainterEditorCommands()
    : TCommands<FOdysseyPainterEditorCommands>( "IliadPainterEditor", NSLOCTEXT( "Contexts", "IliadPainterEditor", "Iliad Painter Editor" ), NAME_None, FOdysseyStyle::GetStyleSetName() )
{
    AddBundle(BrushBlendingModeShortcuts, LOCTEXT("BrushBlendingModeShortcutsCategory", "Brush Blending Mode Shortcuts"));
    AddBundle(BrushAlphaModeShortcuts, LOCTEXT("BrushAlphaModeShortcutsCategory", "Brush Alpha Mode Shortcuts"));
    AddBundle(ViewportShortcuts, LOCTEXT("ViewportShortcutsCategory", "Viewport Shortcuts"));
    AddBundle(BrushStrokesShortcuts, LOCTEXT("Brushes&StrokesShortcutsShortcutsCategory", "Brushes & Strokes Shortcuts"));
    AddBundle(HelpShortcuts, LOCTEXT("HelpShortcutsCategory", "Help Shortcuts"));
    AddBundle(UncategorizedYetShortcuts, LOCTEXT("UncategorizedYetShortcutsCategory", "Uncategorized Yet Shortcuts"));
}

void
FOdysseyPainterEditorCommands::RegisterCommands()
{
// Help Shortcuts Category
    UI_CMD( AboutIliad, HelpShortcuts, "About ILIAD", "About ILIAD",                                                                             EUserInterfaceActionType::Button, FInputChord( EKeys::F2 ) );
    UI_CMD( VisitPraxinosWebsite, HelpShortcuts, "Praxinos Website...", "Praxinos Website...",                                                   EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( ManualAndReleaseNotes, HelpShortcuts, "Manual and Release Notes ...", "Manual and Release Notes ...",                                EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( Discord, HelpShortcuts, "Talk with the developpers ...", "Talk with the developpers ...",                                            EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SwitchTabletAPI, HelpShortcuts, "Change Tablet API", "Switch between Ink and Wintab on Windows, use NSevent on Mac",                 EUserInterfaceActionType::Button, FInputChord( EKeys::F8 ) );

// Uncategorized Yet Shortcuts
    UI_CMD( FillCurrentLayer, UncategorizedYetShortcuts, "Fill Current Layer", "Fill current layer",                                             EUserInterfaceActionType::Button, FInputChord( EKeys::F ) );
    UI_CMD( ClearCurrentLayer, UncategorizedYetShortcuts, "Clear Current Layer", "Clear current layer",                                          EUserInterfaceActionType::Button, FInputChord( EKeys::Delete ) );

// Viewport Shortcuts Category
    UI_CMD( PanViewport, ViewportShortcuts,  "Pan Viewport", "Hold the key to pan the viewport",                                                 EUserInterfaceActionType::None, FInputChord(EKeys::P));
    UI_CMD( RotateViewport, ViewportShortcuts,  "Rotate Viewport", "Hold the key to Rotate the viewport",                                        EUserInterfaceActionType::None, FInputChord(EKeys::R));
    UI_CMD( ZoomViewport, ViewportShortcuts,  "Zoom Viewport", "Hold the key to Zoom the viewport",                                              EUserInterfaceActionType::None, FInputChord(EKeys::Z));
    UI_CMD( ResetViewportRotation, ViewportShortcuts, "Reset Viewport Rotation", "Reset viewport rotation",                                      EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::R ) );
    UI_CMD( ResetViewportPosition, ViewportShortcuts, "Reset Viewport Position", "Reset viewport position",                                      EUserInterfaceActionType::Button, FInputChord( EKeys::Escape ) );
    UI_CMD( RotateViewportLeft, ViewportShortcuts, "Rotate Viewport Left", "Rotate viewport left",                                               EUserInterfaceActionType::Button, FInputChord( EModifierKey::Shift, EKeys::L ) );
    UI_CMD( RotateViewportRight, ViewportShortcuts, "Rotate Viewport Right", "Rotate viewport right",                                            EUserInterfaceActionType::Button, FInputChord( EModifierKey::Shift, EKeys::R ) );
    UI_CMD( SetZoom10Percent, ViewportShortcuts, "Set Zoom 10 Percent", "Set zoom at 10 percent",                                                EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetZoom20Percent, ViewportShortcuts, "Set Zoom 20 Percent", "Set zoom at 20 percent",                                                EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetZoom30Percent, ViewportShortcuts, "Set Zoom 30 Percent", "Set zoom at 30 percent",                                                EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetZoom40Percent, ViewportShortcuts, "Set Zoom 40 Percent", "Set zoom at 40 percent",                                                EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetZoom50Percent, ViewportShortcuts, "Set Zoom 50 Percent", "Set zoom at 50 percent",                                                EUserInterfaceActionType::Button, FInputChord( EKeys::H ) );
    UI_CMD( SetZoom60Percent, ViewportShortcuts, "Set Zoom 60 Percent", "Set zoom at 60 percent",                                                EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetZoom70Percent, ViewportShortcuts, "Set Zoom 70 Percent", "Set zoom at 70 percent",                                                EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetZoom80Percent, ViewportShortcuts, "Set Zoom 80 Percent", "Set zoom at 80 percent",                                                EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetZoom90Percent, ViewportShortcuts, "Set Zoom 90 Percent", "Set zoom at 90 percent",                                                EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetZoom100Percent, ViewportShortcuts, "Set Zoom 100 Percent", "Set zoom at 100 percent",                                             EUserInterfaceActionType::Button, FInputChord( EModifierKey::Shift, EKeys::Z ) );
    UI_CMD( SetZoomFitScreen, ViewportShortcuts, "Set Zoom Fit Screen", "Set zoom fit screen",                                                   EUserInterfaceActionType::Button, FInputChord( EKeys::S ) );
    UI_CMD( ZoomInExponential, ViewportShortcuts, "Zoom In (exponential)", "Zoom in (exponential)",                                              EUserInterfaceActionType::Button, FInputChord( EKeys::Multiply ) );
    UI_CMD( ZoomOutExponential, ViewportShortcuts, "Zoom Out (exponential)", "Zoom out (exponential)",                                           EUserInterfaceActionType::Button, FInputChord( EKeys::Divide ) );
                                                                                                                                                
// Brushes Shortcuts Category                                                                                                                     
    UI_CMD( RefreshBrush, BrushStrokesShortcuts, "Refresh Brush", "Refresh Brush",                                                               EUserInterfaceActionType::Button, FInputChord( EKeys::F5 ) );                                                                                                                                                
    UI_CMD( IncreaseBrushSize, BrushStrokesShortcuts, "Increase Brush Size", "Increase Brush Size by 1",                                         EUserInterfaceActionType::Button, FInputChord( EKeys::Add ) );
    UI_CMD( DecreaseBrushSize, BrushStrokesShortcuts, "Decrease Brush Size", "Decrease Brush Size by 1",                                         EUserInterfaceActionType::Button, FInputChord( EKeys::Subtract ) );
    UI_CMD( ToggleEraserButton, BrushStrokesShortcuts, "Toggle current Brush to Eraser", "Toggle current Brush to Eraser",                       EUserInterfaceActionType::Button, FInputChord( EKeys::E ) );
    UI_CMD( GetBrushPack, BrushStrokesShortcuts, "Get more brushes ...", "Get more brushes ...",                                                                       EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( PickColorInViewport, BrushStrokesShortcuts, "Color Picker", "Hold the key to Pick a color in the viewport",                          EUserInterfaceActionType::None, FInputChord(EKeys::I));
#if PLATFORM_MAC                                                                                                                                
    UI_CMD( Undo, BrushStrokesShortcuts, "Undo Brush Stroke", "Undo Brush Stroke in Iliad",                                                      EUserInterfaceActionType::Button, FInputChord( EModifierKey::Command, EKeys::Z ) );
    UI_CMD( Redo, BrushStrokesShortcuts, "Redo Brush Stroke", "Redo Brush Stroke in Iliad",                                                      EUserInterfaceActionType::Button, FInputChord( EModifierKey::Command, EKeys::Y ) );
#else                                                                                                                                           
    UI_CMD( Undo, BrushStrokesShortcuts, "Undo Brush Stroke", "Undo Brush Stroke in Iliad",                                                      EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::Z ) );
    UI_CMD( Redo, BrushStrokesShortcuts, "Redo Brush Stroke", "Redo Brush Stroke in Iliad",                                                      EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::Y ) );
#endif                                                                                                                                          
    UI_CMD( ClearUndo, BrushStrokesShortcuts, "Clear Brush Stroke History", "Clears the Brush Stroke History in Iliad",                          EUserInterfaceActionType::Button, FInputChord() );
                                                                                                                                                
                                                                                                                                                
// Brush Alpha Mode Shortcuts Category                                                                                                          
    UI_CMD( SetAlphaModeNormal, BrushAlphaModeShortcuts, "Normal", "Set Alpha Mode to Normal",                                                   EUserInterfaceActionType::Button, FInputChord( EKeys::B ) );
    UI_CMD( SetAlphaModeErase, BrushAlphaModeShortcuts, "Erase", "Set Brush Alpha Mode to Erase",                                                EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetAlphaModeTop, BrushAlphaModeShortcuts, "Top", "Set Brush Alpha Mode to Top",                                                      EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetAlphaModeBack, BrushAlphaModeShortcuts, "Back", "Set Brush Alpha Mode to Back",                                                   EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetAlphaModeSub, BrushAlphaModeShortcuts, "Sub", "Set Brush Alpha Mode to Sub",                                                      EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetAlphaModeAdd, BrushAlphaModeShortcuts, "Add", "Set Brush Alpha Mode to Add",                                                      EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetAlphaModeMul, BrushAlphaModeShortcuts, "Mul", "Set Brush Alpha Mode to Mul",                                                      EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetAlphaModeMin, BrushAlphaModeShortcuts, "Min", "Set Brush Alpha Mode to Min",                                                      EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetAlphaModeMax, BrushAlphaModeShortcuts, "Max", "Set Brush Alpha Mode to Max",                                                      EUserInterfaceActionType::Button, FInputChord() );

// Brush Blending Mode Shortcuts Category
    UI_CMD( SetBlendModeNormal, BrushBlendingModeShortcuts, "Normal", "Set Brush Blend Mode to Normal",                                          EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeTop, BrushBlendingModeShortcuts, "Top", "Set Brush Blend Mode to Top",                                                   EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeBack, BrushBlendingModeShortcuts, "Back", "Set Brush Blend Mode to Back",                                                EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeBehind, BrushBlendingModeShortcuts, "Behind", "Set Brush Blend Mode to Behind",                                          EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeDissolve, BrushBlendingModeShortcuts, "Dissolve", "Set Brush Blend Mode to Dissolve",                                    EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeBayerDither8x8, BrushBlendingModeShortcuts, "BayerDither8x8", "Set Brush Blend Mode to BayerDither8x8",                  EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeDarken, BrushBlendingModeShortcuts, "Darken", "Set Brush Blend Mode to Darken",                                          EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeMultiply, BrushBlendingModeShortcuts, "Multiply", "Set Brush Blend Mode to Multiply",                                    EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeColorBurn, BrushBlendingModeShortcuts, "Colorburn", "Set Brush Blend Mode to Colorburn",                                 EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeLinearBurn, BrushBlendingModeShortcuts, "LinearBurn", "Set Brush Blend Mode to LinearBurn",                              EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeDarkerColor, BrushBlendingModeShortcuts, "DarkerColor", "Set Brush Blend Mode to DarkerColor",                           EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeLighten, BrushBlendingModeShortcuts, "Lighten", "Set Brush Blend Mode to Lighten",                                       EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeScreen, BrushBlendingModeShortcuts, "Screen", "Set Brush Blend Mode to Screen",                                          EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeColorDodge, BrushBlendingModeShortcuts, "ColorDodge", "Set Brush Blend Mode to ColorDodge",                              EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeLinearDodge, BrushBlendingModeShortcuts, "LinearDodge", "Set Brush Blend Mode to LinearDodge",                           EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeLighterColor, BrushBlendingModeShortcuts, "LightColor", "Set Brush Blend Mode to LightColor",                            EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeOverlay, BrushBlendingModeShortcuts, "Overlay", "Set Brush Blend Mode to Overlay",                                       EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeSoftLight, BrushBlendingModeShortcuts, "SoftLight", "Set Brush Blend Mode to SoftLight",                                 EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeHardLight, BrushBlendingModeShortcuts, "HardLight", "Set Brush Blend Mode to HardLight",                                 EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeVividLight, BrushBlendingModeShortcuts, "VividLight", "Set Brush Blend Mode to VividLight",                              EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeLinearLight, BrushBlendingModeShortcuts, "LinearLight", "Set Brush Blend Mode to LinearLight",                           EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModePinLight, BrushBlendingModeShortcuts, "PinLight", "Set Brush Blend Mode to PinLight",                                    EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeHardMix, BrushBlendingModeShortcuts, "Hardmix", "Set Brush Blend Mode to Hardmix",                                       EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModePhoenix, BrushBlendingModeShortcuts, "Phoenix", "Set Brush Blend Mode to Phoenix",                                       EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeReflect, BrushBlendingModeShortcuts, "Reflect", "Set Brush Blend Mode to Reflect",                                       EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeGlow, BrushBlendingModeShortcuts, "Glow", "Set Brush Blend Mode to Glow",                                                EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeDifference, BrushBlendingModeShortcuts, "Difference", "Set Brush Blend Mode to Difference",                              EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeExclusion, BrushBlendingModeShortcuts, "Exclusion", "Set Brush Blend Mode to Exclusion",                                 EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeAdd, BrushBlendingModeShortcuts, "Add", "Set Brush Blend Mode to Add",                                                   EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeSubstract, BrushBlendingModeShortcuts, "Substract", "Set Brush Blend Mode to Substract",                                 EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeDivide, BrushBlendingModeShortcuts, "Divide", "Set Brush Blend Mode to Divide",                                          EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeAverage, BrushBlendingModeShortcuts, "Average", "Set Brush Blend Mode to Average",                                       EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeNegation, BrushBlendingModeShortcuts, "Negation", "Set Brush Blend Mode to Negation",                                    EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeHue, BrushBlendingModeShortcuts, "Hue", "Set Brush Blend Mode to Hue",                                                   EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeSaturation, BrushBlendingModeShortcuts, "Saturation", "Set Brush Blend Mode to Saturation",                              EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeColor, BrushBlendingModeShortcuts, "Color", "Set Brush Blend Mode to Color",                                             EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeLuminosity, BrushBlendingModeShortcuts, "Lunminosity", "Set Brush Blend Mode to Lunminosity",                            EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModePartialDerivative, BrushBlendingModeShortcuts, "PartialDerivative", "Set Brush Blend Mode to PartialDerivative",         EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeWhiteOut, BrushBlendingModeShortcuts, "Whiteout", "Set Brush Blend Mode to Whiteout",                                    EUserInterfaceActionType::Button, FInputChord() );
    UI_CMD( SetBlendModeAngleCorrected, BrushBlendingModeShortcuts, "AngleCorrected", "Set Brush Blend Mode to AngleCorrected",                  EUserInterfaceActionType::Button, FInputChord() );
}

#undef LOCTEXT_NAMESPACE
