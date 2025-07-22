// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#pragma once

#include "CoreMinimal.h"
#include "TimelineTools/OdysseyAnimationTimelineTools.h"

class SOdysseyAnimationTimelineToolSelector
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineToolSelector)
    {}
    SLATE_END_ARGS()

public:
    void Construct(const FArguments& iArgs );

private:
    EOdysseyTimelineTool GetCurrentTool() const;
    void OnToolChecked(EOdysseyTimelineTool iTool, ECheckBoxState iState);
};
