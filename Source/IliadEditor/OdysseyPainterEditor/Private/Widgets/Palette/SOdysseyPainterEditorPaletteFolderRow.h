// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Palette/SOdysseyPainterEditorPaletteEntryRow.h"

class UOdysseyPaletteEntry;

/**
 * Implements an entry row widget
 */
class SOdysseyPainterEditorPaletteFolderRow
    : public SOdysseyPainterEditorPaletteEntryRow
{
 public:
    SLATE_BEGIN_ARGS(SOdysseyPainterEditorPaletteFolderRow)
        : _Entry(nullptr)
        {}
        SLATE_ATTRIBUTE(UOdysseyPaletteEntry*, Entry)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(const FArguments& iArgs, const TSharedRef<STableViewBase>& iTreeView);

protected:
    virtual const FSlateBrush* GetIcon() const override;
    virtual FSlateColor GetIconColorAndOpacity() const override;

    virtual TSharedRef<SWidget> GenerateContentWidget() override;

public:
    /** @return True if the corresponding item is selected; false otherwise */
    virtual bool IsItemSelected() const override;

protected:
    /** Called to query the selection mode for the row */
    virtual ESelectionMode::Type GetSelectionMode() const override;
};
