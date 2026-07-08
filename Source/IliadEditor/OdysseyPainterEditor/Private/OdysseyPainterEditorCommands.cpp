// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorCommands.h"
#include "OdysseyStyle.h"
#include "Command/OdysseyCommandMacros.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

namespace
{
    const FName BrushBlendingModeShortcuts = "Brush Blending Mode Shortcuts";
    const FName BrushAlphaModeShortcuts = "Brush Alpha Mode Shortcuts";
    const FName BrushStrokesShortcuts = "Brushes & Strokes Shortcuts";
    const FName HelpShortcuts = "Help Shortcuts";
    const FName UncategorizedYetShortcuts = "Uncategorized Yet Shortcuts";
    const FName ToolsShortcuts = "Tools Shortcuts";
    const FName LayerStackShortcuts = "LayerStack Shortcuts";
    const FName ImportExportShortcuts = "Import & Export Shortcuts";
}

FOdysseyPainterEditorCommands::FOdysseyPainterEditorCommands()
    : TCommands<FOdysseyPainterEditorCommands>( "OdysseyPainterEditor", LOCTEXT( "editor-commands.name", "Odyssey Painter Editor" ), NAME_None, FOdysseyStyle::GetStyleSetName() )
{
    AddBundle(BrushBlendingModeShortcuts, LOCTEXT("editor-commands.category.brush-blending-mode-shortcuts", "Brush Blending Mode Shortcuts"));
    AddBundle(BrushAlphaModeShortcuts, LOCTEXT("editor-commands.category.brush-alpha-mode-shortcuts", "Brush Alpha Mode Shortcuts"));
    AddBundle(BrushStrokesShortcuts, LOCTEXT("editor-commands.category.brushes-ans-strokes-shortcuts", "Brushes & Strokes Shortcuts"));
    AddBundle(HelpShortcuts, LOCTEXT("editor-commands.category.help-shortcuts", "Help Shortcuts"));
    AddBundle(ToolsShortcuts, LOCTEXT("editor-commands.category.tools-shortcuts", "Tools Shortcuts"));
    AddBundle(UncategorizedYetShortcuts, LOCTEXT("editor-commands.category.uncategorized-yet-shortcuts", "Uncategorized Yet Shortcuts"));
    AddBundle(LayerStackShortcuts, LOCTEXT("editor-commands.category.layerstack-shortcuts-category", "LayerStack Shortcuts"));
    AddBundle(ImportExportShortcuts, LOCTEXT("editor-commands.category.import-export-shortcuts", "Import & Export Shortcuts"));
}

