// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyFlipbookTimelineView.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Texture2D.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Widgets/Input/SSlider.h"

#include "SOdysseyFlipbookPlaybackControls.h"
#include "SOdysseyFlipbookTimeline.h"
#include "SOdysseyFlipbookTimelineTrack.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyPainterEditorFlipbookListener.h"
#include "OdysseyPainterEditorFlipbookUtils.h"
#include "PaperSprite.h"
#include "OdysseyPainterEditorFlipbookCommands.h"

#define LOCTEXT_NAMESPACE "FlipbookEditor"
#define MaxZoom 1.0
#define MinZoom 0.01
#define ZoomStep 0.01

//CONSTRUCTION/DESTRUCTION-----------------------------------------------

SLATE_IMPLEMENT_WIDGET(SOdysseyFlipbookTimelineView)
void SOdysseyFlipbookTimelineView::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
    SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, mFlipbook, EInvalidateWidgetReason::None)
    .OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda(
        [](SWidget& Widget)
        {
            static_cast<SOdysseyFlipbookTimelineView&>(Widget).Rebuild();
        }
    ));
}

SOdysseyFlipbookTimelineView::~SOdysseyFlipbookTimelineView()
{
}

SOdysseyFlipbookTimelineView::SOdysseyFlipbookTimelineView()
    : mFlipbook(*this, nullptr)
{
}


void SOdysseyFlipbookTimelineView::Construct(const FArguments& InArgs)
{
    mFlipbook.Assign(*this, InArgs._Flipbook);
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
    mOnKeyframeRemoved = InArgs._OnKeyframeRemoved;

    Rebuild();
}

void
SOdysseyFlipbookTimelineView::Rebuild()
{
    ChildSlot.DetachWidget();

    if (!mFlipbook.Get())
        return;

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

    ChildSlot.AttachWidget(
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
            SAssignNew(mTimelineWidget, SOdysseyFlipbookTimeline)
            .OnScrubStarted(this, &SOdysseyFlipbookTimelineView::OnScrubStarted)
            .OnScrubPositionChanged(this, &SOdysseyFlipbookTimelineView::OnScrubPositionChanged)
            .OnScrubStopped(mOnScrubStopped)
            [
                SAssignNew(mTimelineTrackWidget, SOdysseyFlipbookTimelineTrack)
                .Flipbook(mFlipbook.Get())
                .TextureConfiguration(this, &SOdysseyFlipbookTimelineView::GetTextureConfiguration)
                .FrameSize(this, &SOdysseyFlipbookTimelineView::FrameSize)
                .OnFlipbookChanged(mOnFlipbookChanged)
                .OnKeyframeRemoved(mOnKeyframeRemoved)
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
    );
}

FOdysseyTextureConfiguration
SOdysseyFlipbookTimelineView::GetTextureConfiguration() const
{
    return mTextureConfiguration;
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
    if (!mFlipbook.Get())
        return 0;

    return OdysseyPainterEditorFlipbookUtils::GetKeyframeIndexAtPosition(mFlipbook.Get(), GetScrubPosition());
}

void
SOdysseyFlipbookTimelineView::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

    if (!mIsPlaying)
        return;

    float scrubPosition = mTimelineWidget->ScrubPosition();
    float offset = mIsPlayingBackward ? -InDeltaTime * mFlipbook.Get()->GetFramesPerSecond() : InDeltaTime * mFlipbook.Get()->GetFramesPerSecond();
    scrubPosition += offset;
    if (mIsLooping)
    {
        if (mIsPlayingBackward)
        {
            if (scrubPosition < 0)
            {
                scrubPosition += mFlipbook.Get()->GetNumFrames();
            }
        }
        else
        {
            scrubPosition = FGenericPlatformMath::Fmod(scrubPosition, mFlipbook.Get()->GetNumFrames());
        }

    }
    else if (!mIsPlayingBackward && scrubPosition >= mFlipbook.Get()->GetNumFrames())
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

    int32 previousFrame = OdysseyPainterEditorFlipbookUtils::GetKeyframeIndexAtPosition(mFlipbook.Get(), iOldPosition);
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
    if (!mFlipbook.Get())
        return;

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
    if (!mFlipbook.Get())
        return;

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
    if (!mFlipbook.Get())
        return;

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
    if (!mTimelineWidget)
        return false;

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

    mOnTextureCreated.ExecuteIfBound(texture, mTextureConfiguration);
}

