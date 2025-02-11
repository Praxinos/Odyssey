// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Palette/SOdysseyPainterEditorPaletteFolderRow.h"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
void SOdysseyPainterEditorPaletteFolderRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& iTreeView)
{
    SOdysseyPainterEditorPaletteEntryRow::Construct(
        SOdysseyPainterEditorPaletteEntryRow::FArguments()
            .Entry(InArgs._Entry),
        iTreeView
    );
}

const FSlateBrush*
SOdysseyPainterEditorPaletteFolderRow::GetIcon() const
{
    if (IsItemExpanded())
        return FAppStyle::Get().GetBrush("ContentBrowser.AssetTreeFolderOpen");
    return FAppStyle::Get().GetBrush("ContentBrowser.AssetTreeFolderClosed");
}

FSlateColor
SOdysseyPainterEditorPaletteFolderRow::GetIconColorAndOpacity() const
{
    static const FName FolderColorName("ContentBrowser.DefaultFolderColor");
    return FAppStyle::Get().GetSlateColor(FolderColorName).GetSpecifiedColor();
}

bool
SOdysseyPainterEditorPaletteFolderRow::IsItemSelected() const
{
    return false;
}

ESelectionMode::Type
SOdysseyPainterEditorPaletteFolderRow::GetSelectionMode() const
{
    return ESelectionMode::None;
}
