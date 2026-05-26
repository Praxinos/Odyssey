// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UOdysseyAnimation;

class SOdysseyAnimationDetails : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationDetails)
    {}
        SLATE_ATTRIBUTE(UOdysseyAnimation*, Animation)
    SLATE_END_ARGS()


    void Construct(const FArguments& InArgs);

private:
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
    TAttribute<UOdysseyAnimation*> mAnimation;
    UOdysseyAnimation* mCurrentAnimation;
    TSharedPtr<class IDetailsView> mAnimationPropertiesWidget;
};
