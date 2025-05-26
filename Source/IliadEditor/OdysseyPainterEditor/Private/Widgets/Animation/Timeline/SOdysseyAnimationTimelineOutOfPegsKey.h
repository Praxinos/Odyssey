// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyLighttable.h"
#include "Widgets/SCompoundWidget.h"

class UOdysseyAnimationLayer;
class UOdysseyAnimationCell;
class FOdysseyPainterEditorAnimationTimelinePosition;

class SOdysseyAnimationTimelineOutOfPegsKey
    : public SCompoundWidget
{
public:
    DECLARE_DELEGATE_OneParam(FOnKeyChanged, FOdysseyLighttableKey)
    DECLARE_DELEGATE_OneParam(FOnActivateOutOfPegs, UOdysseyAnimationCell*)
    DECLARE_DELEGATE_RetVal_OneParam(ECheckBoxState, FOnIsOutOfPegsChecked, UOdysseyAnimationCell*)

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineOutOfPegsKey)
    {}
        SLATE_ARGUMENT(TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>, TimelinePosition)
        SLATE_ATTRIBUTE(UOdysseyAnimationCell*, Cell)
        SLATE_ATTRIBUTE(FOdysseyLighttableKey, Key)
        SLATE_EVENT(FOnActivateOutOfPegs, OnActivateOutOfPegs)
        SLATE_EVENT(FSimpleDelegate, OnInactivateOutOfPegs)
        SLATE_EVENT(FOnIsOutOfPegsChecked, OnIsOutOfPegsChecked)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    bool IsOutOfPegsEnabled() const;
    const FSlateBrush* GetOutOfPegsButtonImage() const;
    void OnOutOfPegsCheckStateChanged(ECheckBoxState iValue);
    ECheckBoxState IsOutOfPegsChecked() const;

private:
    TAttribute<UOdysseyAnimationCell*> mCell;
    TAttribute<FOdysseyLighttableKey> mKey;

    FOnActivateOutOfPegs mOnActivateOutOfPegs;
    FSimpleDelegate mOnInactivateOutOfPegs;
    FOnIsOutOfPegsChecked mOnIsOutOfPegsChecked;
};
