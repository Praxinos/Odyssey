// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LayerStack/SOdysseyAnimationLayerStack.h"

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineSection
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineSection)
        : _HAlign(HAlign_Fill)
        , _VAlign(VAlign_Fill)
    {}
        SLATE_DEFAULT_SLOT( FArguments, Content )
        SLATE_ATTRIBUTE(float, WidthInFrames)
        SLATE_ARGUMENT(TSharedPtr<FOdysseyAnimationEditorTimelinePosition>, TimelinePosition)
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
    TSharedPtr<FOdysseyAnimationEditorTimelinePosition> mTimelinePosition;
    TAttribute<float> mWidthInFrames;
    TAttribute<float> mHeightInScreenUnits;
    TSharedPtr<SBox> mBox;
};
