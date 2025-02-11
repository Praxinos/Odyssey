// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "SOdysseyPalette.h"

#include "OdysseyPalette.h"

#include "SOdysseyPaletteTreeView.h"

#include "PropertyCustomizationHelpers.h"

SLATE_IMPLEMENT_WIDGET(SOdysseyPalette)
void
SOdysseyPalette::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
    SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, mPaletteAttribute, EInvalidateWidgetReason::None)
    .OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda(
        [](SWidget& Widget)
        {
            static_cast<SOdysseyPalette&>(Widget).OnPaletteChanged();
        }
    ));
}

/////////////////////////////////////////////////////
// SOdysseyPalette
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

SOdysseyPalette::SOdysseyPalette()
    : mPaletteAttribute(*this, nullptr)
    , mPalette(nullptr)
{
}

void SOdysseyPalette::Construct(const FArguments& InArgs)
{
    mPaletteAttribute.Assign(*this, InArgs._Palette);
    mPalette = mPaletteAttribute.Get();
    mOnPaletteChanged = InArgs._OnPaletteChanged;
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
            .AllowCreate(true)
            .DisplayUseSelected(false)
            .DisplayBrowse(false)
            .DisplayThumbnail(false)
            .EnableContentPicker(true)
            .DisplayCompactSize(true)
            .DisplayThumbnail(true)
            .ThumbnailSizeOverride(FIntPoint(32, 32))
        ]
        + SScrollBox::Slot()
        [
            SNew(SOdysseyPaletteTreeView)
            .Visibility(this, &SOdysseyPalette::GetTreeViewVisibility)
            .Palette(InArgs._Palette)
            .CurrentColorEntry(InArgs._CurrentColorEntry)
            .CurrentSet(InArgs._CurrentSet)
            .OnCurrentColorEntrySelected(InArgs._OnCurrentColorEntrySelected)
            .OnCurrentSetSelected(InArgs._OnCurrentSetSelected)
        ]
    ];
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Getter / Setter

EVisibility
SOdysseyPalette::GetTreeViewVisibility() const
{
    return mPalette ? EVisibility::Visible : EVisibility::Collapsed;
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Private internal callbacks

void
SOdysseyPalette::OnObjectChanged(const FAssetData& AssetData)
{
    if (!AssetData.IsValid())
    {

        mPalette = nullptr;
        return;
    }

    mPalette = CastChecked< UOdysseyPalette >(AssetData.GetAsset());
}


FString
SOdysseyPalette::ObjectPath() const
{
    if (!mPalette)
        return FString();

    return mPalette->GetPathName();
}

void
SOdysseyPalette::OnPaletteChanged()
{
    mPalette = mPaletteAttribute.Get();
}
