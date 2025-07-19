// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Palette/SOdysseyPainterEditorPaletteSetList.h"

/**
 * Implements a layer row widget
 */
class SOdysseyPainterEditorPaletteRow
    : public STableRow<TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem>>
{
public:
    DECLARE_DELEGATE_OneParam(FOnSetChanged, FGuid)

public:
    SLATE_BEGIN_ARGS(SOdysseyPainterEditorPaletteRow)
        {}
        SLATE_ATTRIBUTE(UOdysseyPalette*, Palette)
        SLATE_ATTRIBUTE(FGuid, Set)
        SLATE_EVENT(FOnSetChanged, OnSetChanged)
        SLATE_EVENT(FOnClicked, OnDeleteButtonClicked)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(const FArguments& iArgs, const TSharedRef<STableViewBase>& iTreeView);

    /** @return True if the corresponding item is selected; false otherwise */
    virtual bool IsItemSelected() const override;

protected:
    /** Called to query the selection mode for the row */
    virtual ESelectionMode::Type GetSelectionMode() const override;

private:
    FText GetPaletteName() const;

protected:
    TAttribute<UOdysseyPalette*> mPalette;
    TAttribute<FGuid> mSet;
    FOnSetChanged mOnSetChanged;
    FOnClicked mOnDeleteButtonClicked;
};
