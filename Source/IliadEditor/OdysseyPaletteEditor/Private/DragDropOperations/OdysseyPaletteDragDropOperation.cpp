// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPaletteDragDropOperation.h"

#include "OdysseyPalette.h"
#include "OdysseyPaletteEntry.h"
#include "OdysseyStyle.h"
#include "Palette/OdysseyPaletteEntryColor.h"
#include "Palette/OdysseyPaletteEntryFolder.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "Palette"

FOdysseyPaletteDragDropOperation::FOdysseyPaletteDragDropOperation(UOdysseyPalette* iPalette, TArray<UOdysseyPaletteEntry*> iPaletteEntries)
    : mPalette(iPalette)
    , mPaletteEntries(iPaletteEntries)
{
}

TSharedPtr<SWidget>
FOdysseyPaletteDragDropOperation::GetDefaultDecorator() const
{
    return SNew(SBorder)
        .BorderImage(FAppStyle::GetBrush("Graph.ConnectorFeedback.Border")) //weird, but everyone does this
        .Content()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(0.f, 0.f, 2.f, 0.f)
            [
                SNew(SImage)
                .Image(this, &FOdysseyPaletteDragDropOperation::GetIcon)
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(STextBlock)
                .Text(this, &FOdysseyPaletteDragDropOperation::GetText)
            ]
        ];
}

FText
FOdysseyPaletteDragDropOperation::GetText() const
{
    if (mPaletteEntries.Num() == 1)
        return mPaletteEntries[0]->EntryName;

    return FText::Format(LOCTEXT("palette-drag-drop-operation.entries-count", "{0} Entries"), FText::AsNumber(mPaletteEntries.Num()));
}

const FSlateBrush*
FOdysseyPaletteDragDropOperation::GetIcon() const
{
    if (mPaletteEntries.Num() == 1)
    {
        if(mPaletteEntries[0]->IsA<UOdysseyPaletteEntryFolder>())
        {
            return FAppStyle::GetBrush("ContentBrowser.AssetTreeFolderClosed");
        }
        else if(mPaletteEntries[0]->IsA<UOdysseyPaletteEntryColor>())
        {
            return FOdysseyStyle::GetBrush("OdysseyPalette.EntryColor");
        }
        else
        {
            checkNoEntry();
        }
    }

    return FOdysseyStyle::GetBrush("PainterEditor.Layers16");
}

TArray< UOdysseyPaletteEntry* >
FOdysseyPaletteDragDropOperation::GetTopmostPaletteEntries() const
{

    TArray< UOdysseyPaletteEntry* > topmostEntries;
    for (UOdysseyPaletteEntry* entry : mPaletteEntries)
    {
        bool hasParentInDraggedEntries = false;
        TArray<UOdysseyPaletteEntry*> parents = entry->GetParents();
        for ( UOdysseyPaletteEntry* parent : parents )
        {
            if ( mPaletteEntries.Contains(parent) )
            {
                hasParentInDraggedEntries = true;
                break;
            }
        }

        if (hasParentInDraggedEntries)
            continue;

        topmostEntries.Add(entry);
    }
    return topmostEntries;
}

TArray< UOdysseyPaletteEntry* >
FOdysseyPaletteDragDropOperation::GetPaletteEntries() const
{
    return mPaletteEntries;
}

UOdysseyPalette*
FOdysseyPaletteDragDropOperation::GetPalette() const
{
    return mPalette;
}

#undef LOCTEXT_NAMESPACE
