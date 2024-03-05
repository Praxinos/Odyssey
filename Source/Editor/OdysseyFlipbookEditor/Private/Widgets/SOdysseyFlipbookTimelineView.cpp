// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyFlipbookTimelineView.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Texture2D.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Widgets/Input/SSlider.h"

#include "SOdysseyPlaybackControls.h"
#include "SOdysseyTimeline.h"
#include "SOdysseyFlipbookTimelineTrack.h"
#include "OdysseySurfaceTexture2DEditable.h"

#define LOCTEXT_NAMESPACE "FlipbookEditor"
#define MaxZoom 1.0
#define MinZoom 0.01
#define ZoomStep 0.01

//CONSTRUCTION/DESTRUCTION-----------------------------------------------


SOdysseyFlipbookTimelineView::~SOdysseyFlipbookTimelineView()
{
}


void SOdysseyFlipbookTimelineView::Construct(const FArguments& InArgs)
{
    mFlipbookWrapper = InArgs._FlipbookWrapper;

    mIsPlaying = false;
    mIsPlayingBackward = false;
    mIsLooping = true;
    mScrubPositionBeforePlay = 0.0f;

	mOnCurrentKeyframeChanged = InArgs._OnCurrentKeyframeChanged;
	mOnPlayStarted = InArgs._OnPlayStarted;
	mOnPlayStopped = InArgs._OnPlayStopped;
	mOnScrubStarted = InArgs._OnScrubStarted;
	mOnScrubStopped = InArgs._OnScrubStopped;
	mOnScrubPositionChanged = InArgs._OnScrubPositionChanged;
    mOnFlipbookChanged = InArgs._OnFlipbookChanged;
	mOnSpriteCreated = InArgs._OnSpriteCreated;
	mOnTextureCreated = InArgs._OnTextureCreated;

    // create zoom menu
    FMenuBuilder ZoomMenuBuilder(true, NULL);
    {
        FUIAction Zoom25Action(FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::OnZoomMenuEntryClicked, 0.25));
        ZoomMenuBuilder.AddMenuEntry(LOCTEXT("timeline.zoom.25percent.name", "25%"), LOCTEXT("timeline.zoom.25percent.tooltip", "Show the texture at a quarter of its size."), FSlateIcon(), Zoom25Action);

        FUIAction Zoom50Action(FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::OnZoomMenuEntryClicked, 0.5));
        ZoomMenuBuilder.AddMenuEntry(LOCTEXT("timeline.zoom.50percent.name", "50%"), LOCTEXT("timeline.zoom.50percent.tooltip", "Show the texture at half its size."), FSlateIcon(), Zoom50Action);

        FUIAction Zoom100Action(FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::OnZoomMenuEntryClicked, 1.0));
        ZoomMenuBuilder.AddMenuEntry(LOCTEXT("timeline.zoom.100percent.name", "100%"), LOCTEXT("timeline.zoom.100percent.tooltip", "Show the texture in its original size."), FSlateIcon(), Zoom100Action);

        FUIAction Zoom200Action(FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::OnZoomMenuEntryClicked, 2.0));
        ZoomMenuBuilder.AddMenuEntry(LOCTEXT("timeline.zoom.200percent.name", "200%"), LOCTEXT("timeline.zoom.200percent.tooltip", "Show the texture at twice its size."), FSlateIcon(), Zoom200Action);

        FUIAction Zoom400Action(FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::OnZoomMenuEntryClicked, 4.0));
        ZoomMenuBuilder.AddMenuEntry(LOCTEXT("timeline.zoom.400percent.name", "400%"), LOCTEXT("timeline.zoom.400percent.tooltip", "Show the texture at four times its size."), FSlateIcon(), Zoom400Action);

        ZoomMenuBuilder.AddMenuSeparator();
    }

    ChildSlot
    [
        SNew(SVerticalBox)

        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SOverlay)
            + SOverlay::Slot()
            .HAlign(HAlign_Center)
            [
                SNew(SOdysseyPlaybackControls)
                .IsPlaying(this, &SOdysseyFlipbookTimelineView::IsPlaying)
                .IsPlayingBackward(this, &SOdysseyFlipbookTimelineView::IsPlayingBackward)
                .IsLooping(this, &SOdysseyFlipbookTimelineView::IsLooping)
                .OnPlayClicked(this, &SOdysseyFlipbookTimelineView::OnPlayClicked, false)
                .OnPlayBackwardClicked(this, &SOdysseyFlipbookTimelineView::OnPlayClicked, true)
                .OnPauseClicked(this, &SOdysseyFlipbookTimelineView::OnPauseClicked)
                .OnStopClicked(this, &SOdysseyFlipbookTimelineView::OnStopClicked)
                .OnBeginningClicked(this, &SOdysseyFlipbookTimelineView::OnBeginningClicked)
                .OnEndClicked(this, &SOdysseyFlipbookTimelineView::OnEndClicked)
                .OnPreviousClicked(this, &SOdysseyFlipbookTimelineView::OnPreviousClicked)
                .OnNextClicked(this, &SOdysseyFlipbookTimelineView::OnNextClicked)
                .OnPreviousKeyClicked(this, &SOdysseyFlipbookTimelineView::OnPreviousKeyClicked)
                .OnNextKeyClicked(this, &SOdysseyFlipbookTimelineView::OnNextKeyClicked)
                .OnLoopClicked(this, &SOdysseyFlipbookTimelineView::OnLoopClicked)
                //.OnFrameRateChanged(this, &SOdysseyFlipbookTimelineView::OnFrameRateChanged)
            ]
			+ SOverlay::Slot()
			.HAlign(HAlign_Left)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					//.HAlign(HAlign_Center)
					.Text(LOCTEXT("timeline.add-keyframe", "Add Keyframe"))
					.OnClicked(this, &SOdysseyFlipbookTimelineView::OnAddFrameClicked)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.Visibility(this, &SOdysseyFlipbookTimelineView::FixCurrentFrameVisibility)
					//.HAlign(HAlign_Center)
					.Text(LOCTEXT("timeline.fix-current-keyframe", "Fix Current Keyframe"))
					.OnClicked(this, &SOdysseyFlipbookTimelineView::OnFixCurrentFrameClicked)
				]
			]
			+ SOverlay::Slot()
			.HAlign(HAlign_Right)
			[
				SNew(SNumericDropDown<float>)
				.bShowNamedValue(false)
				.DropDownValues(FrameRateDropDownValues())
                .LabelText(LOCTEXT("timeline.framerate", "Frame rate"))
                .MinDesiredValueWidth(50)
                .OnValueChanged(this, &SOdysseyFlipbookTimelineView::OnFrameRateChanged)
                .Orientation(EOrientation::Orient_Horizontal)
                .Value(this, &SOdysseyFlipbookTimelineView::GetFrameRate)
			]
        ]

		+ SVerticalBox::Slot()
        .FillHeight(1.0f)
		.Padding(0, 0, 0, 0)
		[
			SAssignNew(mTimelineWidget, SOdysseyTimeline)
            .OnScrubStarted(this, &SOdysseyFlipbookTimelineView::OnScrubStarted)
            .OnScrubPositionChanged(this, &SOdysseyFlipbookTimelineView::OnScrubPositionChanged)
            .OnScrubStopped(InArgs._OnScrubStopped)
            [
                SAssignNew(mTimelineTrackWidget, SOdysseyFlipbookTimelineTrack)
				.FlipbookWrapper(mFlipbookWrapper.Pin())
				.FrameSize(this, &SOdysseyFlipbookTimelineView::FrameSize)
                .OnFlipbookChanged(mOnFlipbookChanged)
                .OnKeyframeRemoved(InArgs._OnKeyframeRemoved)
                .OnKeyframeAdded(this, &SOdysseyFlipbookTimelineView::OnKeyframeAdded)
                .OnSpriteCreated(this, &SOdysseyFlipbookTimelineView::OnSpriteCreated)
                .OnTextureCreated(this, &SOdysseyFlipbookTimelineView::OnTextureCreated)
            ]
		]
        + SVerticalBox::Slot()
		.AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot() //Spacer
            + SHorizontalBox::Slot() //Spacer
            + SHorizontalBox::Slot()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                        .Text(LOCTEXT("timeline.zoom", "Zoom:"))
                ]

                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                .Padding(4.0f, 0.0f)
                .VAlign(VAlign_Center)
                [
                    SNew(SSlider)
                        .OnValueChanged(this, &SOdysseyFlipbookTimelineView::OnZoomSliderChanged)
                        .Value(this, &SOdysseyFlipbookTimelineView::ZoomSliderValue)
                ]

                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                        .Text(this, &SOdysseyFlipbookTimelineView::OnZoomPercentageText)
                ]

                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(2.0f, 0.0f, 0.0f, 0.0f)
                .VAlign(VAlign_Center)
                [
                    SNew(SComboButton)
                        .ContentPadding(FMargin(0.0))
                        .MenuContent()
                        [
                            ZoomMenuBuilder.MakeWidget()
                        ]
                ]
            ]
        ]
    ];
}

