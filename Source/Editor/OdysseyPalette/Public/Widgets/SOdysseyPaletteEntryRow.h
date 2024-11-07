// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "OdysseyPaletteEntry.h"
#include "Widgets/SOdysseyPaletteTreeView.h"

class SOdysseyLayerStackTreeView;
class UOdysseyPaletteEntry;

/**
 * Implements a layer row widget
 */
class ODYSSEYPALETTE_API SOdysseyPaletteEntryRow
    : public SMultiColumnTableRow<UOdysseyPaletteEntry*>
{
public:
    SLATE_BEGIN_ARGS(SOdysseyPaletteEntryRow)
        {}
    SLATE_END_ARGS()
    
public:
    // Construction / Destruction
    void Construct(const FArguments& iArgs, const TSharedRef<SOdysseyPaletteTreeView>& iOwnerTableView, UOdysseyPaletteEntry* iEntry);
    
public:
    //Getters
    UOdysseyPaletteEntry* GetPaletteEntry();

    //Commands
    void Rename();

protected:
    //SMultiColumnTableRow overrides
    virtual TSharedRef<SWidget> GenerateWidgetForColumn( const FName& InColumnName ) override;
    virtual const FSlateBrush* GetBorder() const override;

protected:
    virtual TSharedRef<SWidget> GenerateHeaderWidget();
    TSharedRef<SWidget> GenerateIsActivatedWidget();
    TSharedRef<SWidget> GenerateExpandableHeaderWidget();

    void OnIsActivatedCheckBoxStateChanged(ECheckBoxState iState);
    ECheckBoxState GetIsActivatedCheckBoxState() const;
    
    void OnEntryNameCommited(const FText& iText, ETextCommit::Type iType);
    
    FText GetEntryName() const;
    FSlateFontInfo GetEntryNameFont() const;
    
    TOptional<EItemDropZone> OnRowCanAcceptDrop(const FDragDropEvent& iEvent, EItemDropZone iDropZone, UOdysseyPaletteEntry* iEntry);
    
    FReply OnRowAcceptDrop(const FDragDropEvent& iEvent, EItemDropZone iDropZone, UOdysseyPaletteEntry* iEntry);

    FReply OnRowDragDetected(const FGeometry& iGeometry, const FPointerEvent& iEvent, TWeakPtr<SOdysseyPaletteTreeView> iTreeView);
    

private:
    EItemDropZone ComputeItemDropZoneForLeaf(FVector2D iLocalPointerPos, FVector2D iLocalSize, bool iCanHaveChildren, bool iIsExpanded);
    
private:
    TSharedPtr<SInlineEditableTextBlock> mNameWidget = nullptr;
    UOdysseyPaletteEntry* mEntry = nullptr;
};
