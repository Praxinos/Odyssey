// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Math/Color.h"
#include "Misc/FrameNumber.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class FPaintArgs;
class FSlateWindowElementList;
class ISequencer;
class FSequencerKeyCollection;

class SStoryboardTransportRange : public SCompoundWidget
{
public:

    SLATE_BEGIN_ARGS(SStoryboardTransportRange){}
    SLATE_END_ARGS()

    /** Construct this widget */
    void Construct(const FArguments& InArgs);

    /** Assign a new sequencer to this transport */
    void SetSequencer(TWeakPtr<ISequencer> InSequencer);

    virtual FVector2D ComputeDesiredSize(float) const override;
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual void OnMouseCaptureLost(const FCaptureLostEvent& CaptureLostEvent) override;

private:

    void SetTime(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

    ISequencer* GetSequencer() const;

    void DrawKeys(const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements, int32 LayerId, bool bParentEnabled, const TArrayView<const FFrameNumber>& Keys, const TArrayView<const FLinearColor>& KeyColors, bool& bOutPlayMarkerOnKey) const;

    /** The sequencer that we're controlling */
    TWeakPtr<ISequencer> WeakSequencer;

    /** The collection of keys for the currently active sequencer selection */
    TUniquePtr<FSequencerKeyCollection> ActiveKeyCollection;

    bool bDraggingTime;
};