TSharedPtr<SWidget>
SOdysseyFlipbookTimelineView::OnArrowNavigation(EUINavigation iNavigation)
{
    switch(iNavigation)
    {
        case EUINavigation::Left:
            ScrubToPreviousFrame();
        break;

        case EUINavigation::Right:
            ScrubToNextFrame();
        break;

        case EUINavigation::Next:
            ScrubToNextKeyFrame();
        break;

        case EUINavigation::Previous:
            ScrubToPreviousKeyFrame();
        break;
    }
    return NULL;
}

//In Frames
float
SOdysseyFlipbookTimelineView::GetScrubPosition() const
{
    return mTimelineWidget->ScrubPosition();
}

//The keyframe index at Scrub position
int
SOdysseyFlipbookTimelineView::GetCurrentKeyframeIndex() const
{
	TSharedPtr<FOdysseyFlipbookWrapper> wrapper = mFlipbookWrapper.Pin();
	if (!wrapper)
		return INDEX_NONE;

    return wrapper->GetKeyframeIndexAtPosition(GetScrubPosition());
}

void
SOdysseyFlipbookTimelineView::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
    
    if (!mIsPlaying)
        return;

	TSharedPtr<FOdysseyFlipbookWrapper> wrapper = mFlipbookWrapper.Pin();
	if (!wrapper)
		return;

    UPaperFlipbook* flipbook = wrapper->GetFlipbook();
    if (!flipbook)
        return;
    
    float scrubPosition = mTimelineWidget->ScrubPosition();
    float offset = mIsPlayingBackward ? -InDeltaTime * flipbook->GetFramesPerSecond() : InDeltaTime * flipbook->GetFramesPerSecond();
    scrubPosition += offset;
    if (mIsLooping)
    {
        if (mIsPlayingBackward)
        {
            if (scrubPosition < 0)
            {
                scrubPosition += flipbook->GetNumFrames();
            }
        }
        else
        {
            scrubPosition = FGenericPlatformMath::Fmod(scrubPosition, flipbook->GetNumFrames());
        }
        
    }
    else if (!mIsPlayingBackward && scrubPosition >= flipbook->GetNumFrames())
    {
        //If we don't loop and we play after the flipbook duration, then we stop playing
        Stop();
        return;
    }
    else if (mIsPlayingBackward && scrubPosition < 0)
    {
        //If we don't loop and we play after the flipbook duration, then we stop playing
        Stop();
        return;
    }

    mTimelineWidget->ScrubPosition(scrubPosition);
}

