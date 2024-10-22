// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
    // Construction / Destruction
    void Construct(const FArguments& iArgs, const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView, UOdysseyLayer* iLayer);

public:
	//Commands
	void Rename();

protected:
    //SMultiColumnTableRow overrides
    virtual TSharedRef<SWidget> GenerateWidgetForColumn( const FName& InColumnName ) override;

protected:
	virtual TSharedRef<SWidget> GenerateHeaderWidget();
    virtual TSharedRef<SWidget> GenerateOptionsWidget();
    TSharedRef<SWidget> GenerateExpandableHeaderWidget();
	TSharedRef<SWidget> GenerateDisplayOptionsWidget();
    TSharedRef<SWidget> GenerateIsActivatedWidget();
    TSharedRef<SWidget> GenerateIsLockedWidget();
    
    void OnIsActivatedCheckBoxStateChanged(ECheckBoxState iState);
    ECheckBoxState GetIsActivatedCheckBoxState() const;
    void OnIsLockedCheckBoxStateChanged(ECheckBoxState iState);
    ECheckBoxState GetIsLockedCheckBoxState() const;

	void OnLayerNameCommited(const FText& iText, ETextCommit::Type iType);
    FText GetLayerName() const;
    FSlateFontInfo GetLayerNameFont() const;
    
    EVisibility OptionsWidgetVisibility() const;

    virtual FReply OnRowDragDetected(const FGeometry& iGeometry, const FPointerEvent& iEvent, TWeakPtr<SOdysseyLayerStackTreeView> iTreeView) override;

private:
    void OnDisplayOptionsCheckBoxStateChanged(ECheckBoxState iState);
    ECheckBoxState GetDisplayOptionsCheckBoxState() const;

private:
    TSharedPtr<SInlineEditableTextBlock> mNameWidget = nullptr;
};
