// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LayerStack/SOdysseyAnimationLayerStack.h"

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineSection
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineSection)
        {}
        SLATE_ATTRIBUTE(float, WidthInFrames)
        SLATE_ATTRIBUTE(float, HeightInScreenUnits)
    SLATE_END_ARGS()

public:
    void Construct(const FArguments& iArgs, TSharedPtr<SOdysseyAnimationLayerStack> iLayerStackWidget);

public:
    TSharedPtr<SOdysseyAnimationLayerStack> GetLayerStackWidget() const;
	void SetContent(TSharedPtr<SWidget> iContent);

private:
    virtual FOptionalSize GetSectionWidth() const;
    virtual FOptionalSize GetSectionHeight() const;

private:
    TAttribute<float> mWidthInFrames;
    TAttribute<float> mHeightInScreenUnits;
    TSharedPtr<SBox> mBox;
    TWeakPtr<SOdysseyAnimationLayerStack> mLayerStackWidget;
};