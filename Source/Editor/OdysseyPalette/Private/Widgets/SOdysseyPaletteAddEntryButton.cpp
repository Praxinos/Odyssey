// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyPaletteAddEntryButton.h"
#include "EditorFontGlyphs.h"
#include "OdysseyPaletteEntryColor.h"
#include "OdysseyPaletteEntryMaterial.h"
#include "OdysseyPaletteEntryFolder.h"

#define LOCTEXT_NAMESPACE "SOdysseyPaletteAddEntryButton"

//CONSTRUCTION/DESTRUCTION-----------------------------------------------

SOdysseyPaletteAddEntryButton::~SOdysseyPaletteAddEntryButton()
{
}


void SOdysseyPaletteAddEntryButton::Construct(const FArguments& InArgs)
{
    mPalette = InArgs._Palette;

    SComboButton::FArguments args;
    args.OnGetMenuContent(this, &SOdysseyPaletteAddEntryButton::MakeMenu)
        .ButtonStyle(FAppStyle::Get(), "FlatButton.Success")
        .ContentPadding(FMargin(0.0f, 5.0f))
        .HasDownArrow(true)
        .ButtonContent()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .VAlign(VAlign_Bottom)
            .AutoWidth()
            [
                SNew(STextBlock)
                .TextStyle(FAppStyle::Get(), "NormalText.Important")
                .Font(FAppStyle::Get().GetFontStyle("FontAwesome.10"))
                .Text(FEditorFontGlyphs::Plus)
            ]

            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(4, 0, 0, 0)
            [
                SNew(STextBlock)
                .TextStyle(FAppStyle::Get(), "NormalText.Important")
                .Text(LOCTEXT("AddEntry", "Add Entry"))
            ]
        ];

    SComboButton::Construct(args);
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

        menuBuilder.AddMenuEntry(
            entryCDO->EntryTypeName,
            entryCDO->Description,
            FSlateIcon(),
            FUIAction(FExecuteAction::CreateRaw(this, &SOdysseyPaletteAddEntryButton::AddEntryFromClass, FAssetData(UOdysseyPaletteEntryMaterial::StaticClass()))));

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
    UObject* loadedAsset = iAssetData.FastGetAsset(true);
    if (!loadedAsset)
        return;
    
    UClass* entryClass = Cast<UClass>(loadedAsset);

    if (entryClass == UOdysseyPaletteEntryColor::StaticClass())
    {
        UOdysseyPaletteEntryColor* entry = NewObject< UOdysseyPaletteEntryColor >(mPalette.Get(), entryClass, NAME_None, RF_Public | RF_Transactional);
        entry->AddToRoot();
        entry->EntryName = FText::FromString("Name");
        mPalette.Get()->mPaletteEntries.Add(entry);
    }
    else if (entryClass == UOdysseyPaletteEntryMaterial::StaticClass())
    {
        UOdysseyPaletteEntryMaterial* entry = NewObject< UOdysseyPaletteEntryMaterial >(mPalette.Get(), entryClass, NAME_None, RF_Public | RF_Transactional);
        entry->AddToRoot();
        entry->EntryName = FText::FromString("Name");
        mPalette.Get()->mPaletteEntries.Add(entry);
    }
    else if (entryClass == UOdysseyPaletteEntryFolder::StaticClass())
    {
        UOdysseyPaletteEntryFolder* entry = NewObject< UOdysseyPaletteEntryFolder >(mPalette.Get(), entryClass, NAME_None, RF_Public | RF_Transactional);
        entry->AddToRoot();
        entry->EntryName = FText::FromString("Name");
        mPalette.Get()->mPaletteEntries.Add(entry);
    }
}

#undef LOCTEXT_NAMESPACE
