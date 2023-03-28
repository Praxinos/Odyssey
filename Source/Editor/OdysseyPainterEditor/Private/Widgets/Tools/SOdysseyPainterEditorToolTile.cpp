// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Tools/SOdysseyPainterEditorToolTile.h"

#define LOCTEXT_NAMESPACE "SOdysseyPainterEditorToolTile"

SOdysseyPainterEditorToolTile::~SOdysseyPainterEditorToolTile()
{
}

SOdysseyPainterEditorToolTile::SOdysseyPainterEditorToolTile()
{
}

//CONSTRUCTION/DESTRUCTION-----------------------------------------------
void
SOdysseyPainterEditorToolTile::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
    mTool = InArgs._Tool;
    mOnSelected = InArgs._OnSelected;

    STableRow<UOdysseyPainterEditorTool*>::Construct(
        STableRow<UOdysseyPainterEditorTool*>::FArguments()
        .Content()
        [
            SNew(SButton)
		    .ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("Button.TransparentNoPadding"))
            .OnClicked(this, &SOdysseyPainterEditorToolTile::OnClicked)
            .IsEnabled(this, &SOdysseyPainterEditorToolTile::IsEnabled)
            [
                SNew(SImage)
                .Image(&mTool->Icon)
            ]
        ],
        InOwnerTableView
    );
}

FReply
SOdysseyPainterEditorToolTile::OnClicked()
{
    mOnSelected.ExecuteIfBound();
    return FReply::Handled();
}

bool
SOdysseyPainterEditorToolTile::IsEnabled() const
{
    return mTool->IsActivable();
}

#undef LOCTEXT_NAMESPACE
