// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#include "TimelineTools/OdysseyAnimationTimelineTools.h"
#include "TimelineTools/OdysseyAnimationTimelineTool.h"
#include "TimelineTools/OdysseyAnimationTimelineSelectionTool.h"
#include "TimelineTools/OdysseyAnimationTimelineMoveTool.h"
#include "TimelineTools/OdysseyAnimationTimelineCutTool.h"
#include "OdysseyPainterEditorAnimationCommands.h"
#include "OdysseyKeyState.h"
#include "OdysseyPainterEditorAnimationCommands.h"
#include "Framework/Application/SlateApplication.h"

FOdysseyAnimationTimelineTools&
FOdysseyAnimationTimelineTools::Get()
{
    static FOdysseyAnimationTimelineTools tools;
    return tools;
}

FOdysseyAnimationTimelineTools::FOdysseyAnimationTimelineTools()
    : mCurrentTool(EOdysseyTimelineTool::Selection)
{
}

TSharedPtr<FOdysseyAnimationTimelineTool>
FOdysseyAnimationTimelineTools::CreateTool(TSharedRef<FOdysseyPainterEditorAnimationTimelinePosition> iTimelinePosition, TSharedRef<FOdysseyLayerCellSelection> iTimelineCellSelection) const
{
    switch(mCurrentTool)
    {
        case EOdysseyTimelineTool::Selection: return MakeShared<FOdysseyAnimationTimelineSelectionTool>(iTimelinePosition, iTimelineCellSelection);
        case EOdysseyTimelineTool::Move: return MakeShared<FOdysseyAnimationTimelineMoveTool>(iTimelinePosition);
        case EOdysseyTimelineTool::Cut: return MakeShared<FOdysseyAnimationTimelineCutTool>(iTimelinePosition);
    }

    return nullptr;
}

EOdysseyTimelineTool
FOdysseyAnimationTimelineTools::GetCurrentTool() const
{

    FKey pressedKey = FOdysseyKeyState::GetLastKey();
    if (pressedKey == FKey())
        return mCurrentTool;

    FModifierKeysState modifiers = FSlateApplication::Get().GetModifierKeys();
    const FInputChord activeChord(pressedKey,
        EModifierKey::FromBools(
            modifiers.IsControlDown(),
            modifiers.IsAltDown(),
            modifiers.IsShiftDown(),
            modifiers.IsCommandDown()
        )
    );

    if (FOdysseyPainterEditorAnimationCommands::Get().HoldActivateTimelineSelectionTool->HasActiveChord(activeChord))
    {
        return EOdysseyTimelineTool::Selection;
    }
    else if (FOdysseyPainterEditorAnimationCommands::Get().HoldActivateTimelineMoveTool->HasActiveChord(activeChord))
    {
        return EOdysseyTimelineTool::Move;
    }
    else if (FOdysseyPainterEditorAnimationCommands::Get().HoldActivateTimelineCutTool->HasActiveChord(activeChord))
    {
        return EOdysseyTimelineTool::Cut;
    }

    return mCurrentTool;
}

void
FOdysseyAnimationTimelineTools::SetCurrentTool(EOdysseyTimelineTool iTool)
{
    mCurrentTool = iTool;
}
