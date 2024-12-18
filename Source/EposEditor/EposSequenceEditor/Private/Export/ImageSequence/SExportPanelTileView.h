// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "Widgets/Views/STableRow.h"

#include "Export/ImageSequence/PanelThumbnail.h"
#include "Export/ExportStruct.h"

//---

struct FExportImageSequenceOptions;

struct FPanelItem
{
    FExportPanel                    mPanel;
    int32                           mIndex;
    bool                            mExport;
    TWeakPtr<ISequencer>            mSequencer;
    FExportImageSequenceOptions*    mOptions;

    TSharedPtr<FPanelThumbnail>     mThumbnail;

    void CreateThumbnail();
};

//---

/**
 * Single thumbnail tile for a panel to export.
 */
class SPanelTileView
    : public STableRow<TSharedPtr<FPanelItem>>
{
public:
    SLATE_BEGIN_ARGS( SPanelTileView )
        {}
        SLATE_ARGUMENT( TSharedPtr<FPanelItem>, Item )
    SLATE_END_ARGS()

    static TSharedRef<ITableRow> BuildTile( TSharedPtr<FPanelItem> Item, const TSharedRef<STableViewBase>& OwnerTable );

    void Construct( const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTable );

private:
    /** Handles the change in activation of the item in the list */
    void            HandleExportCheckStateChanged( const ECheckBoxState iNewCheckedState );
    /** Gets whether the panel is exportable in the list  */
    ECheckBoxState  GetExportCheckBoxState() const;

    FText GetTooltipText() const;

    const FSlateBrush*  GetTopAreaBackgroundBrush() const;
    const FSlateBrush*  GetBottomAreaBackgroundBrush() const;
    FSlateColor         GetNameAreaTextColor() const;

private:
    TSharedPtr<FPanelItem> mPanelItem;
};
