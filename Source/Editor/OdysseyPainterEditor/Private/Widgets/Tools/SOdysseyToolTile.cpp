// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Tools/SOdysseyToolTile.h"

#define LOCTEXT_NAMESPACE "SOdysseyToolTile"

SOdysseyToolTile::~SOdysseyToolTile()
{
}

SOdysseyToolTile::SOdysseyToolTile()
{
}

//CONSTRUCTION/DESTRUCTION-----------------------------------------------
void
SOdysseyToolTile::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
    UOdysseyTool* tool = InArgs._Tool;
    mOnSelected = InArgs._OnSelected;

    STableRow<UOdysseyTool*>::Construct(
        STableRow<UOdysseyTool*>::FArguments()
        .Content()
        [
            SNew(SButton)
		    .ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("Button.TransparentNoPadding"))
            .OnClicked(this, &SOdysseyToolTile::OnClicked)
            [
                SNew(SImage)
                .Image(&tool->Icon)
            ]
        ],
        InOwnerTableView
    );
}

FReply
SOdysseyToolTile::OnClicked()
{
    mOnSelected.ExecuteIfBound();
    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