void
SOdysseyFlipbookTimelineView::OnScrubStarted()
{
    //If we're already playing, then it means a manual scrub started
    //So we stop playing and just continue scrubbing manually
    if (mIsPlaying)
    {
	    mIsPlaying = false;
        mOnPlayStopped.ExecuteIfBound();
        return;
    }
	
    //If we're not playing, just start scrubbing
    mOnScrubStarted.ExecuteIfBound();
}

void
SOdysseyFlipbookTimelineView::OnScrubPositionChanged(float iOldPosition)
{
    mOnScrubPositionChanged.ExecuteIfBound(iOldPosition);

	TSharedPtr<FOdysseyFlipbookWrapper> wrapper = mFlipbookWrapper.Pin();
	if (!wrapper)
		return;

    int32 previousFrame = wrapper->GetKeyframeIndexAtPosition(iOldPosition);
    int32 nextFrame = GetCurrentKeyframeIndex();

    //if both frame are out of range we do nothing
    if (previousFrame == -1 && nextFrame == -1)
        return;

    //if at least one frame is in range and both frame are different, we admit we changed current frame
    if (nextFrame != previousFrame)
    {
        mOnCurrentKeyframeChanged.ExecuteIfBound(nextFrame);
    }
}

void
SOdysseyFlipbookTimelineView::PlayOrPause(bool iBackward)
{
    if (mIsPlaying && mIsPlayingBackward == iBackward)
    {
        Pause();
        return;
    }

    Play(iBackward);
}

