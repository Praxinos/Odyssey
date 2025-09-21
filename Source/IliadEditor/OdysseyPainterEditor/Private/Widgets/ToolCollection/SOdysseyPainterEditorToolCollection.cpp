// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/ToolCollection/SOdysseyPainterEditorToolCollection.h"

#include "OdysseyPainterEditor.h"
#include "ToolCollection/OdysseyToolCollection.h"
#include "Widgets/Views/STileView.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "ToolCollection/OdysseyToolCollectionDragDropOp.h"
#include "SOdysseyPainterEditorToolTile.h"

SOdysseyPainterEditorToolCollection::~SOdysseyPainterEditorToolCollection()
{
    if (mToolCollection.Get())
    {
        mToolCollection.Get()->OnCollectionChanged.RemoveAll(this);
    }
}

SOdysseyPainterEditorToolCollection::SOdysseyPainterEditorToolCollection()
{}

void
SOdysseyPainterEditorToolCollection::Construct(const FArguments& InArgs)
{
    mEditor = InArgs._Editor;
    mToolCollection = InArgs._ToolCollection;

    FOdysseyPainterEditor* editor = mEditor.Get();
    UOdysseyToolCollection* toolCollection = mToolCollection.Get();

    if (!editor || !toolCollection)
        return;

    toolCollection->OnCollectionChanged.AddSP(this, &SOdysseyPainterEditorToolCollection::HandleToolsChanged);

    ChildSlot
        [
            SNew(SExpandableArea)
                .InitiallyCollapsed(false)
                .AreaTitle(this, &SOdysseyPainterEditorToolCollection::GetCollectionDisplayName)
                .BodyContent()
                [
                    SAssignNew(mToolListView, STileView<UOdysseyPainterEditorTool*>)
                        .ListItemsSource(&(mToolCollection.Get()->GetTools()))
                        .OnGenerateTile(this, &SOdysseyPainterEditorToolCollection::GenerateToolTile)
                        .ItemHeight(32)
                        .ItemWidth(32)
                ]
        ];
}


TSharedRef<ITableRow> SOdysseyPainterEditorToolCollection::GenerateToolTile(UOdysseyPainterEditorTool* Tool, const TSharedRef<STableViewBase>& OwnerTable)
{
    return SNew(SOdysseyPainterEditorToolTile, OwnerTable)
            .Tool( Tool );
        //.OnDragDetected(this, &SOdysseyPainterEditorToolCollection::OnToolDragDetected, Tool)
        //.OnCanAcceptDrop(this, &SOdysseyPainterEditorToolCollection::OnRowCanAcceptDrop)
        //.OnAcceptDrop(this, &SOdysseyPainterEditorToolCollection::OnRowAcceptDrop)
        /*[
            SNew(SImage).Image(GetToolIcon(Tool))
        ];*/
}

FText
SOdysseyPainterEditorToolCollection::GetCollectionDisplayName() const
{
    return mToolCollection.Get() && !mToolCollection.Get()->IsCollectionTransient() ? FText::FromString(mToolCollection.Get()->GetName())
                                 : FText::FromString(TEXT("Recent Tools"));
}

const FSlateBrush*
SOdysseyPainterEditorToolCollection::GetToolIcon(UOdysseyPainterEditorTool* iTool) const
{
    if( !iTool )
        return FAppStyle::GetBrush("ClassIcon.Default");
    else
        return &iTool->Icon;
}

FReply
SOdysseyPainterEditorToolCollection::OnAddToolClicked()
{
    return FReply::Handled();
}

void
SOdysseyPainterEditorToolCollection::HandleToolsChanged()
{
    if (mToolListView.IsValid())
    {
        mToolListView->RequestListRefresh();
    }
}
