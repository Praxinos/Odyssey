// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyPaletteEntryFolder.h"

#define LOCTEXT_NAMESPACE "Palette"

UOdysseyPaletteEntryFolder::UOdysseyPaletteEntryFolder()
{
    EntryTypeName = LOCTEXT("entry-folder.type", "Folder");
    DefaultName = LOCTEXT("entry-folder.default-name", "Folder");
    Icon = *FAppStyle::GetBrush("ContentBrowser.AssetTreeFolderClosed");
    IconExpanded = *FAppStyle::GetBrush("ContentBrowser.AssetTreeFolderOpen");
    CanHaveChildren = true;
}

#undef LOCTEXT_NAMESPACE
