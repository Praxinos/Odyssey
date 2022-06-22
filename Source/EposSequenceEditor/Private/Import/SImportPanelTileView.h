// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "Widgets/Views/STableRow.h"

#include "Import/ImportImageSequenceStruct.h"

//---

struct FImportPanelItem
{
    const FImportImageSequenceStruct*   mRootStruct;
    const FImportImageSequenceBoard*    mBoard;
    const FImportImageSequenceShot*     mShot;
    FImportImageSequencePanel*          mPanel;

    FImportImageSequenceOptions*    mOptions;

    TObjectPtr<UTexture2D>          mThumbnail;
    FSlateBrush*                    mBrush;

    void CreateThumbnail();
};

//---

/**
 * Single thumbnail tile for a panel to import.
 */
class SImportPanelTileView
    : public STableRow<TSharedPtr<FImportPanelItem>>
{
public:
    SLATE_BEGIN_ARGS( SImportPanelTileView )
        {}
        SLATE_ARGUMENT( TSharedPtr<FImportPanelItem>, Item )
    SLATE_END_ARGS()

    static TSharedRef<ITableRow> BuildTile( TSharedPtr<FImportPanelItem> Item, const TSharedRef<STableViewBase>& OwnerTable );

    void Construct( const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTable );

private:
    FText GetTooltipText() const;

    FText GetDurationText() const;

    void OnDurationTextCommitted( const FText& iText, ETextCommit::Type iType );
    bool OnDurationTextVerify( const FText& iText, FText& iError );

    bool    IsBoardEven() const;
    bool    IsShotEven() const;

    const FSlateBrush* GetBoardAreaBackgroundBrush() const;
    const FSlateBrush* GetShotAreaBackgroundBrush() const;
    const FSlateBrush* GetPanelAreaBackgroundBrush() const;

    const FSlateBrush*  GetTopAreaBackgroundBrush() const;
    const FSlateBrush*  GetThumbnailAreaBackgroundBrush() const;
    const FSlateBrush*  GetBottomAreaBackgroundBrush() const;

    FSlateColor         GetNameAreaTextColor() const;

private:
    TSharedPtr<FImportPanelItem> mPanelItem;
};
