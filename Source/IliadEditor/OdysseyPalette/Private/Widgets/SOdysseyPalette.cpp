// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "SOdysseyPalette.h"

#include "OdysseyPalette.h"

#include "SOdysseyPaletteTreeView.h"
#include "Widgets/SOdysseyPaletteSetComboBox.h"

#include "PropertyCustomizationHelpers.h"

#define LOCTEXT_NAMESPACE "OdysseyPalette"

SLATE_IMPLEMENT_WIDGET(SOdysseyPalette)
void
SOdysseyPalette::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
    SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, mPalette, EInvalidateWidgetReason::None);
}

/////////////////////////////////////////////////////
// SOdysseyPalette
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

SOdysseyPalette::SOdysseyPalette()
    : mPalette(*this, nullptr)
{
}

void SOdysseyPalette::Construct(const FArguments& InArgs)
{
    mPalette.Assign(*this, InArgs._Palette);

    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            .Visibility(this, &SOdysseyPalette::GetTreeViewVisibility)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(FMargin(0, 0, 4, 0))
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("palette.set.name", "Set"))
            ]
            + SHorizontalBox::Slot()
            [
                SNew(SOdysseyPaletteSetComboBox)
                .Palette(InArgs._Palette)
                .CurrentSet(InArgs._CurrentSet)
                .OnCurrentSetSelected(InArgs._OnCurrentSetSelected)
            ]
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SOdysseyPaletteTreeView)
            .Visibility(this, &SOdysseyPalette::GetTreeViewVisibility)
            .Palette(InArgs._Palette)
            .CurrentColorEntry(InArgs._CurrentColorEntry)
            .OnCurrentColorEntryChanged(InArgs._OnCurrentColorEntryChanged)
        ]
    ];
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Getter / Setter

EVisibility
SOdysseyPalette::GetTreeViewVisibility() const
{
    return mPalette.Get() ? EVisibility::Visible : EVisibility::Collapsed;
}

#undef LOCTEXT_NAMESPACE
