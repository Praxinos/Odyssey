// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyPaletteAddEntryButton.h"
#include "EditorFontGlyphs.h"
#include "OdysseyPaletteEntryColor.h"
#include "OdysseyPaletteEntryMaterial.h"
#include "OdysseyPaletteEntryFolder.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "SPositiveActionButton.h"
#include "OdysseyPalette.h"
#include "OdysseyStyleSet.h"

#define LOCTEXT_NAMESPACE "Palette"

//CONSTRUCTION/DESTRUCTION-----------------------------------------------

SOdysseyPaletteAddEntryButton::~SOdysseyPaletteAddEntryButton()
{
}


void SOdysseyPaletteAddEntryButton::Construct(const FArguments& InArgs)
{
    mPalette = InArgs._Palette;
    mOnAdded = InArgs._OnAdded;

    ChildSlot
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        [
            SNew(SButton)
            .HAlign(HAlign_Left)
            .OnClicked(this, &SOdysseyPaletteAddEntryButton::AddColorEntry )
            [
                SNew(SImage).Image(FOdysseyStyle::GetBrush("OdysseyPalette.AddColor"))
            ]
        ]
        + SHorizontalBox::Slot()
        [
            SNew(SButton)
            .HAlign(HAlign_Left)
            .OnClicked(this, &SOdysseyPaletteAddEntryButton::AddMaterialEntry)
            [
                SNew(SImage).Image(FOdysseyStyle::GetBrush("OdysseyPalette.AddMaterial"))
            ]
        ]
        + SHorizontalBox::Slot()
        [
            SNew(SButton)
            .HAlign(HAlign_Left)
            .OnClicked(this, &SOdysseyPaletteAddEntryButton::AddFolderEntry)
            [
                SNew(SImage).Image(FOdysseyStyle::GetBrush("OdysseyPalette.AddFolder"))
            ]
        ]
        + SHorizontalBox::Slot()
        [
            SNew(SButton)
            .HAlign(HAlign_Left)
            .OnClicked(this, &SOdysseyPaletteAddEntryButton::AddColorEntry)
            [
                SNew(SImage).Image(FOdysseyStyle::GetBrush("OdysseyPalette.AddPicker"))
            ]
        ]
    ];
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- SWidget overrides

//PRIVATE API-----------------------------------------------------------

FReply
SOdysseyPaletteAddEntryButton::AddColorEntry()
{
    UOdysseyPalette* palette = mPalette.Get();
    if (!palette)
        return FReply::Unhandled();

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("add-color-entry-button.transaction.add-color-entry", "Add Color Entry"));
#endif

    UOdysseyPaletteEntry* currentEntry = palette->CurrentEntry.Get();
    if (currentEntry)
    {
        if (currentEntry->CanHaveChildren)
        {
            currentEntry = palette->AddEntry(UOdysseyPaletteEntryColor::StaticClass(), currentEntry);
        }
        else
        {
            UOdysseyPaletteEntry* parent = currentEntry->GetParent();
            int index = currentEntry->GetIndexInParent();
            currentEntry = palette->AddEntry(UOdysseyPaletteEntryColor::StaticClass(), parent, index);
        }
    }
    else
    {
        currentEntry = palette->AddEntry(UOdysseyPaletteEntryColor::StaticClass());
    }

    mOnAdded.ExecuteIfBound(currentEntry);
    FOdysseyObjectEditorUtils::SetPropertyValue(palette, "CurrentEntry", TSoftObjectPtr<UOdysseyPaletteEntry>(currentEntry));

    return FReply::Handled();
}

FReply
SOdysseyPaletteAddEntryButton::AddMaterialEntry()
{
    return FReply::Handled();
}

FReply
SOdysseyPaletteAddEntryButton::AddFolderEntry()
{
    UOdysseyPalette* palette = mPalette.Get();
    if (!palette)
        return FReply::Unhandled();

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("add-folder-entry-button.transaction.add-folder-entry", "Add Folder Entry"));
#endif

    UOdysseyPaletteEntry* currentEntry = palette->CurrentEntry.Get();
    if (currentEntry)
    {
        if (currentEntry->CanHaveChildren)
        {
            currentEntry = palette->AddEntry(UOdysseyPaletteEntryFolder::StaticClass(), currentEntry);
        }
        else
        {
            UOdysseyPaletteEntry* parent = currentEntry->GetParent();
            int index = currentEntry->GetIndexInParent();
            currentEntry = palette->AddEntry(UOdysseyPaletteEntryFolder::StaticClass(), parent, index);
        }
    }
    else
    {
        currentEntry = palette->AddEntry(UOdysseyPaletteEntryFolder::StaticClass());
    }

    mOnAdded.ExecuteIfBound(currentEntry);
    FOdysseyObjectEditorUtils::SetPropertyValue(palette, "CurrentEntry", TSoftObjectPtr<UOdysseyPaletteEntry>(currentEntry));

    return FReply::Handled();
}


#undef LOCTEXT_NAMESPACE
