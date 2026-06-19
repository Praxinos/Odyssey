// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "SOdysseyAnimationLayerStack.h"

class SOdysseyAnimationTimelineSection
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineSection)
        : _HAlign(HAlign_Fill)
        , _VAlign(VAlign_Fill)
    {
        _Visibility = EVisibility::SelfHitTestInvisible;
    }
        SLATE_DEFAULT_SLOT( FArguments, Content )
        SLATE_ATTRIBUTE(float, WidthInFrames)
        SLATE_ARGUMENT(TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>, TimelinePosition)
        SLATE_ARGUMENT(EHorizontalAlignment, HAlign)
        SLATE_ARGUMENT(EVerticalAlignment, VAlign)
    SLATE_END_ARGS()

public:
    void Construct(const FArguments& iArgs );

public:
    void SetContent(TSharedPtr<SWidget> iContent);

private:
    virtual FOptionalSize GetSectionWidth() const;

private:
    TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition> mTimelinePosition;
    TAttribute<float> mWidthInFrames;
    TAttribute<float> mHeightInScreenUnits;
    TSharedPtr<SBox> mBox;
};
