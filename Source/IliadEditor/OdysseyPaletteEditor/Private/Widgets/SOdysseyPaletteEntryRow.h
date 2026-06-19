// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "OdysseyPaletteEntry.h"

class UOdysseyPaletteEntry;
class SOdysseyPaletteTreeView;

/**
 * Implements a layer row widget
 */
class SOdysseyPaletteEntryRow
    : public SMultiColumnTableRow<UOdysseyPaletteEntry*>
{
public:
    SLATE_BEGIN_ARGS(SOdysseyPaletteEntryRow)
        : _IsReadOnly(true)
        {}
        SLATE_ARGUMENT(bool, IsReadOnly)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(const FArguments& iArgs, const TSharedRef<SOdysseyPaletteTreeView>& iOwnerTableView, UOdysseyPaletteEntry* iEntry);

public:
    //Getters
    //UOdysseyPaletteEntry* GetPaletteEntry();

    //Commands
    void Rename();

protected:
    //SMultiColumnTableRow overrides
    virtual TSharedRef<SWidget> GenerateWidgetForColumn( const FName& InColumnName ) override;

    virtual const FSlateBrush* GetIcon() const;
    virtual FSlateColor GetIconColorAndOpacity() const;
    //virtual const FSlateBrush* GetBorder() const override;

protected:
    virtual TSharedRef<SWidget> GenerateHeaderWidget();

    void OnEntryNameCommited(const FText& iText, ETextCommit::Type iType);

    FText GetEntryName() const;
    FSlateFontInfo GetEntryNameFont() const;

    TOptional<EItemDropZone> OnRowCanAcceptDrop(const FDragDropEvent& iEvent, EItemDropZone iDropZone, UOdysseyPaletteEntry* iEntry);

    FReply OnRowAcceptDrop(const FDragDropEvent& iEvent, EItemDropZone iDropZone, UOdysseyPaletteEntry* iEntry);

    FReply OnRowDragDetected(const FGeometry& iGeometry, const FPointerEvent& iEvent);


private:
    EItemDropZone ComputeItemDropZoneForLeaf(FVector2D iLocalPointerPos, FVector2D iLocalSize, bool iCanHaveChildren, bool iIsExpanded);

protected:
    TWeakPtr<class SOdysseyPaletteTreeView> mTreeView;
    TSharedPtr<SInlineEditableTextBlock> mNameWidget = nullptr;
    UOdysseyPaletteEntry* mEntry = nullptr;
    bool mIsReadOnly;
};