void
SOdysseyFlipbookTimelineView::Play(bool iBackward)
{
    //Allow playing in the other direction when we are already playing in a direction
    mIsPlayingBackward = iBackward;

    if (mIsPlaying)
        return;

    mScrubPositionBeforePlay = mTimelineWidget->ScrubPosition();
	mIsPlaying = true;

    //start scrubbing
    mOnScrubStarted.ExecuteIfBound();

    //Activate the play, which is basically a automated scrub
	mOnPlayStarted.ExecuteIfBound();
}

void
SOdysseyFlipbookTimelineView::Stop()
{
    if (!mIsPlaying)
        return;

    mTimelineWidget->ScrubPosition(mScrubPositionBeforePlay);

    mIsPlaying = false;
    mIsPlayingBackward = false;

    //We stop the play to stop any automated scrubbing
	mOnPlayStopped.ExecuteIfBound();

    //We stop the scrubbing
    mOnScrubStopped.ExecuteIfBound();
}

void
SOdysseyFlipbookTimelineView::Pause()
{
    if (!mIsPlaying)
        return;

    mIsPlaying = false;
    mIsPlayingBackward = false;

    //We stop the play to stop any automated scrubbing
	mOnPlayStopped.ExecuteIfBound();

    //We stop the scrubbing
    mOnScrubStopped.ExecuteIfBound();
}

bool
SOdysseyFlipbookTimelineView::IsLooping() const
{
	return mIsLooping;
}

bool
SOdysseyFlipbookTimelineView::IsPlaying() const
{
	return mIsPlaying;
}

bool
SOdysseyFlipbookTimelineView::IsPlayingBackward() const
{
	return mIsPlayingBackward;
}

bool
SOdysseyFlipbookTimelineView::IsScrubbing() const
{
	return mIsPlaying || mTimelineWidget->IsScrubbing();
}

float
SOdysseyFlipbookTimelineView::FrameSize() const
{
    return mTimelineWidget->FrameSize();
}

void
SOdysseyFlipbookTimelineView::OnKeyframeAdded(FPaperFlipbookKeyFrame& iKeyFrame)
{
    if (!iKeyFrame.Sprite)
        return;

    mOnSpriteCreated.ExecuteIfBound(iKeyFrame.Sprite);

    UTexture2D* texture = iKeyFrame.Sprite->GetSourceTexture();
    if (!texture)
        return;
    
    mOnTextureCreated.ExecuteIfBound(texture);
}

void
SOdysseyFlipbookTimelineView::OnFrameRateChanged(float iFrameRate)
{
	TSharedPtr<FOdysseyFlipbookWrapper> wrapper = mFlipbookWrapper.Pin();
	if (!wrapper)
		return;

	UPaperFlipbook* flipbook = wrapper->GetFlipbook();
	if (!flipbook)
		return;

	//Using a mutator is mandatory to change the framerate of the flipbook
	FScopedFlipbookMutator mutator(flipbook);
	mutator.FramesPerSecond = iFrameRate; //This changes directly the flipbook framerate
}

