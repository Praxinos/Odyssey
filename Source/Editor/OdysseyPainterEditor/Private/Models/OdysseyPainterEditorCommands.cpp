// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Models/OdysseyPainterEditorCommands.h"
#include "OdysseyStyleSet.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorCommands"

FOdysseyPainterEditorCommands::FOdysseyPainterEditorCommands()
    : TCommands<FOdysseyPainterEditorCommands>( "IliadPainterEditor", NSLOCTEXT( "Contexts", "IliadPainterEditor", "Iliad Painter Editor" ), NAME_None, FOdysseyStyle::GetStyleSetName() )
{
}

void
FOdysseyPainterEditorCommands::RegisterCommands()
{
    UI_COMMAND( AboutIliad, "About ILIAD", "About ILIAD",                                                                                       EUserInterfaceActionType::Button, FInputChord( EKeys::F2 ) );
    UI_COMMAND( VisitPraxinosWebsite, "Praxinos Website...", "Praxinos Website...",                                                             EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( ManualAndReleaseNotes, "Manual and Release Notes ...", "Manual and Release Notes ...",                                          EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( GetBrushPack, "Get more brushes ...", "Get more brushes ...",                                                                   EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( Discord, "Talk with the developpers ...", "Talk with the developpers ...",                                                      EUserInterfaceActionType::Button, FInputChord() );
                                                                                                                                                
    UI_COMMAND( PanViewport, "Pan Viewport", "Hold the key to pan the viewport",                                                                EUserInterfaceActionType::None, FInputChord(EKeys::P));
    UI_COMMAND( RotateViewport, "Rotate Viewport", "Hold the key to Rotate the viewport",                                                       EUserInterfaceActionType::None, FInputChord(EKeys::R));
    UI_COMMAND( ZoomViewport, "Zoom Viewport", "Hold the key to Zoom the viewport",                                                             EUserInterfaceActionType::None, FInputChord(EKeys::Z));
    UI_COMMAND( PickColorInViewport, "Pick Color in Viewport", "Hold the key to Pick a color in the viewport",                                  EUserInterfaceActionType::None, FInputChord(EKeys::I));
                                                                                                                                                
    UI_COMMAND( ResetViewportRotation, "Reset Viewport Rotation", "Reset viewport rotation",                                                    EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::R ) );
    UI_COMMAND( ResetViewportPosition, "Reset Viewport Position", "Reset viewport position",                                                    EUserInterfaceActionType::Button, FInputChord( EKeys::Escape ) );
    UI_COMMAND( RotateViewportLeft, "Rotate Viewport Left", "Rotate viewport left",                                                             EUserInterfaceActionType::Button, FInputChord( EModifierKey::Shift, EKeys::L ) );
    UI_COMMAND( RotateViewportRight, "Rotate Viewport Right", "Rotate viewport right",                                                          EUserInterfaceActionType::Button, FInputChord( EModifierKey::Shift, EKeys::R ) );
    UI_COMMAND( SetZoom10Percent, "Set Zoom 10 Percent", "Set zoom at 10 percent",                                                              EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetZoom20Percent, "Set Zoom 20 Percent", "Set zoom at 20 percent",                                                              EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetZoom30Percent, "Set Zoom 30 Percent", "Set zoom at 30 percent",                                                              EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetZoom40Percent, "Set Zoom 40 Percent", "Set zoom at 40 percent",                                                              EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetZoom50Percent, "Set Zoom 50 Percent", "Set zoom at 50 percent",                                                              EUserInterfaceActionType::Button, FInputChord( EKeys::H ) );
    UI_COMMAND( SetZoom60Percent, "Set Zoom 60 Percent", "Set zoom at 60 percent",                                                              EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetZoom70Percent, "Set Zoom 70 Percent", "Set zoom at 70 percent",                                                              EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetZoom80Percent, "Set Zoom 80 Percent", "Set zoom at 80 percent",                                                              EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetZoom90Percent, "Set Zoom 90 Percent", "Set zoom at 90 percent",                                                              EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetZoom100Percent, "Set Zoom 100 Percent", "Set zoom at 100 percent",                                                           EUserInterfaceActionType::Button, FInputChord( EModifierKey::Shift, EKeys::Z ) );
    UI_COMMAND( SetZoomFitScreen, "Set Zoom Fit Screen", "Set zoom fit screen",                                                                 EUserInterfaceActionType::Button, FInputChord( EKeys::S ) );
    UI_COMMAND( ZoomInExponential, "Zoom In (exponential)", "Zoom in (exponential)",                                                            EUserInterfaceActionType::Button, FInputChord( EKeys::Add ) );
    UI_COMMAND( ZoomOutExponential, "Zoom Out (exponential)", "Zoom out (exponential)",                                                         EUserInterfaceActionType::Button, FInputChord( EKeys::Subtract ) );
                                                                                                                                                
    UI_COMMAND( RefreshBrush, "Refresh Brush", "Refresh Brush",                                                                                 EUserInterfaceActionType::Button, FInputChord( EKeys::F5 ) );
                                                                                                                                                
    UI_COMMAND( IncreaseBrushSize, "Increase Brush Size", "Increase Brush Size by 1",                                                           EUserInterfaceActionType::Button, FInputChord( EKeys::Add ) );
    UI_COMMAND( DecreaseBrushSize, "Decrease Brush Size", "Decrease Brush Size by 1",                                                           EUserInterfaceActionType::Button, FInputChord( EKeys::Subtract ) );
    UI_COMMAND( SetAlphaModeNormal, "Set Brush Alpha Mode to Normal", "Set Alpha Mode to Normal",                                               EUserInterfaceActionType::Button, FInputChord( EKeys::B ) );
    UI_COMMAND( SetAlphaModeErase, "Set Brush Alpha Mode to Erase", "Set Brush Alpha Mode to Erase",                                            EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetAlphaModeTop, "Set Brush Alpha Mode to Top", "Set Brush Alpha Mode to Top",                                                  EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetAlphaModeBack, "Set Brush Alpha Mode to Back", "Set Brush Alpha Mode to Back",                                               EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetAlphaModeSub, "Set Brush Alpha Mode to Sub", "Set Brush Alpha Mode to Sub",                                                  EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetAlphaModeAdd, "Set Brush Alpha Mode to Add", "Set Brush Alpha Mode to Add",                                                  EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetAlphaModeMul, "Set Brush Alpha Mode to Mul", "Set Brush Alpha Mode to Mul",                                                  EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetAlphaModeMin, "Set Brush Alpha Mode to Min", "Set Brush Alpha Mode to Min",                                                  EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetAlphaModeMax, "Set Brush Alpha Mode to Max", "Set Brush Alpha Mode to Max",                                                  EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( ToggleEraserButton, "Toggle current Brush to Eraser", "Toggle current Brush to Eraser",                                         EUserInterfaceActionType::Button, FInputChord( EKeys::E ) );
                                                                                                                                                
    UI_COMMAND( SetBlendModeNormal, "Set Brush Blend Mode to Normal", "Set Brush Blend Mode to Normal",                                         EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeTop, "Set Brush Blend Mode to Top", "Set Brush Blend Mode to Top",                                                  EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeBack, "Set Brush Blend Mode to Back", "Set Brush Blend Mode to Back",                                               EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeBehind, "Set Brush Blend Mode to Behind", "Set Brush Blend Mode to Behind",                                         EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeDissolve, "Set Brush Blend Mode to Dissolve", "Set Brush Blend Mode to Dissolve",                                   EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeBayerDither8x8, "Set Brush Blend Mode to BayerDither8x8", "Set Brush Blend Mode to BayerDither8x8",                 EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeDarken, "Set Brush Blend Mode to Darken", "Set Brush Blend Mode to Darken",                                         EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeMultiply, "Set Brush Blend Mode to Multiply", "Set Brush Blend Mode to Multiply",                                   EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeColorBurn, "Set Brush Blend Mode to Colorburn", "Set Brush Blend Mode to Colorburn",                                EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeLinearBurn, "Set Brush Blend Mode to LinearBurn", "Set Brush Blend Mode to LinearBurn",                             EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeDarkerColor, "Set Brush Blend Mode to DarkerColor", "Set Brush Blend Mode to DarkerColor",                          EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeLighten, "Set Brush Blend Mode to Lighten", "Set Brush Blend Mode to Lighten",                                      EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeScreen, "Set Brush Blend Mode to Screen", "Set Brush Blend Mode to Screen",                                         EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeColorDodge, "Set Brush Blend Mode to ColorDodge", "Set Brush Blend Mode to ColorDodge",                             EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeLinearDodge, "Set Brush Blend Mode to LinearDodge", "Set Brush Blend Mode to LinearDodge",                          EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeLighterColor, "Set Brush Blend Mode to LightColor", "Set Brush Blend Mode to LightColor",                           EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeOverlay, "Set Brush Blend Mode to Overlay", "Set Brush Blend Mode to Overlay",                                      EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeSoftLight, "Set Brush Blend Mode to SoftLight", "Set Brush Blend Mode to SoftLight",                                EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeHardLight, "Set Brush Blend Mode to HardLight", "Set Brush Blend Mode to HardLight",                                EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeVividLight, "Set Brush Blend Mode to VividLight", "Set Brush Blend Mode to VividLight",                             EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeLinearLight, "Set Brush Blend Mode to LinearLight", "Set Brush Blend Mode to LinearLight",                          EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModePinLight, "Set Brush Blend Mode to PinLight", "Set Brush Blend Mode to PinLight",                                   EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeHardMix, "Set Brush Blend Mode to Hardmix", "Set Brush Blend Mode to Hardmix",                                      EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModePhoenix, "Set Brush Blend Mode to Phoenix", "Set Brush Blend Mode to Phoenix",                                      EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeReflect, "Set Brush Blend Mode to Reflect", "Set Brush Blend Mode to Reflect",                                      EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeGlow, "Set Brush Blend Mode to Glow", "Set Brush Blend Mode to Glow",                                               EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeDifference, "Set Brush Blend Mode to Difference", "Set Brush Blend Mode to Difference",                             EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeExclusion, "Set Brush Blend Mode to Exclusion", "Set Brush Blend Mode to Exclusion",                                EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeAdd, "Set Brush Blend Mode to Add", "Set Brush Blend Mode to Add",                                                  EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeSubstract, "Set Brush Blend Mode to Substract", "Set Brush Blend Mode to Substract",                                EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeDivide, "Set Brush Blend Mode to Divide", "Set Brush Blend Mode to Divide",                                         EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeAverage, "Set Brush Blend Mode to Average", "Set Brush Blend Mode to Average",                                      EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeNegation, "Set Brush Blend Mode to Negation", "Set Brush Blend Mode to Negation",                                   EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeHue, "Set Brush Blend Mode to Hue", "Set Brush Blend Mode to Hue",                                                  EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeSaturation, "Set Brush Blend Mode to Saturation", "Set Brush Blend Mode to Saturation",                             EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeColor, "Set Brush Blend Mode to Color", "Set Brush Blend Mode to Color",                                            EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeLuminosity, "Set Brush Blend Mode to Lunminosity", "Set Brush Blend Mode to Lunminosity",                           EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModePartialDerivative, "Set Brush Blend Mode to PartialDerivative", "Set Brush Blend Mode to PartialDerivative",        EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeWhiteOut, "Set Brush Blend Mode to Whiteout", "Set Brush Blend Mode to Whiteout",                                   EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetBlendModeAngleCorrected, "Set Brush Blend Mode to AngleCorrected", "Set Brush Blend Mode to AngleCorrected",                 EUserInterfaceActionType::Button, FInputChord() );
                                                                                                                                                
    UI_COMMAND( SwitchTabletAPI, "Switch Tablet API", "Switch tablet API",                                                                      EUserInterfaceActionType::Button, FInputChord( EKeys::F8 ) );
                                                                                                                                                
    UI_COMMAND( FillCurrentLayer, "Fill Current Layer", "Fill current layer",                                                                   EUserInterfaceActionType::Button, FInputChord( EKeys::F ) );
    UI_COMMAND( ClearCurrentLayer, "Clear Current Layer", "Clear current layer",                                                                EUserInterfaceActionType::Button, FInputChord( EKeys::Delete ) );
                                                                                                                                                
#if PLATFORM_MAC                                                                                                                                
    UI_COMMAND( Undo, "Undo Iliad", "Undo stroke in Iliad",                                                                                     EUserInterfaceActionType::Button, FInputChord( EModifierKey::Command, EKeys::Z ) );
    UI_COMMAND( Redo, "Redo Iliad", "Redo stroke in Iliad",                                                                                     EUserInterfaceActionType::Button, FInputChord( EModifierKey::Command, EKeys::Y ) );
#else                                                                                                                                           
    UI_COMMAND( Undo, "Undo Iliad", "Undo stroke in Iliad",                                                                                     EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::Z ) );
    UI_COMMAND( Redo, "Redo Iliad", "Redo stroke in Iliad",                                                                                     EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::Y ) );
#endif                                                                                                                                          
    UI_COMMAND( ClearUndo, "Clear Undo Iliad", "Clears the Undo Stack in Iliad",                                                                EUserInterfaceActionType::Button, FInputChord() );
    
}

#undef LOCTEXT_NAMESPACE
