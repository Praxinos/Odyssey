// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayer.h"

#include "CoreMinimal.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

class SOdysseyLayerStackTreeView;

/**
 * Implements a layer row widget
 */
class ODYSSEYWIDGETS_API SOdysseyPaletteEntryRow
    : public SMultiColumnTableRow<UOdysseyPaletteEntry*>
{
public:
    SLATE_BEGIN_ARGS(SOdysseyPaletteEntryRow)
        {}
    SLATE_END_ARGS()
    
public:
    // Construction / Destruction
    void Construct(const FArguments& iArgs, const TSharedRef<STreeView<UOdysseyPaletteEntry*>>& iOwnerTableView, UOdysseyPaletteEntry* iEntry);
    /*
public:
    //Getters
    UOdysseyLayer* GetLayer();

	//Commands
	void Rename();
    */
protected:
    //SMultiColumnTableRow overrides
    virtual TSharedRef<SWidget> GenerateWidgetForColumn( const FName& InColumnName ) override;
	virtual const FSlateBrush* GetBorder() const override;

protected:
	virtual TSharedRef<SWidget> GenerateHeaderWidget();
    TSharedRef<SWidget> GenerateIsActivatedWidget();
    /*
    void OnIsActivatedCheckBoxStateChanged(ECheckBoxState iState);
    ECheckBoxState GetIsActivatedCheckBoxState() const;
    void OnIsLockedCheckBoxStateChanged(ECheckBoxState iState);
    ECheckBoxState GetIsLockedCheckBoxState() const;

	void OnLayerNameCommited(const FText& iText, ETextCommit::Type iType);
    */
    FText GetEntryName() const;
    FSlateFontInfo GetEntryNameFont() const;
    /*
    TOptional<EItemDropZone> OnRowCanAcceptDrop(const FDragDropEvent& iEvent, EItemDropZone iDropZone, UOdysseyPaletteEntry* iEntry);

    FReply OnRowAcceptDrop(const FDragDropEvent& iEvent, EItemDropZone iDropZone, UOdysseyPaletteEntry* iEntry);

    FReply OnRowDragDetected(const FGeometry& iGeometry, const FPointerEvent& iEvent, TWeakPtr<STreeView<UOdysseyPaletteEntry*>> iTreeView);
    */
    /*

private:
    void OnIsOptionsDisplayedCheckBoxStateChanged(ECheckBoxState iState);
    ECheckBoxState GetIsOptionsDisplayedCheckBoxState() const;
    EItemDropZone ComputeItemDropZoneForLeaf(FVector2D iLocalPointerPos, FVector2D iLocalSize, bool iCanHaveChildren, bool iIsExpanded);
    */
private:
    TSharedPtr<SInlineEditableTextBlock> mNameWidget = nullptr;
    //bool mIsOptionsDisplayed = false;

    UOdysseyPaletteEntry* mEntry = nullptr;
};
