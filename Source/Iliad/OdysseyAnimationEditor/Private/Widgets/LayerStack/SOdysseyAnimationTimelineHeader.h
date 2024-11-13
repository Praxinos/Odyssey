// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class UOdysseyAnimation;
class UOdysseyAnimationPlayer;
class FOdysseyAnimationEditorTimelinePosition;

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineHeader
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineHeader)
        : _Animation(nullptr)
        , _Player(nullptr)
    {}
        SLATE_ARGUMENT( UOdysseyAnimation*, Animation )
        SLATE_ARGUMENT( UOdysseyAnimationPlayer*, Player )
        SLATE_ARGUMENT( TSharedPtr<FOdysseyAnimationEditorTimelinePosition>, TimelinePosition )
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    // SWidget interface
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    // End of SWidget interfacepublic:

private:
    UOdysseyAnimation* mAnimation;
    UOdysseyAnimationPlayer* mPlayer;
    TSharedPtr<FOdysseyAnimationEditorTimelinePosition> mTimelinePosition;

    bool mIsScrubbing = false;
};
