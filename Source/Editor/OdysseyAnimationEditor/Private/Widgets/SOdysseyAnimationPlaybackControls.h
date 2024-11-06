// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SButton.h"

//////////////////////////////////////////////////////////////////////////
// SOdysseyAnimationPlaybackControls
// Diplays all controls for a playback (play / pause / next / end / previous / start)
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationPlaybackControls : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationPlaybackControls)
        {}
        SLATE_ARGUMENT(UOdysseyAnimation*, Animation)
        SLATE_ARGUMENT(UOdysseyAnimationPlayer*, Player)
        SLATE_ATTRIBUTE(float, PlaybackFramesPerSecond)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    bool IsPlayingForward() const;
    bool IsPlayingBackward() const;

    EVisibility GetPlayButtonVisibility() const;
    EVisibility GetPlayBackwardButtonVisibility() const;
    EVisibility GetPauseButtonVisibility() const;
    EVisibility GetPauseBackwardButtonVisibility() const;
    EVisibility GetLoopingButtonVisibility() const;
    EVisibility GetNotLoopingButtonVisibility() const;

private:
    FReply OnPlayClicked();
    FReply OnPlayBackwardClicked();
    FReply OnPauseClicked();
    FReply OnStopClicked();
    FReply OnBeginningClicked();
    FReply OnEndClicked();
    FReply OnPreviousClicked();
    FReply OnNextClicked();
    FReply OnPreviousKeyClicked();
    FReply OnNextKeyClicked();
    FReply OnLoopClicked();

private:
    TAttribute<float> mPlaybackFramesPerSecond;
    UOdysseyAnimation* mAnimation;
    UOdysseyAnimationPlayer* mPlayer;
};
