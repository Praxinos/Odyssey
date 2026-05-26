// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/SListView.h"

#include "Widgets/Palette/SOdysseyPainterEditorPaletteEntryRow.h"
#include "OdysseyStyle.h"

class UOdysseyPaletteEntry;

/**
 * Implements a layer row widget
 */
class SOdysseyPainterEditorPaletteEntryRow
    : public STableRow<TSharedPtr<struct IOdysseyPainterEditorPaletteTreeViewItem>>
{
public:
    SLATE_BEGIN_ARGS(SOdysseyPainterEditorPaletteEntryRow)
        : _Style( &FOdysseyStyle::GetWidgetStyle<FTableRowStyle>("OdysseyPalette.Row") )
        , _Entry(nullptr)
        {}
        SLATE_STYLE_ARGUMENT( FTableRowStyle, Style )
        SLATE_ATTRIBUTE(UOdysseyPaletteEntry*, Entry)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(const FArguments& iArgs, const TSharedRef<STableViewBase>& iTreeView);

protected:
    virtual const FSlateBrush* GetIcon() const;
    virtual FSlateColor GetIconColorAndOpacity() const;

protected:
    virtual TSharedRef<SWidget> GenerateContentWidget();

    FText GetEntryName() const;

protected:
    TAttribute<UOdysseyPaletteEntry*> mEntry;
};
