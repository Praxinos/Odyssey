// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "Widgets/SCompoundWidget.h"

class UOdysseyAnimationLayer;
class UOdysseyAnimationCell;
class FOdysseyPainterEditorAnimationTImelinePosition;

class SOdysseyAnimationTimelineOutOfPegsKey
    : public SCompoundWidget
{
public:
    DECLARE_DELEGATE_OneParam(FOnKeyChanged, FOdysseyAnimationLightTableKey)
    DECLARE_DELEGATE_OneParam(FOnActivateOutOfPegs, UOdysseyAnimationCell*)
    DECLARE_DELEGATE_RetVal_OneParam(ECheckBoxState, FOnIsOutOfPegsChecked, UOdysseyAnimationCell*)

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineOutOfPegsKey)
    {}
        SLATE_ARGUMENT(TSharedPtr<FOdysseyPainterEditorAnimationTImelinePosition>, TimelinePosition)
        SLATE_ATTRIBUTE(UOdysseyAnimationCell*, Cell)
        SLATE_ATTRIBUTE(FOdysseyAnimationLightTableKey, Key)
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
    TAttribute<FOdysseyAnimationLightTableKey> mKey;

    FOnActivateOutOfPegs mOnActivateOutOfPegs;
    FSimpleDelegate mOnInactivateOutOfPegs;
    FOnIsOutOfPegsChecked mOnIsOutOfPegsChecked;
};
