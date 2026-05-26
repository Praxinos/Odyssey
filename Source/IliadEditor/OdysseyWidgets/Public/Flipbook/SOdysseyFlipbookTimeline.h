// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Layout/SScrollBox.h"

#include "SOdysseyFlipbookTimelineEvents.h"

class SOdysseyScrubWidget;

class ODYSSEYWIDGETS_API SOdysseyFlipbookTimeline : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyFlipbookTimeline)
        : _ScrubPosition(0.0f)
        , _Offset(0.0f)
        , _Zoom(1.0f)
    {}
        SLATE_DEFAULT_SLOT( FArguments, Content )
        SLATE_ARGUMENT(float, ScrubPosition)
        SLATE_ARGUMENT(float, Offset)
        SLATE_ARGUMENT(float, Zoom)
        SLATE_EVENT(FOnScrubStarted, OnScrubStarted)
        SLATE_EVENT(FOnScrubPositionChanged, OnScrubPositionChanged)
        SLATE_EVENT(FOnScrubStopped, OnScrubStopped)
        SLATE_EVENT(FOnOffsetChanged, OnOffsetChanged)
        SLATE_EVENT(FOnZoomChanged, OnZoomChanged)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    // SWidget interface
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
    virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    // End of SWidget interface

public:
    void OnScrollBarUserScrolled(float ScrollOffset);

public:
    // 1.0 = 100%
    float Zoom() const;
    void Zoom(float iZoom);

    // Offset of the timeline in frames
    float Offset() const;
    void Offset(float iOffset);

    // ScrubPosition
    float ScrubPosition() const;
    void ScrubPosition(float iPosition);

    bool IsScrubbing() const;

    // Returns the framesize in pixels according to the zoom
    float FrameSize() const;

    FOptionalSize GetScrollBoxHWidth() const;
    FOptionalSize GetScrollBoxVHeight() const;

private:
    //Zoom
    float             mZoom;
    FOnZoomChanged     mOnZoomChanged;

    //Offset
    float                 mOffset;
    FVector2D             mOffsetMousePosition;
    bool                 mIsOffsetting;
    FOnOffsetChanged     mOnOffsetChanged;

    //Scrub
    float                         mScrubPosition;
    bool                         mIsScrubbing;
    FOnScrubStarted             mOnScrubStarted;
    FOnScrubPositionChanged     mOnScrubPositionChanged;
    FOnScrubStopped             mOnScrubStopped;

    TSharedPtr<SWidget> mContent;
    TSharedPtr<SScrollBar> mScrollBarH;
    TSharedPtr<SScrollBox> mScrollBoxH;
    TSharedPtr<SScrollBar> mScrollBarV;
    TSharedPtr<SScrollBox> mScrollBoxV;
};