void
FOdysseyPainterEditorCommands::RegisterCommands()
{
// Help Shortcuts Category
    UI_BUNDLE_COMMAND( AboutOdyssey, HelpShortcuts, "About Odyssey", "About Odyssey",                                                                       EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( VisitPraxinosWebsite, HelpShortcuts, "Praxinos Website...", "Praxinos Website...",                                                   EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( ManualAndReleaseNotes, HelpShortcuts, "Manual and Release Notes ...", "Manual and Release Notes ...",                                EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( Discord, HelpShortcuts, "Talk with the developpers ...", "Talk with the developpers ...",                                            EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SwitchTabletAPI, HelpShortcuts, "Change Tablet API", "Switch between Ink and Wintab on Windows, use NSevent on Mac",                 EUserInterfaceActionType::Button, FInputChord() );

// Brushes Shortcuts Category
    UI_BUNDLE_COMMAND( RefreshBrush, BrushStrokesShortcuts, "Refresh Brush", "Refresh Brush",                                                               EUserInterfaceActionType::Button, FInputChord( EKeys::F5 ) );
    UI_BUNDLE_COMMAND( IncreaseBrushSize, BrushStrokesShortcuts, "Increase Brush Size", "Increase Brush Size by 1",                                         EUserInterfaceActionType::Button, FInputChord( EKeys::Add ) );
    UI_BUNDLE_COMMAND( DecreaseBrushSize, BrushStrokesShortcuts, "Decrease Brush Size", "Decrease Brush Size by 1",                                         EUserInterfaceActionType::Button, FInputChord( EKeys::Subtract ) );
    UI_BUNDLE_COMMAND( ToggleEraserButton, BrushStrokesShortcuts, "Toggle current Brush to Eraser", "Toggle current Brush to Eraser",                       EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( GetBrushPack, BrushStrokesShortcuts, "Get more brushes ...", "Get more brushes ...",                                                 EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( PickColorInViewport, BrushStrokesShortcuts, "Color Picker", "Hold the key to Pick a color in the viewport",                          EUserInterfaceActionType::None, FInputChord(EKeys::I));


// Brush Alpha Mode Shortcuts Category
    UI_BUNDLE_COMMAND( SetAlphaModeNormal, BrushAlphaModeShortcuts, "Normal", "Set Alpha Mode to Normal",                                                   EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetAlphaModeErase, BrushAlphaModeShortcuts, "Erase", "Set Brush Alpha Mode to Erase",                                                EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetAlphaModeTop, BrushAlphaModeShortcuts, "Top", "Set Brush Alpha Mode to Top",                                                      EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetAlphaModeBack, BrushAlphaModeShortcuts, "Back", "Set Brush Alpha Mode to Back",                                                   EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetAlphaModeSub, BrushAlphaModeShortcuts, "Sub", "Set Brush Alpha Mode to Sub",                                                      EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetAlphaModeAdd, BrushAlphaModeShortcuts, "Add", "Set Brush Alpha Mode to Add",                                                      EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetAlphaModeMul, BrushAlphaModeShortcuts, "Mul", "Set Brush Alpha Mode to Mul",                                                      EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetAlphaModeMin, BrushAlphaModeShortcuts, "Min", "Set Brush Alpha Mode to Min",                                                      EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetAlphaModeMax, BrushAlphaModeShortcuts, "Max", "Set Brush Alpha Mode to Max",                                                      EUserInterfaceActionType::Button, FInputChord() );

// Brush Blending Mode Shortcuts Category
    UI_BUNDLE_COMMAND( SetBlendModeNormal, BrushBlendingModeShortcuts, "Normal", "Set Brush Blend Mode to Normal",                                          EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeTop, BrushBlendingModeShortcuts, "Top", "Set Brush Blend Mode to Top",                                                   EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeBack, BrushBlendingModeShortcuts, "Back", "Set Brush Blend Mode to Back",                                                EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeBehind, BrushBlendingModeShortcuts, "Behind", "Set Brush Blend Mode to Behind",                                          EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeDissolve, BrushBlendingModeShortcuts, "Dissolve", "Set Brush Blend Mode to Dissolve",                                    EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeBayerDither8x8, BrushBlendingModeShortcuts, "BayerDither8x8", "Set Brush Blend Mode to BayerDither8x8",                  EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeDarken, BrushBlendingModeShortcuts, "Darken", "Set Brush Blend Mode to Darken",                                          EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeMultiply, BrushBlendingModeShortcuts, "Multiply", "Set Brush Blend Mode to Multiply",                                    EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeColorBurn, BrushBlendingModeShortcuts, "Colorburn", "Set Brush Blend Mode to Colorburn",                                 EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeLinearBurn, BrushBlendingModeShortcuts, "LinearBurn", "Set Brush Blend Mode to LinearBurn",                              EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeDarkerColor, BrushBlendingModeShortcuts, "DarkerColor", "Set Brush Blend Mode to DarkerColor",                           EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeLighten, BrushBlendingModeShortcuts, "Lighten", "Set Brush Blend Mode to Lighten",                                       EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeScreen, BrushBlendingModeShortcuts, "Screen", "Set Brush Blend Mode to Screen",                                          EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeColorDodge, BrushBlendingModeShortcuts, "ColorDodge", "Set Brush Blend Mode to ColorDodge",                              EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeLinearDodge, BrushBlendingModeShortcuts, "LinearDodge", "Set Brush Blend Mode to LinearDodge",                           EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeLighterColor, BrushBlendingModeShortcuts, "LightColor", "Set Brush Blend Mode to LightColor",                            EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeOverlay, BrushBlendingModeShortcuts, "Overlay", "Set Brush Blend Mode to Overlay",                                       EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeSoftLight, BrushBlendingModeShortcuts, "SoftLight", "Set Brush Blend Mode to SoftLight",                                 EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeHardLight, BrushBlendingModeShortcuts, "HardLight", "Set Brush Blend Mode to HardLight",                                 EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeVividLight, BrushBlendingModeShortcuts, "VividLight", "Set Brush Blend Mode to VividLight",                              EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeLinearLight, BrushBlendingModeShortcuts, "LinearLight", "Set Brush Blend Mode to LinearLight",                           EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModePinLight, BrushBlendingModeShortcuts, "PinLight", "Set Brush Blend Mode to PinLight",                                    EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeHardMix, BrushBlendingModeShortcuts, "Hardmix", "Set Brush Blend Mode to Hardmix",                                       EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModePhoenix, BrushBlendingModeShortcuts, "Phoenix", "Set Brush Blend Mode to Phoenix",                                       EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeReflect, BrushBlendingModeShortcuts, "Reflect", "Set Brush Blend Mode to Reflect",                                       EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeGlow, BrushBlendingModeShortcuts, "Glow", "Set Brush Blend Mode to Glow",                                                EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeDifference, BrushBlendingModeShortcuts, "Difference", "Set Brush Blend Mode to Difference",                              EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeExclusion, BrushBlendingModeShortcuts, "Exclusion", "Set Brush Blend Mode to Exclusion",                                 EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeAdd, BrushBlendingModeShortcuts, "Add", "Set Brush Blend Mode to Add",                                                   EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeSubstract, BrushBlendingModeShortcuts, "Substract", "Set Brush Blend Mode to Substract",                                 EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeDivide, BrushBlendingModeShortcuts, "Divide", "Set Brush Blend Mode to Divide",                                          EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeAverage, BrushBlendingModeShortcuts, "Average", "Set Brush Blend Mode to Average",                                       EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeNegation, BrushBlendingModeShortcuts, "Negation", "Set Brush Blend Mode to Negation",                                    EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeHue, BrushBlendingModeShortcuts, "Hue", "Set Brush Blend Mode to Hue",                                                   EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeSaturation, BrushBlendingModeShortcuts, "Saturation", "Set Brush Blend Mode to Saturation",                              EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeColor, BrushBlendingModeShortcuts, "Color", "Set Brush Blend Mode to Color",                                             EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeLuminosity, BrushBlendingModeShortcuts, "Lunminosity", "Set Brush Blend Mode to Lunminosity",                            EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModePartialDerivative, BrushBlendingModeShortcuts, "PartialDerivative", "Set Brush Blend Mode to PartialDerivative",         EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeWhiteOut, BrushBlendingModeShortcuts, "Whiteout", "Set Brush Blend Mode to Whiteout",                                    EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( SetBlendModeAngleCorrected, BrushBlendingModeShortcuts, "AngleCorrected", "Set Brush Blend Mode to AngleCorrected",                  EUserInterfaceActionType::Button, FInputChord() );

    UI_BUNDLE_COMMAND( ActivateColorPickerTool, ToolsShortcuts, "Activate Color Picker Tool", "Activates the Color Picker Tool", EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ActivateDrawingTool, ToolsShortcuts, "Activate Drawing Tool", "Activates the Drawing Tool", EUserInterfaceActionType::Button, FInputChord(EKeys::B));
    UI_BUNDLE_COMMAND( ActivateSelectionTool, ToolsShortcuts, "Activate Selection Tool", "Activates the Selection Tool", EUserInterfaceActionType::Button, FInputChord(EKeys::L));
    UI_BUNDLE_COMMAND( ActivatePaintBucketTool, ToolsShortcuts, "Activate PaintBucket Tool", "Activates the Paint Bucket Tool", EUserInterfaceActionType::Button, FInputChord(EKeys::G));
    UI_BUNDLE_COMMAND( ActivateTransformTool, ToolsShortcuts, "Activate Transform Tool", "Activates the Transform Tool", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::T));
    UI_BUNDLE_COMMAND( ActivatePrimitiveDrawingTool, ToolsShortcuts, "Activate Primitive Drawing Tool", "Activates the Primitive Drawing Tool", EUserInterfaceActionType::Button, FInputChord(EKeys::U));
    UI_BUNDLE_COMMAND( ActivateEraserTool, ToolsShortcuts, "Activate Eraser Tool", "Activates the Eraser Tool", EUserInterfaceActionType::Button, FInputChord(EKeys::E));
    UI_BUNDLE_COMMAND( ActivateWarpTool, ToolsShortcuts, "Activate Warp Tool", "Activates the Warp Tool", EUserInterfaceActionType::Button, FInputChord(EKeys::W));

    UI_BUNDLE_COMMAND( ActivateTemporaryColorPickerTool, ToolsShortcuts, "Activate Color Picker Temporarily", "Activates the Color Picker Tool Temporarily", EUserInterfaceActionType::Button, FInputChord( EKeys::I ) );

    //TODO: rename this shortcut label (and tooltip) to something: "Inactivate Temporary Tool (Out of Peg,...)"
    // once more than 1 temporary tool will be available
    // By listing all temporary tools in label, the shortcut will be found when searching for a specific temporary tool
    UI_BUNDLE_COMMAND( InactivateTemporaryTool, ToolsShortcuts, "Inactivate Out of Peg Tool", "Inactivates any temporary tool like: Out of Peg Tool", EUserInterfaceActionType::Button, FInputChord(EKeys::Enter), FInputChord(EKeys::Escape));

    UI_BUNDLE_COMMAND( ActivateRasterDrawingTool, ToolsShortcuts, "Activate Raster Drawing Tool", "Activates the editor Raster Drawing tool", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( ActivateRasterEraserTool, ToolsShortcuts, "Activate Raster Eraser Tool", "Activates the editor Raster Eraser tool", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( ActivateRasterPaintBucketTool, ToolsShortcuts, "Activate Raster Paint Bucket Tool", "Activates the editor Raster Paint Bucket tool", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( ActivateRasterPrimitiveDrawingTool, ToolsShortcuts, "Activate Raster Primitive Drawing Tool", "Activates the editor Raster Primitive Drawing tool", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( ActivateRasterTransformTool, ToolsShortcuts, "Activate Raster Transform Tool", "Activates the editor Raster Transform tool", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( ActivateVectorEraserTool, ToolsShortcuts, "Activate Vector Eraser Tool", "Activates the editor Vector Eraser tool", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( ActivateVectorGridTool, ToolsShortcuts, "Activate Vector Grid Tool", "Activates the editor Vector Grid tool", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( ActivateVectorPaintBucketTool, ToolsShortcuts, "Activate Vector Paint Bucket Tool", "Activates the editor Vector Paint Bucket tool", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( ActivateVectorPathCutTool, ToolsShortcuts, "Activate Vector Path Cut Tool", "Activates the editor Vector Path Cut tool", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( ActivateVectorPathDrawingTool, ToolsShortcuts, "Activate Vector Path Drawing Tool", "Activates the editor Vector Path Drawing tool", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( ActivateVectorPathEditTool, ToolsShortcuts, "Activate Vector Path Edit Tool", "Activates the editor Vector Path Edit tool", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Shift, EKeys::D) );
    UI_BUNDLE_COMMAND( ActivateVectorPathPushTool, ToolsShortcuts, "Activate Vector Path Push Tool", "Activates the editor Vector Path Push tool", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Shift, EKeys::P) );
    UI_BUNDLE_COMMAND( ActivateVectorPathSmoothTool, ToolsShortcuts, "Activate Vector Path Smooth Tool", "Activates the editor Vector Path Smooth tool", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( ActivateVectorPathStitchTool, ToolsShortcuts, "Activate Vector Path Stitch Tool", "Activates the editor Vector Path Stitch tool", EUserInterfaceActionType::Button, FInputChord(EKeys::Equals) );
    UI_BUNDLE_COMMAND( ActivateVectorPrimitiveDrawingTool, ToolsShortcuts, "Activate Vector Primitive Drawing Tool", "Activates the editor Vector Primitive Drawing tool", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( ActivateVectorScenePanTool, ToolsShortcuts, "Activate Vector Scene Pan Tool", "Activates the editor Vector Scene Pan tool", EUserInterfaceActionType::Button, FInputChord(EKeys::H) );
    UI_BUNDLE_COMMAND( ActivateVectorSelectionTool, ToolsShortcuts, "Activate Vector Selection Tool", "Activates the editor Vector Selection tool", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( ActivateVectorTransformTool, ToolsShortcuts, "Activate Vector Transform Tool", "Activates the editor Vector Transform tool", EUserInterfaceActionType::Button, FInputChord() );

    UI_BUNDLE_COMMAND( SetToolRadius, ToolsShortcuts, "Set Tool Radius", "Set the radius of the current tool (press and hold)", EUserInterfaceActionType::Button, FInputChord(EKeys::Z) );

    // LayerStack Shortcuts Category
    UI_BUNDLE_COMMAND( CreateNewLayer, LayerStackShortcuts, "Create New Layer", "Create new image layer",                                                                                                      EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::N));
    for (int i = 0; i <= 10; i++)
    {
        FNumberFormattingOptions options;
        options.SetMinimumIntegralDigits(2);
        TSharedPtr<FUICommandInfo> commandInfo = FUICommandInfoDecl(
              this->AsShared()
            , FName( *FString::Printf( TEXT( "ChangeLayerOpacity%d" ), i * 10 ))
            , FText::Format( LOCTEXT( "commands.change-layer-opacity.label", "Change current Layer Opacity to {0}%"), FText::AsNumber(i * 10, &options) )
            , FText::Format( LOCTEXT( "commands.change-layer-opacity.tooltip", "Change current Layer Opacity to {0}%"), FText::AsNumber(i * 10, &options))
            , LayerStackShortcuts
        )
        .UserInterfaceType( EUserInterfaceActionType::Button )
        .DefaultChord( FInputChord() );

        ChangeLayerOpacity.Add(commandInfo);
    }

    // Vector Menu commands
    // All modes
    // ...
    // Object Mode
    UI_BUNDLE_COMMAND( VectorResetView, ToolsShortcuts, "Reset View", "Reset the current view", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( VectorMakePaintGroup, ToolsShortcuts, "Make Paint Group", "Make a paint group from selection", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( VectorGroup, ToolsShortcuts, "Make Group", "Make a group from selection", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( VectorUngroup, ToolsShortcuts, "Ungroup", "Ungroup selected groups", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( VectorBringForward, ToolsShortcuts, "Bring Forward", "Bring the selection forward", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( VectorSendBackward, ToolsShortcuts, "Send Backward", "Send the selection backward", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( VectorFlipHorizontal, ToolsShortcuts, "Flip Horizontal", "Flip selection horizontally", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( VectorFlipVertical, ToolsShortcuts, "Flip Vertical", "Flip selection vertically", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( VectorClearColoring, ToolsShortcuts, "Clear Coloring", "Clear Coloring", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( VectorApplyTransformations, ToolsShortcuts, "Apply Transformations", "Apply matrix transformations", EUserInterfaceActionType::Button, FInputChord() );
    // Vertex Mode
    UI_BUNDLE_COMMAND( VectorSubdivideSegments, ToolsShortcuts, "Subdivide Segments", "Subdivide segments", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( VectorAlignPointSelection, ToolsShortcuts, "Align Point Selection", "Align segment handles attached to the selected points", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( VectorUnalignPointSelection, ToolsShortcuts, "Unalign Point Selection", "Unalign segment handles attached to the selected points", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( VectorLockPointSelection, ToolsShortcuts, "Lock Point Selection", "Lock selected points", EUserInterfaceActionType::Button, FInputChord() );
    UI_BUNDLE_COMMAND( VectorUnlockPointSelection, ToolsShortcuts, "Unlock Point Selection", "Unlock selected points", EUserInterfaceActionType::Button, FInputChord() );

    // Raster Menu commands
    UI_BUNDLE_COMMAND( RasterTranformToolTogglePerspectiveMode, ToolsShortcuts, "Raster Transform Tool : Toggle Perspective Mode", "Toggle the perspective mode for the Raster Transform Tool", EUserInterfaceActionType::Button, FInputChord() );

    // Uncategorized Yet Shortcuts. Will be triggered last if no shortcut above has been.
    UI_BUNDLE_COMMAND(ClearCurrentLayer, UncategorizedYetShortcuts, "Clear Current Layer", "Clear current layer", EUserInterfaceActionType::Button, FInputChord(EKeys::BackSpace));
    UI_BUNDLE_COMMAND(SelectAll, UncategorizedYetShortcuts, "Select All", "Select All", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::A));
    UI_BUNDLE_COMMAND(ClearCurrentSelection, UncategorizedYetShortcuts, "Clear Current Selection", "Clear current selection", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::D));
    UI_BUNDLE_COMMAND(CopyCurrentSelection, UncategorizedYetShortcuts, "Copy Current Selection", "Copy current selection", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::C));
    UI_BUNDLE_COMMAND(CutCurrentSelection, UncategorizedYetShortcuts, "Cut Current Selection", "Cut current selection", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::X));
    UI_BUNDLE_COMMAND(PasteCurrentSelection, UncategorizedYetShortcuts, "Paste Current Selection", "Paste current selection", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::V));
    UI_BUNDLE_COMMAND(PasteCurrentSelectionInNewLayer, UncategorizedYetShortcuts, "Paste Current Selection In New Layer", "Paste current selection In New Layer", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control | EModifierKey::Shift, EKeys::V));
    UI_BUNDLE_COMMAND(InvertSelection, UncategorizedYetShortcuts, "Invert Current Selection", "Invert Current Selection", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::I));

}

#undef LOCTEXT_NAMESPACE