FReply
SOdysseyFlipbookTimelineView::OnPlayClicked(bool iBackward)
{
	Play(iBackward);
	return FReply::Handled();
}

FReply
SOdysseyFlipbookTimelineView::OnPauseClicked()
{
	Pause();
	return FReply::Handled();
}

FReply
SOdysseyFlipbookTimelineView::OnStopClicked()
{
	Stop();
	return FReply::Handled();
}

FReply
SOdysseyFlipbookTimelineView::OnBeginningClicked()
{
    ScrubToFirstFrame();
	return FReply::Handled();
}

FReply
SOdysseyFlipbookTimelineView::OnEndClicked()
{
    ScrubToLastFrame();
	return FReply::Handled();
}

FReply
SOdysseyFlipbookTimelineView::OnPreviousClicked()
{
    ScrubToPreviousFrame();
	return FReply::Handled();
}

FReply
SOdysseyFlipbookTimelineView::OnNextClicked()
{
    ScrubToNextFrame();
	return FReply::Handled();
}

FReply
SOdysseyFlipbookTimelineView::OnPreviousKeyClicked()
{
    ScrubToPreviousKeyFrame();
    return FReply::Handled();
}

FReply
SOdysseyFlipbookTimelineView::OnNextKeyClicked()
{
    ScrubToNextKeyFrame();
	return FReply::Handled();
}

FReply
SOdysseyFlipbookTimelineView::OnLoopClicked()
{
    ToggleLooping();
	return FReply::Handled();
}

FReply
SOdysseyFlipbookTimelineView::OnAddFrameClicked()
{
	AddFrame();
	return FReply::Handled();
}

FReply
SOdysseyFlipbookTimelineView::OnFixCurrentFrameClicked()
{
    FixFrame(GetCurrentKeyframeIndex());
	return FReply::Handled();
}

void
SOdysseyFlipbookTimelineView::OnZoomMenuEntryClicked( double ZoomValue )
{
    mTimelineWidget->Zoom( ZoomValue );
}

FText
SOdysseyFlipbookTimelineView::OnZoomPercentageText( ) const
{
    return FText::AsPercent(mTimelineWidget->Zoom());
}

void
SOdysseyFlipbookTimelineView::OnZoomSliderChanged( float NewValue )
{
	mTimelineWidget->Zoom( NewValue * MaxZoom );
}


float
SOdysseyFlipbookTimelineView::ZoomSliderValue() const
{
    return  (mTimelineWidget->Zoom() / MaxZoom );
}

bool
SOdysseyFlipbookTimelineView::FixFrame(int32 iIndex)
{
	TSharedPtr<FOdysseyFlipbookWrapper> wrapper = mFlipbookWrapper.Pin();
	if (!wrapper)
		return false;

    UTexture2D* createdTexture = NULL;
    UPaperSprite* createdSprite = NULL;
    if (!wrapper->FixKeyFrame(iIndex, &createdTexture, &createdSprite))
        return false;
    
    if (createdSprite)
        mOnSpriteCreated.ExecuteIfBound(createdSprite);
    
    if (createdTexture)
    {
        mOnTextureCreated.ExecuteIfBound(createdTexture);
        mTimelineTrackWidget->SetFrame(iIndex, createdTexture);
    }

	mOnFlipbookChanged.ExecuteIfBound();

	return true;
}

void
SOdysseyFlipbookTimelineView::AddFrame(int32 iIndex)
{
	TSharedPtr<FOdysseyFlipbookWrapper> wrapper = mFlipbookWrapper.Pin();
	if (!wrapper)
		return;

    UTexture2D* createdTexture = NULL;
    UPaperSprite* createdSprite = NULL;
    if (!wrapper->CreateKeyFrame(iIndex, &createdTexture, &createdSprite))
        return;

	mTimelineTrackWidget->InsertFrame(iIndex, createdTexture, 1);

    mOnSpriteCreated.ExecuteIfBound(createdSprite);
    mOnTextureCreated.ExecuteIfBound(createdTexture);
	mOnFlipbookChanged.ExecuteIfBound();

	mTimelineWidget->ScrubPosition(wrapper->GetKeyframeStartPosition(iIndex) + 0.5f);
}

