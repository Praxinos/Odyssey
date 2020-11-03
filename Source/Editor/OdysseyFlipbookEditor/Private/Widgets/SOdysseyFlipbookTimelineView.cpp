// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "SOdysseyFlipbookTimelineView.h"

#include "AssetRegistryModule.h"
#include "Engine/Texture2D.h"
#include "GenericPlatform/GenericPlatformMath.h"

#include "SOdysseyPlaybackControls.h"
#include "SOdysseyTimeline.h"
#include "SOdysseyFlipbookTimelineTrack.h"
#include "OdysseyBlock.h"
#include "OdysseySurfaceEditable.h"
#include "OdysseyTexture.h"

#define LOCTEXT_NAMESPACE "OdysseyFlipbook"
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
        ZoomMenuBuilder.AddMenuEntry(LOCTEXT("Zoom25Action", "25%"), LOCTEXT("Zoom25ActionHint", "Show the texture at a quarter of its size."), FSlateIcon(), Zoom25Action);

        FUIAction Zoom50Action(FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::OnZoomMenuEntryClicked, 0.5));
        ZoomMenuBuilder.AddMenuEntry(LOCTEXT("Zoom50Action", "50%"), LOCTEXT("Zoom50ActionHint", "Show the texture at half its size."), FSlateIcon(), Zoom50Action);

        FUIAction Zoom100Action(FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::OnZoomMenuEntryClicked, 1.0));
        ZoomMenuBuilder.AddMenuEntry(LOCTEXT("Zoom100Action", "100%"), LOCTEXT("Zoom100ActionHint", "Show the texture in its original size."), FSlateIcon(), Zoom100Action);

        FUIAction Zoom200Action(FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::OnZoomMenuEntryClicked, 2.0));
        ZoomMenuBuilder.AddMenuEntry(LOCTEXT("Zoom200Action", "200%"), LOCTEXT("Zoom200ActionHint", "Show the texture at twice its size."), FSlateIcon(), Zoom200Action);

        FUIAction Zoom400Action(FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::OnZoomMenuEntryClicked, 4.0));
        ZoomMenuBuilder.AddMenuEntry(LOCTEXT("Zoom400Action", "400%"), LOCTEXT("Zoom400ActionHint", "Show the texture at four times its size."), FSlateIcon(), Zoom400Action);

        ZoomMenuBuilder.AddMenuSeparator();

        /* FUIAction ZoomFitAction(
            FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::OnZoomMenuFitClicked),
            FCanExecuteAction(),
            FIsActionChecked::CreateSP(this, &SOdysseyFlipbookTimelineView::IsZoomMenuFitChecked)
            );
        ZoomMenuBuilder.AddMenuEntry(LOCTEXT("ZoomFitAction", "Scale To Fit"), LOCTEXT("ZoomFillActionHint", "Scale the texture to fit the viewport."), FSlateIcon(), ZoomFitAction, NAME_None, EUserInterfaceActionType::ToggleButton); */
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
					.Text(LOCTEXT("AddKeyframe", "Add Keyframe"))
					.OnClicked(this, &SOdysseyFlipbookTimelineView::OnAddFrameClicked)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.Visibility(this, &SOdysseyFlipbookTimelineView::FixCurrentFrameVisibility)
					//.HAlign(HAlign_Center)
					.Text(LOCTEXT("Fix Current Keyframe", "Fix Current Keyframe"))
					.OnClicked(this, &SOdysseyFlipbookTimelineView::OnFixCurrentFrameClicked)
				]
			]
			+ SOverlay::Slot()
			.HAlign(HAlign_Right)
			[
				SNew(SNumericDropDown<float>)
				.bShowNamedValue(false)
				.DropDownValues(FrameRateDropDownValues())
                .LabelText(LOCTEXT("FrameRate", "Frame rate"))
                .MinDesiredValueWidth(50)
                .OnValueChanged(this, &SOdysseyFlipbookTimelineView::OnFrameRateChanged)
                .Orientation(EOrientation::Orient_Horizontal)
                .Value(this, &SOdysseyFlipbookTimelineView::GetFrameRate)
			]
        ]

		+ SVerticalBox::Slot()
		.Padding(0, 0, 0, 0)
		[
			SAssignNew(mTimelineWidget, SOdysseyTimeline)
            .OnScrubStarted(this, &SOdysseyFlipbookTimelineView::OnScrubStarted)
            .OnScrubPositionChanged(this, &SOdysseyFlipbookTimelineView::OnScrubPositionChanged)
            .OnScrubStopped(InArgs._OnScrubStopped)
            [
                SAssignNew(mTimelineTrackWidget, SOdysseyFlipbookTimelineTrack)
				.FlipbookWrapper(mFlipbookWrapper)
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
                        .Text(LOCTEXT("ZoomLabel", "Zoom:"))
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
    return mFlipbookWrapper->GetKeyframeIndexAtPosition(GetScrubPosition());
}

void
SOdysseyFlipbookTimelineView::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
    
    if (!mIsPlaying)
        return;

    UPaperFlipbook* flipbook = mFlipbookWrapper->Flipbook();
    
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

    int32 previousFrame = mFlipbookWrapper->GetKeyframeIndexAtPosition(iOldPosition);
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
	//Using a mutator is mandatory to change the framerate of the flipbook
	FScopedFlipbookMutator mutator(mFlipbookWrapper->Flipbook());
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
    UTexture2D* createdTexture = NULL;
    UPaperSprite* createdSprite = NULL;
    if (!mFlipbookWrapper->FixKeyFrame(iIndex, &createdTexture, &createdSprite))
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
    UTexture2D* createdTexture = NULL;
    UPaperSprite* createdSprite = NULL;
    if (!mFlipbookWrapper->CreateKeyFrame(iIndex, &createdTexture, &createdSprite))
        return;

	mTimelineTrackWidget->InsertFrame(iIndex, createdTexture, 1);

    mOnSpriteCreated.ExecuteIfBound(createdSprite);
    mOnTextureCreated.ExecuteIfBound(createdTexture);
	mOnFlipbookChanged.ExecuteIfBound();

	mTimelineWidget->ScrubPosition(mFlipbookWrapper->GetKeyframeStartPosition(iIndex) + 0.5f);
}

