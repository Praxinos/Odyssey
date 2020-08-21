// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SButton.h"

DECLARE_DELEGATE_OneParam(FOnFrameRateChanged, float);

//////////////////////////////////////////////////////////////////////////
// SOdysseyPlaybackControlss
// Diplays all controls for a playback (play / pause / next / end / previous / start / framerate)
class ODYSSEYWIDGETS_API SOdysseyPlaybackControls : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SOdysseyPlaybackControls)
		{}
		SLATE_ATTRIBUTE(bool, IsPlaying)
		SLATE_ATTRIBUTE(bool, IsPlayingBackward)
		SLATE_ATTRIBUTE(bool, IsLooping)
		SLATE_EVENT(FOnClicked, OnPlayClicked)
		SLATE_EVENT(FOnClicked, OnPlayBackwardClicked)
		SLATE_EVENT(FOnClicked, OnPauseClicked)
		SLATE_EVENT(FOnClicked, OnStopClicked)
		SLATE_EVENT(FOnClicked, OnBeginningClicked)
		SLATE_EVENT(FOnClicked, OnEndClicked)
		SLATE_EVENT(FOnClicked, OnPreviousClicked)
		SLATE_EVENT(FOnClicked, OnNextClicked)
		SLATE_EVENT(FOnClicked, OnPreviousKeyClicked)
		SLATE_EVENT(FOnClicked, OnNextKeyClicked)
		SLATE_EVENT(FOnClicked, OnLoopClicked)
		SLATE_EVENT(FOnFrameRateChanged, OnFrameRateChanged)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	EVisibility GetPlayButtonVisibility() const;
	EVisibility GetPlayBackwardButtonVisibility() const;
	EVisibility GetPauseButtonVisibility() const;
	EVisibility GetPauseBackwardButtonVisibility() const;
	EVisibility GetLoopingButtonVisibility() const;
	EVisibility GetNotLoopingButtonVisibility() const;

private:
	TAttribute<bool> mIsPlaying;
	TAttribute<bool> mIsPlayingBackward;
	TAttribute<bool> mIsLooping;

	FOnClicked mOnPlayClicked;
	FOnClicked mOnPlayBackwardClicked;
	FOnClicked mOnPauseClicked;
	FOnClicked mOnStopClicked;
	FOnClicked mOnBeginningClicked;
	FOnClicked mOnEndClicked;
	FOnClicked mOnPreviousClicked;
	FOnClicked mOnNextClicked;
	FOnClicked mOnPreviousKeyClicked;
	FOnClicked mOnNextKeyClicked;
	FOnClicked mOnLoopClicked;
	FOnFrameRateChanged mOnFrameRateChanged;
};
