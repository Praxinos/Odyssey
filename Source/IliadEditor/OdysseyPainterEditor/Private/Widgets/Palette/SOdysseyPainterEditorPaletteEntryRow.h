// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Palette/SOdysseyPainterEditorPaletteEntryRow.h"
#include "OdysseyStyleSet.h"

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
    //STableRow overrides
    virtual TSharedRef<SWidget> GenerateWidgetForColumn( const FName& InColumnName );

    virtual const FSlateBrush* GetIcon() const;
    virtual FSlateColor GetIconColorAndOpacity() const;

    float GetColumnWidth(int iColumnIndex) const;
    void OnColumnResized(float iSize, int iColumnIndex);

protected:
    virtual TSharedRef<SWidget> GenerateHeaderWidget();

    FText GetEntryName() const;

protected:
    TAttribute<UOdysseyPaletteEntry*> mEntry;
};
