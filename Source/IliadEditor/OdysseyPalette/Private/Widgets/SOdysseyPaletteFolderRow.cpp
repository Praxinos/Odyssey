// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyPaletteFolderRow.h"

const FSlateBrush*
SOdysseyPaletteFolderRow::GetIcon() const
{
    if (IsItemExpanded())
        return FAppStyle::Get().GetBrush("ContentBrowser.AssetTreeFolderOpen");
    return FAppStyle::Get().GetBrush("ContentBrowser.AssetTreeFolderClosed");
}

FSlateColor
SOdysseyPaletteFolderRow::GetIconColorAndOpacity() const
{
    static const FName FolderColorName("ContentBrowser.DefaultFolderColor");
    return FAppStyle::Get().GetSlateColor(FolderColorName).GetSpecifiedColor();
}
