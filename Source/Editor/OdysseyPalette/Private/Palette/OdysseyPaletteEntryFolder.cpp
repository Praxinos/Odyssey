// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPaletteEntryFolder.h"

#define LOCTEXT_NAMESPACE "Palette"

UOdysseyPaletteEntryFolder::UOdysseyPaletteEntryFolder()
{
    EntryTypeName = LOCTEXT("entry-folder.type", "Folder Entry");
    DefaultName = LOCTEXT("entry-folder.default-name", "Folder");
    Icon = *FEditorStyle::GetBrush("ContentBrowser.AssetTreeFolderClosed");
    IconExpanded = *FEditorStyle::GetBrush("ContentBrowser.AssetTreeFolderOpen");
    CanHaveChildren = true;
}

#undef LOCTEXT_NAMESPACE