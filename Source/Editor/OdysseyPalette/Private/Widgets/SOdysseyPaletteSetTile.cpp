// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyPaletteSetTile.h"
#include "OdysseyStyleSet.h"

SOdysseyPaletteSetTile::~SOdysseyPaletteSetTile()
{
}

SOdysseyPaletteSetTile::SOdysseyPaletteSetTile()
{
}

//CONSTRUCTION/DESTRUCTION-----------------------------------------------

void
SOdysseyPaletteSetTile::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
    mUsedSet = InArgs._UsedSet;
    mOnSelected = InArgs._OnSelected;

    TSharedRef<STextBlock> TextBlock = SNew(STextBlock)
        .Justification(ETextJustify::Center) // Align the text to the center
        .Text(FText::FromName(mUsedSet));

    STableRow<FName>::Construct(
        STableRow<FName>::FArguments()
        .Content()
        [
            SNew(SButton)
		    .ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("Button.TransparentNoPadding"))
            .OnClicked(this, &SOdysseyPaletteSetTile::OnClicked)
            .IsEnabled(this, &SOdysseyPaletteSetTile::IsEnabled)
            .Content()
            [
                TextBlock
            ]
        ],
        InOwnerTableView
    );
}

FReply
SOdysseyPaletteSetTile::OnClicked()
{
    mOnSelected.ExecuteIfBound();
    return FReply::Handled();
}

bool
SOdysseyPaletteSetTile::IsEnabled() const
{
    return true;
}