void
SOdysseyFlipbookTimelineView::OnFrameRateChanged(float iFrameRate)
{
    //Using a mutator is mandatory to change the framerate of the flipbook
    FScopedFlipbookMutator mutator(mFlipbook.Get());
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
    if (!OdysseyPainterEditorFlipbookUtils::FixKeyFrame(mFlipbook.Get(), iIndex, &createdTexture, &createdSprite, mTextureConfiguration))
        return false;

    if (createdSprite)
        mOnSpriteCreated.ExecuteIfBound(createdSprite);

    if (createdTexture)
    {
        mOnTextureCreated.ExecuteIfBound(createdTexture, mTextureConfiguration);
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
    if (!OdysseyPainterEditorFlipbookUtils::CreateKeyFrame(mFlipbook.Get(), iIndex, &createdTexture, &createdSprite, mTextureConfiguration))
        return;

    mTimelineTrackWidget->InsertFrame(iIndex, createdTexture, 1);

    mOnSpriteCreated.ExecuteIfBound(createdSprite);
    mOnTextureCreated.ExecuteIfBound(createdTexture, mTextureConfiguration);
    mOnFlipbookChanged.ExecuteIfBound();

    mTimelineWidget->ScrubPosition(OdysseyPainterEditorFlipbookUtils::GetKeyframeStartPosition(mFlipbook.Get(), iIndex) + 0.5f);
}

void
SOdysseyFlipbookTimelineView::AddFrame()
{
    if (!mFlipbook.Get())
        return;

    AddFrame(mFlipbook.Get()->GetNumKeyFrames());
}

void
SOdysseyFlipbookTimelineView::AddFrameAfter()
{
    if (!mFlipbook.Get())
        return;

    int32 index = GetCurrentKeyframeIndex();
    if (index < 0 || index >= mFlipbook.Get()->GetNumKeyFrames())
    {
        if (GetScrubPosition() < 0)
        {
            AddFrame(0);
        }
        else
        {
            AddFrame(mFlipbook.Get()->GetNumKeyFrames());
        }
        return;
    }

    AddFrame(index + 1);
}

void
SOdysseyFlipbookTimelineView::AddFrameBefore()
{
    if (!mFlipbook.Get())
        return;

    int32 index = GetCurrentKeyframeIndex();
    if (index < 0 || index >= mFlipbook.Get()->GetNumKeyFrames())
    {
        if (GetScrubPosition() < 0)
        {
            AddFrame(0);
        }
        else
        {
            AddFrame(mFlipbook.Get()->GetNumKeyFrames());
        }
        return;
    }
    AddFrame(index);
}

void
SOdysseyFlipbookTimelineView::ScrubToFirstFrame()
{
    if (!mFlipbook.Get())
        return;

    mTimelineWidget->ScrubPosition(0.5f);
}

void
SOdysseyFlipbookTimelineView::ScrubToLastFrame()
{
    if (!mFlipbook.Get())
        return;

    mTimelineWidget->ScrubPosition(mFlipbook.Get()->GetNumFrames() - 0.5f);
}

void
SOdysseyFlipbookTimelineView::ScrubToNextFrame()
{
    if (!mFlipbook.Get())
        return;

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
    if (!mFlipbook.Get())
        return;

    if (mTimelineWidget->ScrubPosition() < 1.0f)
        return;

    mTimelineWidget->ScrubPosition(FGenericPlatformMath::FloorToFloat(mTimelineWidget->ScrubPosition() - 1.0f) + 0.5f);
}

void
SOdysseyFlipbookTimelineView::ScrubToNextKeyFrame()
{
    if (!mFlipbook.Get())
        return;

    int32 index = OdysseyPainterEditorFlipbookUtils::GetKeyframeIndexAtPosition(mFlipbook.Get(), mTimelineWidget->ScrubPosition());
    if (index >= mFlipbook.Get()->GetNumKeyFrames() - 1)
        return;

    if (index < 0)
    {
        if (mTimelineWidget->ScrubPosition() >= mFlipbook.Get()->GetNumFrames())
        {
            float position = OdysseyPainterEditorFlipbookUtils::GetKeyframeStartPosition(mFlipbook.Get(), mFlipbook.Get()->GetNumKeyFrames() - 1);
            mTimelineWidget->ScrubPosition(position + 0.5f);
        }
        else
        {
            float position = OdysseyPainterEditorFlipbookUtils::GetKeyframeStartPosition(mFlipbook.Get(), 0);
            mTimelineWidget->ScrubPosition(position + 0.5f);
        }
        return;
    }

    float position = OdysseyPainterEditorFlipbookUtils::GetKeyframeStartPosition(mFlipbook.Get(), index + 1);
    mTimelineWidget->ScrubPosition(position + 0.5f);
}

void
SOdysseyFlipbookTimelineView::ScrubToPreviousKeyFrame()
{
    if (!mFlipbook.Get())
        return;

    int32 index = OdysseyPainterEditorFlipbookUtils::GetKeyframeIndexAtPosition(mFlipbook.Get(), mTimelineWidget->ScrubPosition());
    if (index == 0)
        return;

    if (index < 0)
    {
        if (mTimelineWidget->ScrubPosition() >= mFlipbook.Get()->GetNumFrames())
        {
            float position = OdysseyPainterEditorFlipbookUtils::GetKeyframeStartPosition(mFlipbook.Get(), mFlipbook.Get()->GetNumKeyFrames() - 1);
            mTimelineWidget->ScrubPosition(position + 0.5f);
        }
        else
        {
            float position = OdysseyPainterEditorFlipbookUtils::GetKeyframeStartPosition(mFlipbook.Get(), 0);
            mTimelineWidget->ScrubPosition(position + 0.5f);
        }
        return;
    }

    float position = OdysseyPainterEditorFlipbookUtils::GetKeyframeStartPosition(mFlipbook.Get(), index - 1);
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
    if (!mFlipbook.Get())
        return EVisibility::Collapsed;

    int32 index = GetCurrentKeyframeIndex();
    if (index < 0 || index >= mFlipbook.Get()->GetNumKeyFrames())
        return EVisibility::Collapsed;

    return OdysseyPainterEditorFlipbookUtils::GetKeyframeTexture(mFlipbook.Get(), index) ? EVisibility::Collapsed : EVisibility::Visible;
}

TArray<SNumericDropDown<float>::FNamedValue>
SOdysseyFlipbookTimelineView::FrameRateDropDownValues() const
{
    TArray<SNumericDropDown<float>::FNamedValue> values;
    FText nameFormat = LOCTEXT("timeline.framerate.n-fps.name", "{0} FPS");
    FText tooltipFormat = LOCTEXT("timeline.framerate.n-fps.tooltip", "{0} frame(s) per second");

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
    if (!mFlipbook.Get())
        return 0.f;

    return mFlipbook.Get()->GetFramesPerSecond();
}

void
SOdysseyFlipbookTimelineView::OnSpriteCreated(UPaperSprite* iSprite)
{
    mOnSpriteCreated.ExecuteIfBound(iSprite);
}

void
SOdysseyFlipbookTimelineView::OnTextureCreated(UTexture2D* iTexture, FOdysseyTextureConfiguration iTextureConfiguration)
{
    mOnTextureCreated.ExecuteIfBound(iTexture, iTextureConfiguration);
}

void
SOdysseyFlipbookTimelineView::BindCommands(const TSharedRef<FUICommandList>& iCommandList)
{
    iCommandList->MapAction(FOdysseyPainterEditorFlipbookCommands::Get().AddNewKeyFrame, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::AddFrame)));
    iCommandList->MapAction(FOdysseyPainterEditorFlipbookCommands::Get().AddNewKeyFrameAfter, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::AddFrameAfter)));
    iCommandList->MapAction(FOdysseyPainterEditorFlipbookCommands::Get().AddNewKeyFrameBefore, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::AddFrameBefore)));
    iCommandList->MapAction(FOdysseyPainterEditorFlipbookCommands::Get().PlayForward, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::PlayOrPause, false)));
    iCommandList->MapAction(FOdysseyPainterEditorFlipbookCommands::Get().PlayBackward, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::PlayOrPause, true)));
    iCommandList->MapAction(FOdysseyPainterEditorFlipbookCommands::Get().Pause, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::Pause)));
    iCommandList->MapAction(FOdysseyPainterEditorFlipbookCommands::Get().Stop, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::Stop)));
    iCommandList->MapAction(FOdysseyPainterEditorFlipbookCommands::Get().PreviousFrame, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::ScrubToPreviousFrame)));
    iCommandList->MapAction(FOdysseyPainterEditorFlipbookCommands::Get().NextFrame, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::ScrubToNextFrame)));
    iCommandList->MapAction(FOdysseyPainterEditorFlipbookCommands::Get().PreviousKeyFrame, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::ScrubToPreviousKeyFrame)));
    iCommandList->MapAction(FOdysseyPainterEditorFlipbookCommands::Get().NextKeyFrame, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::ScrubToNextKeyFrame)));
    iCommandList->MapAction(FOdysseyPainterEditorFlipbookCommands::Get().FirstFrame, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::ScrubToFirstFrame)));
    iCommandList->MapAction(FOdysseyPainterEditorFlipbookCommands::Get().LastFrame, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::ScrubToLastFrame)));
    iCommandList->MapAction(FOdysseyPainterEditorFlipbookCommands::Get().ToggleLooping, FUIAction( FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineView::ToggleLooping)));
}

#undef LOCTEXT_NAMESPACE
