// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "FOdysseyPalette.h"

/** Delegate used to set a generic object */
DECLARE_DELEGATE_OneParam( FOnMeshChanged, UStaticMesh* );

/**
 * Implements the mesh selector
 */
class ODYSSEYWIDGETS_API SOdysseyPalette : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyPalette)
        {}
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyPalette();
    void Construct(const FArguments& InArgs);

public:
    // Getter / Setter
    FOdysseyPalette* GetColorPalette() const;

private:
    // Private internal callbacks
    void OnObjectChanged(const FAssetData& AssetData);
    FString ObjectPath() const;
    FReply HandleMeshColorBlockMouseButtonDown();

private:
    //Internal Widget Creation
    TSharedRef<SWidget> CreateColorPaletteWidget();

public:
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;


private:
    void OnGetChildren(UOdysseyPaletteEntry* iParent, TArray<UOdysseyPaletteEntry*>& oChildren) const;
    TSharedRef<ITableRow> OnGenerateRow(UOdysseyPaletteEntry* iEntry, const TSharedRef<STableViewBase>& iOwnerTable);

private:
    // Private data
    TSharedPtr< FOdysseyPalette > mColorPalette;
    TSharedPtr< FAssetThumbnailPool > mAssetThumbnailPool;
    SScrollBox::FSlot*  mColorPaletteSlot;
    TSharedPtr<STreeView<UOdysseyPaletteEntry*>> mPaletteTreeView;
};
