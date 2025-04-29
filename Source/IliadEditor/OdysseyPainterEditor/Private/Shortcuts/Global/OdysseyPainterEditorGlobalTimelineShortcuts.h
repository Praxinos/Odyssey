// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorShortcuts.h"

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorGlobalTimelineShortcuts
    : public IOdysseyEditorShortcuts
{
public:
    virtual ~FOdysseyPainterEditorGlobalTimelineShortcuts() {};
    FOdysseyPainterEditorGlobalTimelineShortcuts(FOdysseyPainterEditor* iEditor);

public:
    //Common Shortcuts
    void Action_NavigateToNextFrame();
    void Action_NavigateToPreviousFrame();
    void Action_NavigateToNextCell();
    void Action_NavigateToPreviousCell();
    void Action_NavigateToAnimationFirstFrame();
    void Action_NavigateToAnimationLastFrame();
    void Action_Play();
    void Action_PlayStop();
    void Action_PlayReversed();
    void Action_Stop();
    void Action_ActivateLooping();
    void Action_InactivateLooping();
    void Action_ToggleLooping();

    void Action_Flip(int iConfigurationIndex);

public:
    //Shortcuts
    virtual void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList) override;

private:
    FOdysseyPainterEditor* mEditor;
};
