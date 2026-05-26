// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UOdysseyAnimationLayer;

//////////////////////////////////////////////////////////////////////////
// SOdysseyAnimationTimelineCellNamesHeader
class SOdysseyAnimationTimelineCellNamesHeader
    : public SCompoundWidget
{
public:
    SOdysseyAnimationTimelineCellNamesHeader();

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineCellNamesHeader)
        : _Layer(nullptr)
        {}
        SLATE_ARGUMENT(UOdysseyAnimationLayer*, Layer)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    TSharedRef< SWidget > OnOptionsGetMenuContent();

private:
    UOdysseyAnimationLayer* mLayer;
};