void
SOdysseyFlipbookTimelineView::AddFrame()
{
    AddFrame(mFlipbookWrapper->Flipbook()->GetNumKeyFrames());
}

void
SOdysseyFlipbookTimelineView::AddFrameAfter()
{
    int32 index = GetCurrentKeyframeIndex();
    if (index < 0 || index >= mFlipbookWrapper->Flipbook()->GetNumKeyFrames())
    {
        if (GetScrubPosition() < 0)
        {
            AddFrame(0);
        }
        else
        {
            AddFrame(mFlipbookWrapper->Flipbook()->GetNumKeyFrames());
        }
        return;
    }
    
    AddFrame(index + 1);
}

void
SOdysseyFlipbookTimelineView::AddFrameBefore()
{
    int32 index = GetCurrentKeyframeIndex();
    if (index < 0 || index >= mFlipbookWrapper->Flipbook()->GetNumKeyFrames())
    {
        if (GetScrubPosition() < 0)
        {
            AddFrame(0);
        }
        else
        {
            AddFrame(mFlipbookWrapper->Flipbook()->GetNumKeyFrames());
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
	mTimelineWidget->ScrubPosition(mFlipbookWrapper->Flipbook()->GetNumFrames() - 0.5f);
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
    UPaperFlipbook* flipbook = mFlipbookWrapper->Flipbook();
    int32 index = mFlipbookWrapper->GetKeyframeIndexAtPosition(mTimelineWidget->ScrubPosition());
    if (index >= flipbook->GetNumKeyFrames() - 1)
        return;

    if (index < 0)
    {
        if (mTimelineWidget->ScrubPosition() >= flipbook->GetNumFrames())
        {
            float position = mFlipbookWrapper->GetKeyframeStartPosition(flipbook->GetNumKeyFrames() - 1);
            mTimelineWidget->ScrubPosition(position + 0.5f);
        }
        else
        {
            float position = mFlipbookWrapper->GetKeyframeStartPosition(0);
            mTimelineWidget->ScrubPosition(position + 0.5f);
        }
        return;
    }

    float position = mFlipbookWrapper->GetKeyframeStartPosition(index + 1);
    mTimelineWidget->ScrubPosition(position + 0.5f);
}

void
SOdysseyFlipbookTimelineView::ScrubToPreviousKeyFrame()
{
    int32 index = mFlipbookWrapper->GetKeyframeIndexAtPosition(mTimelineWidget->ScrubPosition());
    if (index == 0)
        return;
        
    if (index < 0)
    {
        if (mTimelineWidget->ScrubPosition() >= mFlipbookWrapper->Flipbook()->GetNumFrames())
        {
            float position = mFlipbookWrapper->GetKeyframeStartPosition(mFlipbookWrapper->Flipbook()->GetNumKeyFrames() - 1);
            mTimelineWidget->ScrubPosition(position + 0.5f);
        }
        else
        {
            float position = mFlipbookWrapper->GetKeyframeStartPosition(0);
            mTimelineWidget->ScrubPosition(position + 0.5f);
        }
        return;
    }

    float position = mFlipbookWrapper->GetKeyframeStartPosition(index - 1);
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
    int32 index = GetCurrentKeyframeIndex();
    if (index < 0 || index >= mFlipbookWrapper->Flipbook()->GetNumKeyFrames())
        return EVisibility::Collapsed;

    return mFlipbookWrapper->GetKeyframeTexture(index) ? EVisibility::Collapsed : EVisibility::Visible;
}

TArray<SNumericDropDown<float>::FNamedValue>
SOdysseyFlipbookTimelineView::FrameRateDropDownValues() const
{
    TArray<SNumericDropDown<float>::FNamedValue> values;
    values.Add(SNumericDropDown<float>::FNamedValue(1.0f, LOCTEXT("1Fps", "1 FPS"), LOCTEXT("1FpsDescription", "1 frame per second")));
    values.Add(SNumericDropDown<float>::FNamedValue(2.0f, LOCTEXT("2Fps", "2 FPS"), LOCTEXT("2FpsDescription", "2 frames per second")));
    values.Add(SNumericDropDown<float>::FNamedValue(3.0f, LOCTEXT("3Fps", "3 FPS"), LOCTEXT("3FpsDescription", "3 frames per second")));
    values.Add(SNumericDropDown<float>::FNamedValue(6.0f, LOCTEXT("6Fps", "6 FPS"), LOCTEXT("6FpsDescription", "6 frames per second")));
    values.Add(SNumericDropDown<float>::FNamedValue(12.0f, LOCTEXT("12Fps", "12 FPS"), LOCTEXT("12FpsDescription", "12 frames per second")));
    values.Add(SNumericDropDown<float>::FNamedValue(24.0f, LOCTEXT("24Fps", "24 FPS"), LOCTEXT("24FpsDescription", "24 frames per second")));
    values.Add(SNumericDropDown<float>::FNamedValue(29.97f, LOCTEXT("29.97Fps", "29.97 FPS"), LOCTEXT("29.97FpsDescription", "29.97 frames per second")));
    values.Add(SNumericDropDown<float>::FNamedValue(30.0f, LOCTEXT("30Fps", "30 FPS"), LOCTEXT("30FpsDescription", "30 frames per second")));
    values.Add(SNumericDropDown<float>::FNamedValue(60.0f, LOCTEXT("60Fps", "60 FPS"), LOCTEXT("60FpsDescription", "60 frames per second")));

    return values;
}

float
SOdysseyFlipbookTimelineView::GetFrameRate() const
{
    return mFlipbookWrapper->Flipbook()->GetFramesPerSecond();
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
    iCommandList->MapAction(FOdysseyFlipbookEditorCommands::Get().PlayForward, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::Play, false)));
    iCommandList->MapAction(FOdysseyFlipbookEditorCommands::Get().PlayBackward, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::Play, true)));
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
