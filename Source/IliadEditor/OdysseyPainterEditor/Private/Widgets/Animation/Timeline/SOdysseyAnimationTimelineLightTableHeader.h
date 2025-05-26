// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "OdysseyLighttable.h"

class SColorBlock;
class UOdysseyAnimationLayer;

//////////////////////////////////////////////////////////////////////////
// SOdysseyAnimationTimelineLighttableHeader
class SOdysseyAnimationTimelineLighttableHeader : public SCompoundWidget
{
public:
    SOdysseyAnimationTimelineLighttableHeader();

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineLighttableHeader)
        : _Layer(nullptr)
        {}
        SLATE_ARGUMENT(UOdysseyAnimationLayer*, Layer)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    FLinearColor GetLighttablePreviousKeysColor() const;
    FLinearColor GetLighttableNextKeysColor() const;

    FReply OnLighttablePreviousKeysColorMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) const;
    FReply OnLighttableNextKeysColorMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) const;

    TSharedRef< SWidget > OnOptionsGetMenuContent();

    void OnPreviousKeysContrastValueCommitted(int iValue, ETextCommit::Type iType);
    void OnPreviousKeysContrastValueChanged(int iValue);

    void OnNextKeysContrastValueCommitted(int iValue, ETextCommit::Type iType);
    void OnNextKeysContrastValueChanged(int iValue);

    void SetDisplayPosition(EOdysseyLighttableDisplayPosition iPosition);
    bool IsDisplayPositionSet(EOdysseyLighttableDisplayPosition iPosition) const;

private:
    UOdysseyAnimationLayer* mLayer;
    TSharedPtr<SColorBlock> mLighttablePreviousKeysColorBlockWidget;
    TSharedPtr<SColorBlock> mLighttableNextKeysColorBlockWidget;
};
