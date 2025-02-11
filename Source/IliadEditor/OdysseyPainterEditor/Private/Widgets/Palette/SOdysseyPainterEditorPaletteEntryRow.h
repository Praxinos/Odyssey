// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Palette/SOdysseyPainterEditorPaletteEntryRow.h"

class UOdysseyPaletteEntry;

/**
 * Implements a layer row widget
 */
class SOdysseyPainterEditorPaletteEntryRow
    : public SMultiColumnTableRow<TSharedPtr<struct IOdysseyPainterEditorPaletteTreeViewItem>>
{
public:
    SLATE_BEGIN_ARGS(SOdysseyPainterEditorPaletteEntryRow)
        : _Entry(nullptr)
        {}
        SLATE_ATTRIBUTE(UOdysseyPaletteEntry*, Entry)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(const FArguments& iArgs, const TSharedRef<STableViewBase>& iTreeView);

protected:
    //SMultiColumnTableRow overrides
    virtual TSharedRef<SWidget> GenerateWidgetForColumn( const FName& InColumnName ) override;

    virtual const FSlateBrush* GetIcon() const;
    virtual FSlateColor GetIconColorAndOpacity() const;

protected:
    virtual TSharedRef<SWidget> GenerateHeaderWidget();

    FText GetEntryName() const;

protected:
    TAttribute<UOdysseyPaletteEntry*> mEntry;
};
