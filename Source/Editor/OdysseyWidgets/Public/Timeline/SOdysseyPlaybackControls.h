// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SButton.h"

DECLARE_DELEGATE_OneParam(FOnFrameRateChanged, float);

//////////////////////////////////////////////////////////////////////////
// SOdysseyPlaybackControlss
// Diplays all controls for a playback (play / stop / next / end / previous / start / framerate)
class ODYSSEYWIDGETS_API SOdysseyPlaybackControls : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SOdysseyPlaybackControls)
		{}
		SLATE_ATTRIBUTE(bool, IsPlaying)
		SLATE_ATTRIBUTE(bool, IsLooping)
		SLATE_EVENT(FOnClicked, OnPlayClicked)
		SLATE_EVENT(FOnClicked, OnStopClicked)
		SLATE_EVENT(FOnClicked, OnBeginningClicked)
		SLATE_EVENT(FOnClicked, OnEndClicked)
		SLATE_EVENT(FOnClicked, OnPreviousClicked)
		SLATE_EVENT(FOnClicked, OnNextClicked)
		SLATE_EVENT(FOnClicked, OnLoopClicked)
		SLATE_EVENT(FOnFrameRateChanged, OnFrameRateChanged)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	EVisibility GetPlayButtonVisibility() const;
	EVisibility GetStopButtonVisibility() const;
	EVisibility GetLoopingButtonVisibility() const;
	EVisibility GetNotLoopingButtonVisibility() const;

private:
	TAttribute<bool> mIsPlaying;
	TAttribute<bool> mIsLooping;

	FOnClicked mOnPlayClicked;
	FOnClicked mOnStopClicked;
	FOnClicked mOnBeginningClicked;
	FOnClicked mOnEndClicked;
	FOnClicked mOnPreviousClicked;
	FOnClicked mOnNextClicked;
	FOnClicked mOnLoopClicked;
	FOnFrameRateChanged mOnFrameRateChanged;
};