void
SOdysseyFlipbookTimelineView::AddFrame()
{
	TSharedPtr<FOdysseyFlipbookWrapper> wrapper = mFlipbookWrapper.Pin();
	if (!wrapper)
		return;

	UPaperFlipbook* flipbook = wrapper->GetFlipbook();
	if (!flipbook)
		return;

    AddFrame(flipbook->GetNumKeyFrames());
}

void
SOdysseyFlipbookTimelineView::AddFrameAfter()
{
	TSharedPtr<FOdysseyFlipbookWrapper> wrapper = mFlipbookWrapper.Pin();
	if (!wrapper)
		return;

	UPaperFlipbook* flipbook = wrapper->GetFlipbook();
	if (!flipbook)
		return;

    int32 index = GetCurrentKeyframeIndex();
    if (index < 0 || index >= flipbook->GetNumKeyFrames())
    {
        if (GetScrubPosition() < 0)
        {
            AddFrame(0);
        }
        else
        {
            AddFrame(flipbook->GetNumKeyFrames());
        }
        return;
    }
    
    AddFrame(index + 1);
}

void
SOdysseyFlipbookTimelineView::AddFrameBefore()
{
	TSharedPtr<FOdysseyFlipbookWrapper> wrapper = mFlipbookWrapper.Pin();
	if (!wrapper)
		return;

	UPaperFlipbook* flipbook = wrapper->GetFlipbook();
	if (!flipbook)
		return;

    int32 index = GetCurrentKeyframeIndex();
    if (index < 0 || index >= flipbook->GetNumKeyFrames())
    {
        if (GetScrubPosition() < 0)
        {
            AddFrame(0);
        }
        else
        {
            AddFrame(flipbook->GetNumKeyFrames());
        }
        return;
    }
    AddFrame(index);
}

void
SOdysseyFlipbookTimelineView::ScrubToFirstFrame()
{
	mTimelineWidget->ScrubPosition(0.5f);
}

void
SOdysseyFlipbookTimelineView::ScrubToLastFrame()
{
	TSharedPtr<FOdysseyFlipbookWrapper> wrapper = mFlipbookWrapper.Pin();
	if (!wrapper)
		return;

	UPaperFlipbook* flipbook = wrapper->GetFlipbook();
	if (!flipbook)
		return;

	mTimelineWidget->ScrubPosition(flipbook->GetNumFrames() - 0.5f);
}

void
SOdysseyFlipbookTimelineView::ScrubToNextFrame()
{
    if (mTimelineWidget->ScrubPosition() < 0.f)
    {
        mTimelineWidget->ScrubPosition(0.5f);
        return;
    }

    mTimelineWidget->ScrubPosition(FGenericPlatformMath::FloorToFloat(mTimelineWidget->ScrubPosition() + 1.0f) + 0.5f);
}

void
SOdysseyFlipbookTimelineView::ScrubToPreviousFrame()
{
    if (mTimelineWidget->ScrubPosition() < 1.0f)
        return;

    mTimelineWidget->ScrubPosition(FGenericPlatformMath::FloorToFloat(mTimelineWidget->ScrubPosition() - 1.0f) + 0.5f);
}

