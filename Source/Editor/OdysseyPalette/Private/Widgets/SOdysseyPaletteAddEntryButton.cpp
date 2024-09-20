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
        SNew(SPositiveActionButton)
        .Text(LOCTEXT("add-entry-button.add-color", "Add"))
        .OnGetMenuContent(this, &SOdysseyPaletteAddEntryButton::MakeMenu)
    ];
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- SWidget overrides

//PRIVATE API-----------------------------------------------------------

TSharedRef<SWidget> SOdysseyPaletteAddEntryButton::MakeMenu()
{
    UOdysseyPalette* palette = mPalette.Get();
    if (!palette)
        return SNullWidget::NullWidget;

    
    FMenuBuilder menuBuilder(true, nullptr);
    menuBuilder.BeginSection("AddEntries");
    {
        //Color
        UOdysseyPaletteEntry* entryCDO = UOdysseyPaletteEntryColor::StaticClass()->GetDefaultObject<UOdysseyPaletteEntryColor>();
                
        menuBuilder.AddMenuEntry(
            entryCDO->EntryTypeName,
            entryCDO->Description,
            FSlateIcon(),
            FUIAction(FExecuteAction::CreateRaw(this, &SOdysseyPaletteAddEntryButton::AddEntryFromClass, FAssetData(UOdysseyPaletteEntryColor::StaticClass()))));

        //Material
        entryCDO = UOdysseyPaletteEntryMaterial::StaticClass()->GetDefaultObject<UOdysseyPaletteEntryMaterial>();
        FUIAction action = FUIAction(FExecuteAction::CreateRaw(this, &SOdysseyPaletteAddEntryButton::AddEntryFromClass, FAssetData(UOdysseyPaletteEntryMaterial::StaticClass())), FCanExecuteAction::CreateLambda([](){return false;}));

        menuBuilder.AddMenuEntry(
            entryCDO->EntryTypeName,
            entryCDO->Description,
            FSlateIcon(),
            FUIAction( action ));

        //Folder
        entryCDO = UOdysseyPaletteEntryFolder::StaticClass()->GetDefaultObject<UOdysseyPaletteEntryFolder>();

        menuBuilder.AddMenuEntry(
            entryCDO->EntryTypeName,
            entryCDO->Description,
            FSlateIcon(),
            FUIAction(FExecuteAction::CreateRaw(this, &SOdysseyPaletteAddEntryButton::AddEntryFromClass, FAssetData(UOdysseyPaletteEntryFolder::StaticClass()))));
    }
    menuBuilder.EndSection();
    
    return menuBuilder.MakeWidget();
}

void
SOdysseyPaletteAddEntryButton::AddEntryFromClass(FAssetData iAssetData)
{
    UOdysseyPalette* palette = mPalette.Get();
    if (!palette)
        return;

    UObject* loadedAsset = iAssetData.FastGetAsset(true);
    if ( !loadedAsset )
        return;

    UClass* entryClass = Cast<UClass>(loadedAsset);
    if ( UBlueprint* blueprint = Cast<UBlueprint>(loadedAsset) )
        entryClass = blueprint->GeneratedClass;

    if ( !entryClass )
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("add-entry-button.transaction.add-entry", "Add Entry"));
#endif

    UOdysseyPaletteEntry* currentEntry = palette->CurrentEntry.Get();
    if (currentEntry)
    {
        if (currentEntry->CanHaveChildren)
        {
			currentEntry = palette->AddEntry(entryClass, currentEntry);
        }
        else
        {
			UOdysseyPaletteEntry* parent = currentEntry->GetParent();
			int index = currentEntry->GetIndexInParent();
			currentEntry = palette->AddEntry(entryClass, parent, index);
        }
    }
    else
    {
		currentEntry = palette->AddEntry(entryClass);
    }

    mOnAdded.ExecuteIfBound(currentEntry);
    FOdysseyObjectEditorUtils::SetPropertyValue(palette, "CurrentEntry", TSoftObjectPtr<UOdysseyPaletteEntry>(currentEntry));
}

#undef LOCTEXT_NAMESPACE
