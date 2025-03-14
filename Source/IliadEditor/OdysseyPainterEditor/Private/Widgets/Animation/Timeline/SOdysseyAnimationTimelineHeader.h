// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"

class UOdysseyAnimation;
class UOdysseyAnimationPlayer;
class FOdysseyPainterEditorAnimationTimelinePosition;

class SOdysseyAnimationTimelineHeader
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineHeader)
        : _Animation(nullptr)
        , _Player(nullptr)
    {}
        SLATE_ARGUMENT( UOdysseyAnimation*, Animation )
        SLATE_ARGUMENT( UOdysseyAnimationPlayer*, Player )
        SLATE_ARGUMENT( TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>, TimelinePosition )
        SLATE_EVENT(FSimpleDelegate, OnScrubStart)
        SLATE_EVENT(FSimpleDelegate, OnScrubEnd)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    // SWidget interface
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    // End of SWidget interfacepublic:

private:
    float MousePositionToFrame(float iX) const;
    float FrameToMousePosition(float iFrame) const;

    void OnLeftHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void OnLeftHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void OnLeftHandleStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void OnRightHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void OnRightHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void OnRightHandleStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void BuildContextMenu(FMenuBuilder& iMenuBuilder);
    void MapContextMenuActions(TSharedPtr<FUICommandList> iCommandList);

    void OnAnimationLeftBoundValueCommitted(int iValue, ETextCommit::Type iType);
    void OnAnimationLeftBoundValueChanged(int iValue);
    void OnAnimationLeftBoundBeginSliderMovement();
    void OnAnimationLeftBoundEndSliderMovement(int iValue);
    void OnAnimationRightBoundValueCommitted(int iValue, ETextCommit::Type iType);
    void OnAnimationRightBoundValueChanged(int iValue);
    void OnAnimationRightBoundBeginSliderMovement();
    void OnAnimationRightBoundEndSliderMovement(int iValue);

private:
    UOdysseyAnimation* mAnimation;
    UOdysseyAnimationPlayer* mPlayer;
    TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition> mTimelinePosition;

    bool mIsScrubbing = false;
    FSimpleDelegate mOnScrubStart;

    FSimpleDelegate mOnScrubEnd;

    double mHandleMousePosition;
    int mInitialLeftBound;
    int mInitialRightBound;
};
