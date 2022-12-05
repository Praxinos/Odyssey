// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Tools/SOdysseyToolsTileView.h"
#include "Widgets/Tools/SOdysseyToolTile.h"

#define LOCTEXT_NAMESPACE "SOdysseyToolsTileView"

SOdysseyToolsTileView::~SOdysseyToolsTileView()
{
}

SOdysseyToolsTileView::SOdysseyToolsTileView()
{
}

//CONSTRUCTION/DESTRUCTION-----------------------------------------------
void
SOdysseyToolsTileView::Construct(const FArguments& InArgs)
{
    mTools = InArgs._Tools;
    mOnToolSelected = InArgs._OnToolSelected;

    STileView<UOdysseyTool*>::Construct(
        STileView<UOdysseyTool*>::FArguments()
        .ItemWidth(32)
        .ItemHeight(32)
        .ListItemsSource(&mTools)
        .ItemAlignment(EListItemAlignment::LeftAligned)
        .SelectionMode(ESelectionMode::None)
        .OnGenerateTile(this, &SOdysseyToolsTileView::OnGenerateTile)
    );
}

TSharedRef<class ITableRow>
SOdysseyToolsTileView::OnGenerateTile(UOdysseyTool* iTool, const TSharedRef< class STableViewBase >& iTable)
{
    return SNew(SOdysseyToolTile, iTable)
        .Tool(iTool)
        .OnSelected(this, &SOdysseyToolsTileView::OnToolSelected, iTool);
}

void
SOdysseyToolsTileView::OnToolSelected(UOdysseyTool* iTool)
{
    mOnToolSelected.ExecuteIfBound(iTool);
}

#undef LOCTEXT_NAMESPACE
