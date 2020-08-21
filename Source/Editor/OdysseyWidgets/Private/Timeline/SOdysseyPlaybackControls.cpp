// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "SOdysseyPlaybackControls.h"

void
SOdysseyPlaybackControls::Construct(const FArguments& InArgs)
{
	mIsPlaying = InArgs._IsPlaying;
	mIsPlayingBackward = InArgs._IsPlayingBackward;
	mIsLooping = InArgs._IsLooping;

	mOnPlayClicked = InArgs._OnPlayClicked;
	mOnPlayBackwardClicked = InArgs._OnPlayBackwardClicked;
	mOnPauseClicked = InArgs._OnPauseClicked;
	mOnStopClicked = InArgs._OnStopClicked;
	mOnBeginningClicked = InArgs._OnBeginningClicked;
	mOnEndClicked = InArgs._OnEndClicked;
	mOnPreviousClicked = InArgs._OnPreviousClicked;
	mOnNextClicked = InArgs._OnNextClicked;
	mOnPreviousKeyClicked = InArgs._OnPreviousKeyClicked;
	mOnNextKeyClicked = InArgs._OnNextKeyClicked;
	mOnLoopClicked = InArgs._OnLoopClicked;
	mOnFrameRateChanged = InArgs._OnFrameRateChanged;

	ChildSlot
	[
		SNew(SHorizontalBox)
		.Clipping(EWidgetClipping::ClipToBounds)
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.Beginning"))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(mOnBeginningClicked)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.PreviousKey"))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(mOnPreviousKeyClicked)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.Previous"))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(mOnPreviousClicked)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.PlayBackward"))
			.Visibility(this, &SOdysseyPlaybackControls::GetPlayBackwardButtonVisibility)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(mOnPlayBackwardClicked)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.Pause"))
			.Visibility(this, &SOdysseyPlaybackControls::GetPauseBackwardButtonVisibility)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(mOnPauseClicked)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.Stop"))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(mOnStopClicked)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.Play"))
			.Visibility(this, &SOdysseyPlaybackControls::GetPlayButtonVisibility)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(mOnPlayClicked)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.Pause"))
			.Visibility(this, &SOdysseyPlaybackControls::GetPauseButtonVisibility)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(mOnPauseClicked)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.Next"))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(mOnNextClicked)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.NextKey"))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(mOnNextKeyClicked)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.End"))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(mOnEndClicked)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Visibility(this, &SOdysseyPlaybackControls::GetLoopingButtonVisibility)
			.ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.Looping"))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(mOnLoopClicked)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Visibility(this, &SOdysseyPlaybackControls::GetNotLoopingButtonVisibility)
			.ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.NotLooping"))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(mOnLoopClicked)
		]
		
		//SAssignNew(MainBoxPtr, SHorizontalBox)
	];

	// Rebuild();
}

EVisibility
SOdysseyPlaybackControls::GetPlayButtonVisibility() const
{
	return mIsPlaying.Get() && !mIsPlayingBackward.Get() ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility
SOdysseyPlaybackControls::GetPlayBackwardButtonVisibility() const
{
	return mIsPlaying.Get() && mIsPlayingBackward.Get() ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility
SOdysseyPlaybackControls::GetPauseButtonVisibility() const
{
	return mIsPlaying.Get() && !mIsPlayingBackward.Get() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility
SOdysseyPlaybackControls::GetPauseBackwardButtonVisibility() const
{
	return mIsPlaying.Get() && mIsPlayingBackward.Get() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility
SOdysseyPlaybackControls::GetLoopingButtonVisibility() const
{
	return mIsLooping.Get() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility
SOdysseyPlaybackControls::GetNotLoopingButtonVisibility() const
{
	return mIsLooping.Get() ? EVisibility::Collapsed : EVisibility::Visible;
}