// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "LayerStack/Tools/OdysseyAnimationTimelineTools.h"

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineToolSelector
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