// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyPainterEditorAnimationCommands.h"
#include "Command/OdysseyCommandMacros.h"
#include "OdysseyStyle.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

namespace
{
    const FName ImportExportShortcuts = "Import & Export Shortcuts";
    const FName TimelineShortcuts = "Timeline Shortcuts";
    const FName CellsShortcuts = "Cells Shortcuts";
    const FName NavigationShortcuts = "Navigation Shortcuts";
}

FOdysseyPainterEditorAnimationCommands::FOdysseyPainterEditorAnimationCommands()
    : TCommands<FOdysseyPainterEditorAnimationCommands>( "OdysseyPainterEditorAnimationCommands", LOCTEXT( "editor-commands.name", "Odyssey Animation Editor" ), NAME_None, FOdysseyStyle::GetStyleSetName() )
{
    AddBundle(ImportExportShortcuts, LOCTEXT("editor-commands.category.import-export-shortcuts", "Import & Export Shortcuts"));
    AddBundle(TimelineShortcuts, LOCTEXT("editor-commands.category.timeline-shortcuts", "Timeline Shortcuts"));
    AddBundle(CellsShortcuts, LOCTEXT("editor-commands.category.cells-shortcuts", "Cells Shortcuts"));
    AddBundle(NavigationShortcuts, LOCTEXT("editor-commands.category.navigation-shortcuts", "Navigation Shortcuts"));
}

