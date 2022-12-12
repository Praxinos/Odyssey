// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Tools/SOdysseyPainterEditorToolsTileView.h"
#include "Widgets/Tools/SOdysseyPainterEditorToolTile.h"

#define LOCTEXT_NAMESPACE "SOdysseyPainterEditorToolsTileView"

SOdysseyPainterEditorToolsTileView::~SOdysseyPainterEditorToolsTileView()
{
}

SOdysseyPainterEditorToolsTileView::SOdysseyPainterEditorToolsTileView()
{
}

//CONSTRUCTION/DESTRUCTION-----------------------------------------------
void
SOdysseyPainterEditorToolsTileView::Construct(const FArguments& InArgs)
{
    mTools = InArgs._Tools;
    mOnToolSelected = InArgs._OnToolSelected;

    STileView<UOdysseyPainterEditorTool*>::Construct(
        STileView<UOdysseyPainterEditorTool*>::FArguments()
        .ItemWidth(32)
        .ItemHeight(32)
        .ListItemsSource(&mTools)
        .ItemAlignment(EListItemAlignment::LeftAligned)
        .SelectionMode(ESelectionMode::None)
        .OnGenerateTile(this, &SOdysseyPainterEditorToolsTileView::OnGenerateTile)
    );
}

TSharedRef<class ITableRow>
SOdysseyPainterEditorToolsTileView::OnGenerateTile(UOdysseyPainterEditorTool* iTool, const TSharedRef< class STableViewBase >& iTable)
{
    return SNew(SOdysseyPainterEditorToolTile, iTable)
        .Tool(iTool)
        .OnSelected(this, &SOdysseyPainterEditorToolsTileView::OnToolSelected, iTool);
}

void
SOdysseyPainterEditorToolsTileView::OnToolSelected(UOdysseyPainterEditorTool* iTool)
{
    mOnToolSelected.ExecuteIfBound(iTool);
}

#undef LOCTEXT_NAMESPACE
