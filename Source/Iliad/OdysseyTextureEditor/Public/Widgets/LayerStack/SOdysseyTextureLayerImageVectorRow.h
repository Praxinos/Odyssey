// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SOdysseyLayerRow.h"

/**
 * Implements a layer row widget
 */
class ODYSSEYTEXTUREEDITOR_API SOdysseyTextureLayerImageVectorRow
    : public SOdysseyLayerRow
{
public:
    SLATE_BEGIN_ARGS(SOdysseyTextureLayerImageVectorRow)
        {}
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    SOdysseyTextureLayerImageVectorRow();
    void Construct(const FArguments& iArgs, const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView, class UOdysseyTextureLayerImageVector* iTextureLayerImageVector);

private:
    virtual TSharedRef<SWidget> GenerateHeaderWidget() override;
    virtual TSharedRef<SWidget> GenerateOptionsWidget() override;
    void OnIsColoredCheckStateChanged( ECheckBoxState iState );
    ECheckBoxState GetIsColoredIsChecked() const;
    void OnIsWireframeCheckStateChanged( ECheckBoxState iState );
    ECheckBoxState GetIsWireframeIsChecked() const;
    EVisibility GetCollapsedOpacityVisibility() const;

private:
    void OnBlendModeComboBoxChanged(int32 iValue, ESelectInfo::Type iSelectInfo);
    void OnOpacityValueChanged(int iValue);
    void OnOpacityValueCommitted(int iValue, ETextCommit::Type iType);
    void OnOpacityBeginSliderMovement();
    void OnOpacityEndSliderMovement(int iValue);

private:
    class UOdysseyTextureLayerImageVector* mTextureLayerImageVector;
    FText mSetOpacityTransactionName;
};
