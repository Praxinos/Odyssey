// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#pragma once

#include "OdysseyLayer.h"

#include "CoreMinimal.h"
#include "Widgets/Views/STableRow.h"

class SOdysseyLayerStackTreeView;

/**
 * Implements a layer row widget
 */
class ODYSSEYLAYERSTACKEDITOR_API SOdysseyLayerRowBase
    : public SMultiColumnTableRow<UOdysseyLayer*>
{
public:
    SLATE_BEGIN_ARGS(SOdysseyLayerRowBase)
        {}
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(const FArguments& iArgs, const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView, UOdysseyLayer* iLayer);

public:
    //Getters
    UOdysseyLayer* GetLayer() const;
    TSharedPtr<SOdysseyLayerStackTreeView> GetTreeView() const;

protected:
    //SMultiColumnTableRow overrides
    virtual const FSlateBrush* GetBorder() const override;

protected:
    virtual TSharedRef<SWidget> GenerateWidgetForColumn( const FName& InColumnName ) override;
    virtual TSharedRef<SWidget> GenerateWidgetForRow( const FName& iRow, const FName& iColumn );
    virtual TSharedRef<SWidget> GenerateWidget( const FName& iRow, const FName& iColumn );
    virtual float GetRowHeight(FName iRow) const;
    virtual EVisibility GetRowVisibility(FName iRow) const;
    virtual FMargin GetRowPadding( FName iRow ) const;
    virtual FMargin GetColumnPadding( FName iColumn ) const;

    TOptional<EItemDropZone> OnRowCanAcceptDrop(const FDragDropEvent& iEvent, EItemDropZone iDropZone, UOdysseyLayer* iLayer);
    FReply OnRowAcceptDrop(const FDragDropEvent& iEvent, EItemDropZone iDropZone, UOdysseyLayer* iLayer);
    virtual FReply OnRowDragDetected(const FGeometry& iGeometry, const FPointerEvent& iEvent, TWeakPtr<SOdysseyLayerStackTreeView> iTreeView);
    EItemDropZone ComputeItemDropZoneForLeaf(FVector2D iLocalPointerPos, FVector2D iLocalSize, bool iCanHaveChildren, bool iIsExpanded);

private:
    UOdysseyLayer* mLayer = nullptr;
    TWeakPtr<SOdysseyLayerStackTreeView> mTreeView;
};
