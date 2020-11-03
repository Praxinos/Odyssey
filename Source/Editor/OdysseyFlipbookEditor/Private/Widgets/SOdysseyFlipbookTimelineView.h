// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "SOdysseyFlipbookTimelineViewEvents.h"
#include "Widgets/Input/SNumericDropDown.h"

// Called when the selection changes

class SOdysseyTimeline;
class SOdysseyFlipbookTimelineTrack;
class UTexture2D;
class UPaperSprite;
/**
 * Implements the Timeline widget
 */
class SOdysseyFlipbookTimelineView : public SCompoundWidget
{
public:

    SLATE_BEGIN_ARGS(SOdysseyFlipbookTimelineView)
        {}
        /** Called to populate the add combo button in the toolbar. */
        // SLATE_EVENT( FOnGetAddMenuContent, OnGetAddMenuContent )
        /** Extender to use for the add menu. */
        SLATE_ARGUMENT( TSharedPtr<FOdysseyFlipbookWrapper>, FlipbookWrapper )
        SLATE_EVENT(FOnCurrentKeyframeChanged, OnCurrentKeyframeChanged)
        SLATE_EVENT(FOnPlayStarted, OnPlayStarted)
        SLATE_EVENT(FOnPlayStopped, OnPlayStopped)
        SLATE_EVENT(FOnScrubStarted, OnScrubStarted)
        SLATE_EVENT(FOnScrubStopped, OnScrubStopped)
        SLATE_EVENT(FOnScrubPositionChanged, OnScrubPositionChanged)
		//SLATE_EVENT(FOnStructureChanged, OnStructureChanged)
        SLATE_EVENT(FOnFlipbookChanged, OnFlipbookChanged)
		//SLATE_EVENT(FOnFramesEditStop, OnFramesEditStop)
		SLATE_EVENT( FOnSpriteCreated, OnSpriteCreated)
		SLATE_EVENT( FOnTextureCreated, OnTextureCreated)
        SLATE_EVENT( FOnKeyframeRemoved, OnKeyframeRemoved)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyFlipbookTimelineView();
    void Construct(const FArguments& InArgs);
	void BindCommands(const TSharedRef<FUICommandList>& iCommandList);

private:
    // The playback functions
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
    //Play/Stop Methods
    void Play(bool iBackward);
    void Pause();
    void Stop();

public:
    //In Frames
    float GetScrubPosition() const;

    //The keyframe index at Scrub position
    int GetCurrentKeyframeIndex() const;

    bool IsLooping() const;
    bool IsPlaying() const;
    bool IsPlayingBackward() const;
    bool IsScrubbing() const;

    float FrameSize() const;

    TSharedPtr<SWidget> OnArrowNavigation(EUINavigation iNavigation);

private:
    // Event callbacks

    //SOdysseyPlaybackControls
	FReply OnPlayClicked(bool iBackward);
	FReply OnPauseClicked();
    FReply OnStopClicked();
	FReply OnBeginningClicked();
	FReply OnEndClicked();
	FReply OnPreviousClicked();
	FReply OnNextClicked();
	FReply OnPreviousKeyClicked();
	FReply OnNextKeyClicked();
	FReply OnLoopClicked();
	FReply OnAddFrameClicked();
    FReply OnFixCurrentFrameClicked();

    void OnScrubStarted();
	void OnScrubPositionChanged(float iOldPosition);

	void OnFrameRateChanged(float iFrameRate);
    void OnKeyframeAdded(FPaperFlipbookKeyFrame& iKeyFrame);

    void  OnZoomMenuEntryClicked( double ZoomValue );
    FText OnZoomPercentageText( ) const;
    void  OnZoomSliderChanged( float NewValue );
    float ZoomSliderValue() const;

    void OnSpriteCreated(UPaperSprite* iSprite);
    void OnTextureCreated(UTexture2D* iTexture);

private:
    //Manipulations
	void AddFrame(int32 iIndex);
    void AddFrame();
    void AddFrameAfter();
    void AddFrameBefore();
	bool FixFrame(int32 iIndex);

    //Timeline
    void ScrubToFirstFrame();
    void ScrubToLastFrame();
    void ScrubToNextFrame();
    void ScrubToPreviousFrame();
    void ScrubToNextKeyFrame();
    void ScrubToPreviousKeyFrame();
    void ToggleLooping();

private:
    TArray<SNumericDropDown<float>::FNamedValue> FrameRateDropDownValues() const;
    float GetFrameRate() const;

private:
	//Slate
	EVisibility FixCurrentFrameVisibility() const;

private:
    TSharedPtr<FOdysseyFlipbookWrapper> mFlipbookWrapper;

    bool mIsPlaying;
    bool mIsPlayingBackward;
    bool mIsLooping;

    float mScrubPositionBeforePlay;

    //Widgets
    TSharedPtr<SOdysseyTimeline> mTimelineWidget;
	TSharedPtr<SOdysseyFlipbookTimelineTrack> mTimelineTrackWidget;
    
    //Events
    FOnCurrentKeyframeChanged mOnCurrentKeyframeChanged;
    FOnPlayStarted mOnPlayStarted;
    FOnPlayStopped mOnPlayStopped;
    FOnScrubStarted mOnScrubStarted;
	FOnScrubStopped mOnScrubStopped;
	FOnScrubPositionChanged mOnScrubPositionChanged;
	//FOnStructureChanged mOnStructureChanged;
    FOnFlipbookChanged mOnFlipbookChanged;
	//FOnFramesEditStop mOnFramesEditStop;
	FOnSpriteCreated mOnSpriteCreated;
	FOnTextureCreated mOnTextureCreated;
};