void
SOdysseyFlipbookTimelineView::ScrubToNextKeyFrame()
{
	TSharedPtr<FOdysseyFlipbookWrapper> wrapper = mFlipbookWrapper.Pin();
	if (!wrapper)
		return;

	UPaperFlipbook* flipbook = wrapper->GetFlipbook();
	if (!flipbook)
		return;

    int32 index = wrapper->GetKeyframeIndexAtPosition(mTimelineWidget->ScrubPosition());
    if (index >= flipbook->GetNumKeyFrames() - 1)
        return;

    if (index < 0)
    {
        if (mTimelineWidget->ScrubPosition() >= flipbook->GetNumFrames())
        {
            float position = wrapper->GetKeyframeStartPosition(flipbook->GetNumKeyFrames() - 1);
            mTimelineWidget->ScrubPosition(position + 0.5f);
        }
        else
        {
            float position = wrapper->GetKeyframeStartPosition(0);
            mTimelineWidget->ScrubPosition(position + 0.5f);
        }
        return;
    }

    float position = wrapper->GetKeyframeStartPosition(index + 1);
    mTimelineWidget->ScrubPosition(position + 0.5f);
}

void
SOdysseyFlipbookTimelineView::ScrubToPreviousKeyFrame()
{
	TSharedPtr<FOdysseyFlipbookWrapper> wrapper = mFlipbookWrapper.Pin();
	if (!wrapper)
		return;

	UPaperFlipbook* flipbook = wrapper->GetFlipbook();
	if (!flipbook)
		return;

    int32 index = wrapper->GetKeyframeIndexAtPosition(mTimelineWidget->ScrubPosition());
    if (index == 0)
        return;
        
    if (index < 0)
    {
        if (mTimelineWidget->ScrubPosition() >= flipbook->GetNumFrames())
        {
            float position = wrapper->GetKeyframeStartPosition(flipbook->GetNumKeyFrames() - 1);
            mTimelineWidget->ScrubPosition(position + 0.5f);
        }
        else
        {
            float position = wrapper->GetKeyframeStartPosition(0);
            mTimelineWidget->ScrubPosition(position + 0.5f);
        }
        return;
    }

    float position = wrapper->GetKeyframeStartPosition(index - 1);
    mTimelineWidget->ScrubPosition(position + 0.5f);
}


void
SOdysseyFlipbookTimelineView::ToggleLooping()
{
	mIsLooping = !mIsLooping;
}


EVisibility
SOdysseyFlipbookTimelineView::FixCurrentFrameVisibility() const
{
	TSharedPtr<FOdysseyFlipbookWrapper> wrapper = mFlipbookWrapper.Pin();
	if (!wrapper)
		return EVisibility::Collapsed;

	UPaperFlipbook* flipbook = wrapper->GetFlipbook();
	if (!flipbook)
		return EVisibility::Collapsed;

    int32 index = GetCurrentKeyframeIndex();
    if (index < 0 || index >= flipbook->GetNumKeyFrames())
        return EVisibility::Collapsed;

    return wrapper->GetKeyframeTexture(index) ? EVisibility::Collapsed : EVisibility::Visible;
}

TArray<SNumericDropDown<float>::FNamedValue>
SOdysseyFlipbookTimelineView::FrameRateDropDownValues() const
{
    TArray<SNumericDropDown<float>::FNamedValue> values;
    FText nameFormat = LOCTEXT("timeline.framerate.n-fps.name", "{0} FPS");
    FText tooltipFormat = LOCTEXT("timeline.framerate.n-fps.tooltip", "{0} frame(s) per second");

    FText::Format(nameFormat, FText::FromString("1"));
    FText::Format(tooltipFormat, FText::FromString("1"));
    values.Add(SNumericDropDown<float>::FNamedValue(1.0f, FText::Format(nameFormat, FText::FromString("1")), FText::Format(tooltipFormat, FText::FromString("1"))));
    values.Add(SNumericDropDown<float>::FNamedValue(2.0f, FText::Format(nameFormat, FText::FromString("2")), FText::Format(tooltipFormat, FText::FromString("2"))));
    values.Add(SNumericDropDown<float>::FNamedValue(3.0f, FText::Format(nameFormat, FText::FromString("3")), FText::Format(tooltipFormat, FText::FromString("3"))));
    values.Add(SNumericDropDown<float>::FNamedValue(6.0f, FText::Format(nameFormat, FText::FromString("6")), FText::Format(tooltipFormat, FText::FromString("6"))));
    values.Add(SNumericDropDown<float>::FNamedValue(12.0f, FText::Format(nameFormat, FText::FromString("12")), FText::Format(tooltipFormat, FText::FromString("12"))));
    values.Add(SNumericDropDown<float>::FNamedValue(24.0f, FText::Format(nameFormat, FText::FromString("24")), FText::Format(tooltipFormat, FText::FromString("24"))));
    values.Add(SNumericDropDown<float>::FNamedValue(29.97f, FText::Format(nameFormat, FText::FromString("29.97")), FText::Format(tooltipFormat, FText::FromString("29.97"))));
    values.Add(SNumericDropDown<float>::FNamedValue(30.0f, FText::Format(nameFormat, FText::FromString("30")), FText::Format(tooltipFormat, FText::FromString("30"))));
    values.Add(SNumericDropDown<float>::FNamedValue(60.0f, FText::Format(nameFormat, FText::FromString("60")), FText::Format(tooltipFormat, FText::FromString("60"))));

    return values;
}

