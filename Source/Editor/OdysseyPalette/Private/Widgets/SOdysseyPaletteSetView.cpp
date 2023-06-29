// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyPaletteSetView.h"
#include "SOdysseyPaletteSetTile.h"

#define LOCTEXT_NAMESPACE "SOdysseyPaletteSetView"

SOdysseyPaletteSetView::~SOdysseyPaletteSetView()
{
}

SOdysseyPaletteSetView::SOdysseyPaletteSetView()
{
}

//CONSTRUCTION/DESTRUCTION-----------------------------------------------
void
SOdysseyPaletteSetView::Construct(const FArguments& InArgs)
{
    mPalette = InArgs._Palette;
    mOnSetSelected = InArgs._OnSetSelected;

    if (mPalette)
    {
        STileView<FName>::Construct(
            STileView<FName>::FArguments()
            .ItemWidth(32)
            .ItemHeight(32)
            .ListItemsSource(&(mPalette->Sets))
            .ItemAlignment(EListItemAlignment::LeftAligned)
            .SelectionMode(ESelectionMode::Single)
            .OnGenerateTile(this, &SOdysseyPaletteSetView::OnGenerateTile)
        );
    }
}

TSharedRef<class ITableRow>
SOdysseyPaletteSetView::OnGenerateTile(FName iSet, const TSharedRef< class STableViewBase >& iTable)
{
    return SNew(SOdysseyPaletteSetTile, iTable)
        .UsedSet(iSet) //All sets one after the other
        .OnSelected(this, &SOdysseyPaletteSetView::OnSetSelected, iSet);
}

void
SOdysseyPaletteSetView::OnSetSelected(FName iSet)
{
    RequestListRefresh();
    Private_ClearSelection();
    Private_SetItemSelection(iSet, true, true);
    mOnSetSelected.ExecuteIfBound(iSet);
}

#undef LOCTEXT_NAMESPACE
