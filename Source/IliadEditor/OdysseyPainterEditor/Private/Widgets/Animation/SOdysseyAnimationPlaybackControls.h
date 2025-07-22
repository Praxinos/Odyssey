// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SButton.h"

class UOdysseyAnimation;
class UOdysseyAnimationPlayer;

//////////////////////////////////////////////////////////////////////////
// SOdysseyAnimationPlaybackControls
// Diplays all controls for a playback (play / pause / next / end / previous / start)
class SOdysseyAnimationPlaybackControls : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationPlaybackControls)
        {}
        SLATE_ARGUMENT(UOdysseyAnimation*, Animation)
        SLATE_ARGUMENT(UOdysseyAnimationPlayer*, Player)
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
    UOdysseyAnimation* mAnimation;
    UOdysseyAnimationPlayer* mPlayer;
};
