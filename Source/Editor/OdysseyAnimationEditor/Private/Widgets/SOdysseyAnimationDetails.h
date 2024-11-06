// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class UOdysseyAnimation;

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationDetails : public SCompoundWidget
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
