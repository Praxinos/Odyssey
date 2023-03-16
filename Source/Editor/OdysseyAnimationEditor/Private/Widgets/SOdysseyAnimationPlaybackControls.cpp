// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/SOdysseyAnimationPlaybackControls.h"

void
SOdysseyAnimationPlaybackControls::Construct(const FArguments& InArgs)
{
    mAnimation = InArgs._Animation;
	mPlayer = InArgs._Player;
    mPlaybackFramesPerSecond = InArgs._PlaybackFramesPerSecond;

	ChildSlot
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.Beginning"))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(this, &SOdysseyAnimationPlaybackControls::OnBeginningClicked)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.PreviousKey"))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(this, &SOdysseyAnimationPlaybackControls::OnPreviousKeyClicked)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.Previous"))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(this, &SOdysseyAnimationPlaybackControls::OnPreviousClicked)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.PlayBackward"))
			.Visibility(this, &SOdysseyAnimationPlaybackControls::GetPlayBackwardButtonVisibility)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(this, &SOdysseyAnimationPlaybackControls::OnPlayBackwardClicked)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.Stop"))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(this, &SOdysseyAnimationPlaybackControls::OnStopClicked)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.Play"))
			.Visibility(this, &SOdysseyAnimationPlaybackControls::GetPlayButtonVisibility)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(this, &SOdysseyAnimationPlaybackControls::OnPlayClicked)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.Next"))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(this, &SOdysseyAnimationPlaybackControls::OnNextClicked)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.NextKey"))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(this, &SOdysseyAnimationPlaybackControls::OnNextKeyClicked)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.End"))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(this, &SOdysseyAnimationPlaybackControls::OnEndClicked)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Visibility(this, &SOdysseyAnimationPlaybackControls::GetLoopingButtonVisibility)
			.ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.Looping"))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(this, &SOdysseyAnimationPlaybackControls::OnLoopClicked)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Visibility(this, &SOdysseyAnimationPlaybackControls::GetNotLoopingButtonVisibility)
			.ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.NotLooping"))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(this, &SOdysseyAnimationPlaybackControls::OnLoopClicked)
		]
	];
}

bool
SOdysseyAnimationPlaybackControls::IsPlayingForward() const
{
    return mPlayer->GetStatus() == EOdysseyAnimationPlayerStatus::Playing && !mPlayer->IsBackward();
}

bool
SOdysseyAnimationPlaybackControls::IsPlayingBackward() const
{
	return mPlayer->GetStatus() == EOdysseyAnimationPlayerStatus::Playing && mPlayer->IsBackward();
}


EVisibility
SOdysseyAnimationPlaybackControls::GetPlayButtonVisibility() const
{
	return IsPlayingForward() ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility
SOdysseyAnimationPlaybackControls::GetPlayBackwardButtonVisibility() const
{
	return IsPlayingBackward() ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility
SOdysseyAnimationPlaybackControls::GetLoopingButtonVisibility() const
{
	return mPlayer->GetIsLooping() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility
SOdysseyAnimationPlaybackControls::GetNotLoopingButtonVisibility() const
{
	return mPlayer->GetIsLooping() ? EVisibility::Collapsed : EVisibility::Visible;
}

FReply
SOdysseyAnimationPlaybackControls::OnPlayClicked()
{
	mPlayer->Play(false);
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnPlayBackwardClicked()
{
	mPlayer->Play(true);
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnStopClicked()
{
    mPlayer->Stop();
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnBeginningClicked()
{
	FOdysseyObjectEditorUtils::SetPropertyValue(mAnimation, "CurrentFrame", 0);
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnEndClicked()
{
	FOdysseyObjectEditorUtils::SetPropertyValue(mAnimation, "CurrentFrame", mAnimation->GetFrameRange().GetUpperBoundValue());
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnPreviousClicked()
{
	FOdysseyObjectEditorUtils::SetPropertyValue(mAnimation, "CurrentFrame", mAnimation->CurrentFrame - 1);
	return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnNextClicked()
{
	FOdysseyObjectEditorUtils::SetPropertyValue(mAnimation, "CurrentFrame", mAnimation->CurrentFrame + 1);
	return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnPreviousKeyClicked()
{
    OnPreviousClicked();
    //TODO: Seek to Previous KeyFrame instead of previous frame
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnNextKeyClicked()
{
    OnNextClicked();
    //TODO: Seek to Next KeyFrame instead of next frame
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnLoopClicked()
{
    mPlayer->SetIsLooping(!mPlayer->GetIsLooping());
    return FReply::Handled();
}
