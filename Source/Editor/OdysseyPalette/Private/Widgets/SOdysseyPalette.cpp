// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyPalette.h"
#include "Widgets/Colors/SColorBlock.h"
#include "SOdysseyPaletteAddEntryButton.h"
#include "SOdysseyPaletteEntryRow.h"
#include "PropertyCustomizationHelpers.h"
#include "SOdysseyPaletteFolderRow.h"
#include "SOdysseyPaletteColorRow.h"
#include "OdysseyPaletteEntryColor.h"
#include "OdysseyPaletteEntryFolder.h"

/////////////////////////////////////////////////////
// SOdysseyPalette
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
SOdysseyPalette::~SOdysseyPalette()
{
    mColorPalette.Reset();
}


void SOdysseyPalette::Construct(const FArguments& InArgs)
{
    mColorPalette = MakeShareable(new FOdysseyPalette());
    mAssetThumbnailPool = MakeShareable(new FAssetThumbnailPool(1024));

    TArray<const UClass*> allowedClasses;
    allowedClasses.Add(UOdysseyPalette::StaticClass());

    ChildSlot
    [
        SNew(SScrollBox)
        .Orientation(Orient_Vertical)
        .ScrollBarAlwaysVisible(false)
        + SScrollBox::Slot()
        [
            SNew(SObjectPropertyEntryBox)
            .AllowedClass(UOdysseyPalette::StaticClass())
            .ObjectPath(this, &SOdysseyPalette::ObjectPath)
            .ThumbnailPool(mAssetThumbnailPool)
            .OnObjectChanged(this, &SOdysseyPalette::OnObjectChanged)
            .AllowClear(true)
            .DisplayUseSelected(false)
            .DisplayBrowse(false)
            .DisplayThumbnail(false)
            .EnableContentPicker(true)
            .DisplayCompactSize(true)
            .DisplayThumbnail(true)
            .ThumbnailSizeOverride(FIntPoint(32, 32))
        ]
        + SScrollBox::Slot()
        .Expose(mColorPaletteSlot)
        [
            SNullWidget::NullWidget
        ]
    ];

    if( mColorPalette->GetPalette() )
        mColorPaletteSlot->AttachWidget( CreateColorPaletteWidget());
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Getter / Setter

FOdysseyPalette* SOdysseyPalette::GetColorPalette() const
{
    return mColorPalette.Get();
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Private internal callbacks

void SOdysseyPalette::OnObjectChanged(const FAssetData& AssetData)
{
    if (AssetData.IsValid())
        mColorPalette->SetPalette( CastChecked< UOdysseyPalette >(AssetData.GetAsset()));
    else
        mColorPalette->SetPalette(nullptr);

    mColorPaletteSlot->DetachWidget();

    if(mColorPalette->GetPalette())
        mColorPaletteSlot->AttachWidget(CreateColorPaletteWidget());
}


FString SOdysseyPalette::ObjectPath() const
{
    if (!mColorPalette)
        return FString();

    return mColorPalette->GetPalette()->GetPathName();
}


TSharedRef<SWidget> SOdysseyPalette::CreateColorPaletteWidget()
{
    return
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SAssignNew(mPaletteTreeView, SOdysseyPaletteTreeView)
            .Palette(mColorPalette->GetPalette())
            .OnGenerateRow(this, &SOdysseyPalette::OnGenerateRow)
        ]
        + SVerticalBox::Slot()
        .Padding(FMargin(0, 5, 0, 0))
        .HAlign(HAlign_Left)
        .AutoHeight()
        [
            SNew(SOdysseyPaletteAddEntryButton)
            .Palette(mColorPalette->GetPalette())
        ];
}

TSharedRef<ITableRow> SOdysseyPalette::OnGenerateRow(UOdysseyPaletteEntry* iEntry, const TSharedRef<STableViewBase>& iOwnerTable)
{
    check(iEntry);

    UClass* entryClass = iEntry->GetClass();
    
    if (entryClass == UOdysseyPaletteEntryColor::StaticClass())
    {
        return SNew(SOdysseyPaletteColorRow, mPaletteTreeView.ToSharedRef(), Cast<UOdysseyPaletteEntryColor>(iEntry));
    }
    else if (entryClass == UOdysseyPaletteEntryFolder::StaticClass())
    {
        return SNew(SOdysseyPaletteFolderRow, mPaletteTreeView.ToSharedRef(), Cast<UOdysseyPaletteEntryFolder>(iEntry));
    }

    return SNew(SOdysseyPaletteEntryRow, mPaletteTreeView.ToSharedRef(), Cast<UOdysseyPaletteEntry>(iEntry)); //Default widget
}