float
SOdysseyFlipbookTimelineView::GetFrameRate() const
{
	TSharedPtr<FOdysseyFlipbookWrapper> wrapper = mFlipbookWrapper.Pin();
	if (!wrapper)
		return 0.f;

	UPaperFlipbook* flipbook = wrapper->GetFlipbook();
	if (!flipbook)
		return 0.f;

    return flipbook->GetFramesPerSecond();
}

void
SOdysseyFlipbookTimelineView::OnSpriteCreated(UPaperSprite* iSprite)
{
	mOnSpriteCreated.ExecuteIfBound(iSprite);
}

void
SOdysseyFlipbookTimelineView::OnTextureCreated(UTexture2D* iTexture)
{
    mOnTextureCreated.ExecuteIfBound(iTexture);
}

void
SOdysseyFlipbookTimelineView::BindCommands(const TSharedRef<FUICommandList>& iCommandList)
{
	iCommandList->MapAction(FOdysseyFlipbookEditorCommands::Get().AddNewKeyFrame, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::AddFrame)));
    iCommandList->MapAction(FOdysseyFlipbookEditorCommands::Get().AddNewKeyFrameAfter, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::AddFrameAfter)));
    iCommandList->MapAction(FOdysseyFlipbookEditorCommands::Get().AddNewKeyFrameBefore, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::AddFrameBefore)));
    iCommandList->MapAction(FOdysseyFlipbookEditorCommands::Get().PlayForward, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::PlayOrPause, false)));
    iCommandList->MapAction(FOdysseyFlipbookEditorCommands::Get().PlayBackward, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::PlayOrPause, true)));
    iCommandList->MapAction(FOdysseyFlipbookEditorCommands::Get().Pause, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::Pause)));
    iCommandList->MapAction(FOdysseyFlipbookEditorCommands::Get().Stop, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::Stop)));
    iCommandList->MapAction(FOdysseyFlipbookEditorCommands::Get().PreviousFrame, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::ScrubToPreviousFrame)));
    iCommandList->MapAction(FOdysseyFlipbookEditorCommands::Get().NextFrame, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::ScrubToNextFrame)));
    iCommandList->MapAction(FOdysseyFlipbookEditorCommands::Get().PreviousKeyFrame, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::ScrubToPreviousKeyFrame)));
    iCommandList->MapAction(FOdysseyFlipbookEditorCommands::Get().NextKeyFrame, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::ScrubToNextKeyFrame)));
    iCommandList->MapAction(FOdysseyFlipbookEditorCommands::Get().FirstFrame, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::ScrubToFirstFrame)));
    iCommandList->MapAction(FOdysseyFlipbookEditorCommands::Get().LastFrame, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::ScrubToLastFrame)));
    iCommandList->MapAction(FOdysseyFlipbookEditorCommands::Get().ToggleLooping, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::ToggleLooping)));
	mTimelineTrackWidget->BindCommands(iCommandList);
}

#undef LOCTEXT_NAMESPACE
