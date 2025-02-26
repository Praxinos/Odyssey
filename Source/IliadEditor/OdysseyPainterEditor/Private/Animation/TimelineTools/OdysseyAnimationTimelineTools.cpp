// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023
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
FOdysseyAnimationTimelineTools::CreateTool(TSharedRef<FOdysseyPainterEditorAnimationTImelinePosition> iTimelinePosition, TSharedRef<FOdysseyAnimationCellSelection> iTimelineCellSelection) const
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