void
FOdysseyPainterEditorAnimationCommands::RegisterCommands()
{
// Import & Export Shortcuts Category
    UI_BUNDLE_COMMAND( ImportTextureSequence, ImportExportShortcuts, "Import Texture Sequence", "Import several textures from the content-browser in a new layer",                                               EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ImportImageSequence, ImportExportShortcuts, "Import Image Sequence", "Import several images from your operating system in a new layer",                                               EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ExportImageSequence, ImportExportShortcuts, "Export Image Sequence", "Export the animation as an image sequence on your operating system",                                               EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ExportAsFlipbook, ImportExportShortcuts, "Export As Flipbook", "Export the animation as a flipbook",                                               EUserInterfaceActionType::Button, FInputChord());

// LayerStack Shortcuts Category
    UI_BUNDLE_COMMAND( NavigateToNextFrame, NavigationShortcuts, "Navigate to next Frame", "Change the current frame to be the next frame in the animation timeline",                                                            EUserInterfaceActionType::Button, FInputChord(EKeys::Semicolon));
    UI_BUNDLE_COMMAND( NavigateToPreviousFrame, NavigationShortcuts, "Navigate to previous Frame", "Change the current frame to be the previous frame in the animation timeline",                                                EUserInterfaceActionType::Button, FInputChord(EKeys::Comma));
    UI_BUNDLE_COMMAND( NavigateToNextCell, NavigationShortcuts, "Navigate to next Cell", "Change the current frame to be on the first frame of the next cell of the current layer in the animation timeline",                    EUserInterfaceActionType::Button, FInputChord(EKeys::T));
    UI_BUNDLE_COMMAND( NavigateToPreviousCell, NavigationShortcuts, "Navigate to previous Cell", "Change the current frame to be on the first frame of the next cell of the current layer in the animation timeline",            EUserInterfaceActionType::Button, FInputChord(EKeys::R));
    UI_BUNDLE_COMMAND( NavigateToAnimationFirstFrame, NavigationShortcuts, "Navigate To Animation First Frame", "Change the current frame to be on the first frame of the animation", EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( NavigateToAnimationLastFrame, NavigationShortcuts, "Navigate To Animation Last Frame", "Change the current frame to be on the last frame of the animation", EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( Play, NavigationShortcuts, "Play", "Play the animation", EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( PlayStop, NavigationShortcuts, "Play/Stop", "Play or Stop the animation", EUserInterfaceActionType::Button, FInputChord(EKeys::SpaceBar));
    UI_BUNDLE_COMMAND( PlayReversed, NavigationShortcuts, "Play Reversed", "Play the animation in reverse", EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( Stop, NavigationShortcuts, "Stop", "Stop the animation", EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ActivateLooping, NavigationShortcuts, "Activate Looping", "Activate looping when playing the animation", EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( InactivateLooping, NavigationShortcuts, "Inactivate Looping", "Inactivate looping when playing the animation", EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ToggleLooping, NavigationShortcuts, "Toggle Looping", "Toggle looping when playing the animation", EUserInterfaceActionType::Button, FInputChord());

    UI_BUNDLE_COMMAND( ConvertToStaggerCell, CellsShortcuts, "Convert To Stagger Cell", "Converts selected cells to Stagger Cells", EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( ConvertToReferenceCells, CellsShortcuts, "Convert To Reference Cell", "Converts selected cells to the cells referenced  by them.", EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( CrossFade, CellsShortcuts, "Cross Fade Selected Cells", "Applies a cross fade on selected cells", EUserInterfaceActionType::Button, FInputChord());

    UI_BUNDLE_COMMAND( ActivateTimelineSelectionTool, TimelineShortcuts, "Activate Timeline Selection Tool", "Activates the timeline Selection Tool", EUserInterfaceActionType::Button, FInputChord(EKeys::V));
    UI_BUNDLE_COMMAND( ActivateTimelineMoveTool, TimelineShortcuts, "Activate Timeline Move Tool", "Activates the timeline Move Tool", EUserInterfaceActionType::Button, FInputChord(EKeys::A));
    UI_BUNDLE_COMMAND( ActivateTimelineCutTool, TimelineShortcuts, "Activate Timeline Cut Tool", "Activates the timeline Cut Tool", EUserInterfaceActionType::Button, FInputChord(EKeys::C));

    UI_BUNDLE_COMMAND( HoldActivateTimelineSelectionTool, TimelineShortcuts, "Activate Timeline Selection Tool (Hold)", "Hold the key to activate the timeline Selection Tool temporarily", EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( HoldActivateTimelineMoveTool, TimelineShortcuts, "Activate Timeline Move Tool (Hold)", "Hold the key to activate the timeline Move Tool temporarily", EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( HoldActivateTimelineCutTool, TimelineShortcuts, "Activate Timeline Cut Tool (Hold)", "Hold the key to activate the timeline Cut Tool temporarily", EUserInterfaceActionType::Button, FInputChord());

    UI_BUNDLE_COMMAND( BreakCell, TimelineShortcuts, "Break Cell", "Breaks Cell at Current Frame in Current Layer", EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( BreakAndClearCell, TimelineShortcuts, "Break & Clear Cell", "Breaks & Clear Cell at Current Frame in Current Layer", EUserInterfaceActionType::Button, FInputChord());
    UI_BUNDLE_COMMAND( IncreaseCellExposure, TimelineShortcuts, "Increase Cell Exposure", "Increase Current or Selected Cells Exposure by 1 frame", EUserInterfaceActionType::Button, FInputChord(EKeys::Add));
    UI_BUNDLE_COMMAND( DecreaseCellExposure, TimelineShortcuts, "Decrease Cell Exposure", "Decrease Current or Selected Cells Exposure by 1 frame", EUserInterfaceActionType::Button, FInputChord(EKeys::Subtract));
    UI_BUNDLE_COMMAND( SetCellExposure, TimelineShortcuts, "Set Cell Exposure", "Set Current or Selected Cells Exposure", EUserInterfaceActionType::Button, FInputChord());

    UI_BUNDLE_COMMAND( RemoveCellMark, TimelineShortcuts, "Remove Cell Mark", "Removes any cell mark applied on the selected cells", EUserInterfaceActionType::Button, FInputChord());

    for (int i = 0; i < 12; i++)
    {
        FNumberFormattingOptions options;
        options.SetMinimumIntegralDigits(2);
        TSharedPtr<FUICommandInfo> commandInfo = FUICommandInfoDecl(
              this->AsShared()
            , FName( *FString::Printf( TEXT( "SetCellMark-%d" ), i + 1 ))
            , FText::Format( LOCTEXT( "commands.set-cell-mark.label", "Set Cell Mark #{0}"), FText::AsNumber(i + 1, &options) )
            , FText::Format( LOCTEXT( "commands.set-cell-mark.tooltip", "Sets the Cell Mark #{0} on selected cells"), FText::AsNumber(i + 1, &options))
            , TimelineShortcuts
        )
        .UserInterfaceType( EUserInterfaceActionType::Button )
        .DefaultChord( FInputChord() );

        SetCellMark.Add(commandInfo);
    }

    UI_BUNDLE_COMMAND( PanZoomTimeline, TimelineShortcuts,  "Pan / Zoom Timeline", "Hold the key to Pan (Left Mouse Button) or Zoom (Right Mouse Button) the Timeline",                                        EUserInterfaceActionType::None, FInputChord(EKeys::M));

    for (int i = 0; i < 10; i++)
    {
        FNumberFormattingOptions options;
        options.SetMinimumIntegralDigits(2);
        TSharedPtr<FUICommandInfo> commandInfo = FUICommandInfoDecl(
              this->AsShared()
            , FName( *FString::Printf( TEXT( "Flip-%d" ), i + 1 ))
            , FText::Format( LOCTEXT( "commands.flip.label", "Flip #{0}"), FText::AsNumber(i + 1, &options) )
            , FText::Format( LOCTEXT( "commands.flip.tooltip", "Flips the timeline using the flip configuratio #{0}"), FText::AsNumber(i + 1, &options))
            , TimelineShortcuts
        )
        .UserInterfaceType( EUserInterfaceActionType::Button )
        .DefaultChord( FInputChord() );

        Flip.Add(commandInfo);
    }

    UI_BUNDLE_COMMAND( SetAnimationLeftBoundAutomatic, TimelineShortcuts, "Set Animation Left Bound To Automatic", "Set Animation Left Bound To Automatic", EUserInterfaceActionType::RadioButton, FInputChord());
    UI_BUNDLE_COMMAND( SetAnimationLeftBoundManual, TimelineShortcuts, "Set Animation Left Bound To Manual", "Set Animation Left Bound To Manual", EUserInterfaceActionType::RadioButton, FInputChord());
    UI_BUNDLE_COMMAND( SetAnimationRightBoundAutomatic, TimelineShortcuts, "Set Animation Right Bound To Automatic", "Set Animation Right Bound To Automatic", EUserInterfaceActionType::RadioButton, FInputChord());
    UI_BUNDLE_COMMAND( SetAnimationRightBoundManual, TimelineShortcuts, "Set Animation Right Bound To Manual", "Set Animation Right Bound To Manual", EUserInterfaceActionType::RadioButton, FInputChord());
}

#undef LOCTEXT_NAMESPACE
