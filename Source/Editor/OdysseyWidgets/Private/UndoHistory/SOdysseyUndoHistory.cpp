// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "UndoHistory/SOdysseyUndoHistory.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "EditorStyleSet.h"
#include "Editor.h"
#include "Widgets/Views/SListView.h"
#include "UndoHistory/SOdysseyUndoHistoryTableRow.h"
#include "Editor/TransBuffer.h"
#include "OdysseyTransaction.h"


#define LOCTEXT_NAMESPACE "SOdysseyUndoHistory"


/* SUndoHistory interface
 *****************************************************************************/

void SOdysseyUndoHistory::Construct( const FArguments& InArgs, FOdysseyUndoHistory* InOdysseyUndoHistory )
{
    UndoHistory = InOdysseyUndoHistory;
    ChildSlot
    [
        SNew(SVerticalBox)

        + SVerticalBox::Slot()
            .FillHeight(1.0f)
            [
                SNew(SBorder)
                    .BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
                    .Padding(FMargin(0.0f, 4.0f))
                    [
                        SAssignNew(UndoListView, SListView<TSharedPtr<FOdysseyTransaction> >)
                            .ItemHeight(24.0f)
                            .ListItemsSource(UndoHistory->GetTransactionsList())
                            .SelectionMode(ESelectionMode::Single)
                            .OnGenerateRow(this, &SOdysseyUndoHistory::HandleUndoListGenerateRow)
                            .OnSelectionChanged(this, &SOdysseyUndoHistory::HandleUndoListSelectionChanged)
                            .HeaderRow
                            (
                                SNew(SHeaderRow)
                                    .Visibility(EVisibility::Collapsed)

                                + SHeaderRow::Column("Title")
                            )
                    ]
            ]

        + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.0f, 4.0f, 0.0f, 0.0f)
            [
                SNew(SHorizontalBox)

                + SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    .VAlign(VAlign_Center)
                    [
                        // undo size
                        SNew(STextBlock)
                            .Text(this, &SOdysseyUndoHistory::HandleUndoSizeTextBlockText)
                    ]

                + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(8.0f, 0.0f, 0.0f, 0.0f)
                    [
                        // discard history button
                        SNew(SButton)
                            .ButtonStyle(FCoreStyle::Get(), "NoBorder")
                            .OnClicked(this, &SOdysseyUndoHistory::HandleDiscardHistoryButtonClicked)
                            .ToolTipText(LOCTEXT("DiscardHistoryButtonToolTip", "Discard the Undo History."))
                            [
                                SNew(SImage)
                                    .Image(FEditorStyle::Get().GetBrush("TrashCan_Small"))
                            ]
                    ]
            ]
    ];

    ReloadUndoList();
}

SOdysseyUndoHistory::~SOdysseyUndoHistory()
{
    if( UndoListView.IsValid() )
        UndoListView.Reset();
}

void SOdysseyUndoHistory::OnUndoBufferChanged()
{
    ReloadUndoList();

    SelectLastTransaction();
}

void SOdysseyUndoHistory::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
    UndoListView->RequestListRefresh();
}


/* SUndoHistory implementation
 *****************************************************************************/

void SOdysseyUndoHistory::ReloadUndoList( )
{
}

void SOdysseyUndoHistory::SelectLastTransaction()
{
    if( UndoHistory->GetCurrentTransactionIndex() != -1 )
        UndoListView->SetSelection(UndoHistory->GetTransactionsList()->GetData()[UndoHistory->GetCurrentTransactionIndex()]);
}


/* SUndoHistory callbacks
 *****************************************************************************/

FReply SOdysseyUndoHistory::HandleDiscardHistoryButtonClicked( )
{
    UndoHistory->DeleteAllTransactions();
    ReloadUndoList();

    return FReply::Handled();
}


TSharedRef<ITableRow> SOdysseyUndoHistory::HandleUndoListGenerateRow( TSharedPtr<FOdysseyTransaction> TransactionInfo, const TSharedRef<STableViewBase>& OwnerTable )
{

    UndoHistory->DeleteTransactionsAfterIndex( TransactionInfo->GetQueueIndex() );

    return SNew(SOdysseyUndoHistoryTableRow, OwnerTable)
        .IsApplied(this, &SOdysseyUndoHistory::HandleUndoListRowIsApplied, TransactionInfo->GetQueueIndex())
        .QueueIndex( TransactionInfo->GetQueueIndex() )
        .TransactionName( TransactionInfo->GetName() );

        //this->Private_SetItemSelection( SelectedNode.ToSharedRef(), true );

}


bool SOdysseyUndoHistory::HandleUndoListRowIsApplied( int32 QueueIndex ) const
{
    return ( QueueIndex <= UndoHistory->GetCurrentTransactionIndex() );
}


void SOdysseyUndoHistory::HandleUndoListSelectionChanged( TSharedPtr<FOdysseyTransaction> InItem, ESelectInfo::Type SelectInfo )
{
    UndoHistory->GetCurrentTransactionIndex();
    int selectedTransactionIndex = -1;

    for (int i = 0; i < UndoHistory->GetTransactionsList()->Num(); i++)
    {
        if( InItem == (UndoHistory->GetTransactionsList()->GetData()[i]) )
        {
            selectedTransactionIndex = i;
            break;
        }
    }

    if( selectedTransactionIndex != -1 )
    {
        if( UndoHistory->GetCurrentTransactionIndex() != selectedTransactionIndex )
            UndoHistory->TransactionBetweenIndexes( UndoHistory->GetCurrentTransactionIndex(), selectedTransactionIndex );


        UndoHistory->SetCurrentTransactionIndex( selectedTransactionIndex );
    }

}


FText SOdysseyUndoHistory::HandleUndoSizeTextBlockText( ) const
{
    if ((GEditor == nullptr) || (GEditor->Trans == nullptr))
    {
        return FText::GetEmpty();
    }

    return FText::Format(LOCTEXT("TransactionCountF", " Transactions ({0})"), FText::AsMemory(GEditor->Trans->GetUndoSize()));
}


#undef LOCTEXT_NAMESPACE
