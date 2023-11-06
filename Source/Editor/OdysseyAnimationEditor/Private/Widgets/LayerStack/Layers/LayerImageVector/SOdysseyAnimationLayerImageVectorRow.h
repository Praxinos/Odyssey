// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LayerStack/Layers/SOdysseyAnimationLayerRow.h"

class FOdysseyAnimationEditorExtension;

/**
 * Implements a layer row widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerImageVectorRow
    : public SOdysseyAnimationLayerRow
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerImageVectorRow)
        {}
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(
        const FArguments& iArgs,
        const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
        FOdysseyAnimationEditorExtension* iExtension,
        class UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector
    );

private:
    virtual TSharedRef<SWidget> GenerateHeaderWidget() override;
    virtual TSharedRef<SWidget> GenerateOptionsWidget() override;
    virtual TSharedRef<SWidget> GenerateTimelineWidget() override;
    void OnLightTableCheckStateChanged(ECheckBoxState iState);
    void OnIsColoredCheckStateChanged( ECheckBoxState iState );
    ECheckBoxState GetLightTableIsChecked() const;
    ECheckBoxState GetIsColoredIsChecked() const;
    EVisibility GetLightTableVisibility() const;

private:
    void OnBlendModeComboBoxChanged(int32 iValue, ESelectInfo::Type iSelectInfo);
    void OnOpacityValueChanged(int iValue);
    void OnOpacityValueCommitted(int iValue, ETextCommit::Type iType);
    void OnOpacityBeginSliderMovement();
    void OnOpacityEndSliderMovement(int iValue);

private:
    class UOdysseyAnimationLayerImageVector* mAnimationLayerImageVector;
};
