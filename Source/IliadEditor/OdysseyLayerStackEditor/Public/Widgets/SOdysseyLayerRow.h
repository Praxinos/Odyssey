// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyLayer.h"

#include "CoreMinimal.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/SOdysseyLayerRowBase.h"

class SOdysseyLayerStackTreeView;

/**
 * Implements a layer row widget
 */
class ODYSSEYLAYERSTACKEDITOR_API SOdysseyLayerRow
    : public SOdysseyLayerRowBase
{
public:
    SOdysseyLayerRow();

    // Construction / Destruction
    void Construct(const FArguments& iArgs, const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView, UOdysseyLayer* iLayer);

public:
    //Commands
    void Rename();

protected:
    virtual TSharedRef<SWidget> GenerateWidget( const FName& iRow, const FName& iColumn ) override;
    virtual FMargin GetColumnPadding( FName iColumn ) const override;

    TSharedRef<SWidget> GenerateMainRowIsActivatedWidget();
    TSharedRef<SWidget> GenerateMainRowIsLockedWidget();
    TSharedRef<SWidget> GenerateMainRowDisplayOptionsWidget();

    TSharedRef<SWidget> GenerateMainRowHeaderWidget();
    TSharedRef<SWidget> GenerateLayerNameWidget();
    virtual TArray<TSharedPtr<SWidget>> GenerateMainRowHeaderOptionWidgets();

    TSharedRef<SWidget> GenerateBlendRowHeaderWidget();

    void OnIsActivatedCheckBoxStateChanged(ECheckBoxState iState);
    ECheckBoxState GetIsActivatedCheckBoxState() const;
    bool GetIsActivatedCheckBoxEnabled() const;
    void OnIsLockedCheckBoxStateChanged(ECheckBoxState iState);
    ECheckBoxState GetIsLockedCheckBoxState() const;
    bool GetIsLockedCheckBoxEnabled() const;

    void OnLayerNameCommited(const FText& iText, ETextCommit::Type iType);
    FText GetLayerName() const;
    FSlateFontInfo GetLayerNameFont() const;

    virtual FReply OnRowDragDetected(const FGeometry& iGeometry, const FPointerEvent& iEvent, TWeakPtr<SOdysseyLayerStackTreeView> iTreeView) override;

private:
    void OnBlendModeComboBoxChanged(int32 iValue, ESelectInfo::Type iSelectInfo);
    void OnOpacityValueChanged(int iValue);
    void OnOpacityValueCommitted(int iValue, ETextCommit::Type iType);
    void OnOpacityBeginSliderMovement();
    void OnOpacityEndSliderMovement(int iValue);

    EVisibility GetCollapsedOpacityVisibility() const;

private:
    void OnDisplayOptionsCheckBoxStateChanged(ECheckBoxState iState);
    ECheckBoxState GetDisplayOptionsCheckBoxState() const;

private:
    TSharedPtr<SInlineEditableTextBlock> mNameWidget = nullptr;
    FText mSetOpacityTransactionName;
};
