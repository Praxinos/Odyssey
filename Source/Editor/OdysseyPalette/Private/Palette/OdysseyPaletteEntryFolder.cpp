// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPaletteEntryFolder.h"

#define LOCTEXT_NAMESPACE "UOdysseyPaletteEntryFolder"

UOdysseyPaletteEntryFolder::UOdysseyPaletteEntryFolder()
{
    EntryTypeName = LOCTEXT("EntryTypeName", "Folder Entry");
    DefaultName = LOCTEXT("DefaultName", "Folder");
    Icon = *FEditorStyle::GetBrush("ContentBrowser.AssetTreeFolderClosed");
    IconExpanded = *FEditorStyle::GetBrush("ContentBrowser.AssetTreeFolderOpen");
    CanHaveChildren = true;
}

#undef LOCTEXT_NAMESPACE