// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"

class SColorBlock;
class UOdysseyAnimationLayer;

//////////////////////////////////////////////////////////////////////////
// SOdysseyAnimationTimelineLightTableHeader
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineLightTableHeader : public SCompoundWidget
{
public:
    SOdysseyAnimationTimelineLightTableHeader();

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineLightTableHeader)
        : _Layer(nullptr)
        {}
        SLATE_ARGUMENT(UOdysseyAnimationLayer*, Layer)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    FLinearColor GetLightTablePreviousKeysColor() const;
    FLinearColor GetLightTableNextKeysColor() const;

    FReply OnLightTablePreviousKeysColorMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) const;
    FReply OnLightTableNextKeysColorMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) const;

    TSharedRef< SWidget > OnOptionsGetMenuContent();

    void OnPreviousKeysContrastValueCommitted(int iValue, ETextCommit::Type iType);
    void OnPreviousKeysContrastValueChanged(int iValue);

    void OnNextKeysContrastValueCommitted(int iValue, ETextCommit::Type iType);
    void OnNextKeysContrastValueChanged(int iValue);

    void SetDisplayPosition(EOdysseyLightTableDisplayPosition iPosition);
    bool IsDisplayPositionSet(EOdysseyLightTableDisplayPosition iPosition) const;

private:
    UOdysseyAnimationLayer* mLayer;
    TSharedPtr<SColorBlock> mLightTablePreviousKeysColorBlockWidget;
    TSharedPtr<SColorBlock> mLightTableNextKeysColorBlockWidget;
};
