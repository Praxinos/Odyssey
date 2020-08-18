// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

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
        SLATE_ARGUMENT( UPaperFlipbook*, Flipbook )
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

private:
    // The playback functions
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
    //Play/Stop Methods
    void Play();
    void Stop();

public:
    //In Frames
    float GetScrubPosition() const;

    //The keyframe index at Scrub position
    int GetCurrentKeyframeIndex() const;
    int GetKeyframeIndexAtScrubPosition(float iPosition) const;
    float GetScrubPositionForKeyFrameIndex(int32 iIndex) const;

    bool IsLooping() const;
    bool IsPlaying() const;
    bool IsScrubbing() const;

    float FrameSize() const;

    // Deprecated Accessors 

private:
    // Event callbacks

    //SOdysseyPlaybackControls
	FReply OnPlayClicked();
	FReply OnPauseClicked();
	FReply OnBeginningClicked();
	FReply OnEndClicked();
	FReply OnPreviousClicked();
	FReply OnNextClicked();
	FReply OnLoopClicked();
	FReply OnAddFrameClicked();
    FReply OnFixCurrentFrameClicked();

    void OnScrubStarted();
	void OnScrubPositionChanged(float iOldPosition);

	void OnFrameRateChanged(float iFrameRate);

private:
    //Manipulations
	bool AddFrame();
	bool FixFrame(int32 iIndex);
	UTexture2D* CreateTexture(int32 width, int32 height);
    UPaperSprite* CreateSprite(UTexture2D* iTexture);

private:
    TArray<SNumericDropDown<float>::FNamedValue> FrameRateDropDownValues() const;
    float GetFrameRate() const;

private:
	//Slate
	EVisibility FixCurrentFrameVisibility() const;

private:
    UPaperFlipbook* mFlipbook;

    bool mIsPlaying;
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
