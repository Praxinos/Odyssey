// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STableRow.h"
#include "OdysseyUndoHistory.h"


/**
 * Implements the undo history panel for the painter editor.
 */
class ODYSSEYWIDGETS_API SOdysseyUndoHistory
    : public SCompoundWidget
{
public:

    SLATE_BEGIN_ARGS(SOdysseyUndoHistory) { }
    SLATE_END_ARGS()

public:

    virtual ~SOdysseyUndoHistory();

    /**
     * Construct this widget
     *
     * @param InArgs The declaration data for this widget.
     */
    void Construct( const FArguments& InArgs, FOdysseyUndoHistory* InOdysseyUndoHistory );

public:

    //~ Begin SWidget Interface

    virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;

    //~ End SWidget Interface

protected:

    /**
     * Reloads the list of undo transactions.
     */
    void ReloadUndoList();
    void OnUndoBufferChanged();

private:

    // Select the last transaction in the undo history.
    void SelectLastTransaction();

    // Callback for clicking the 'Discard History' button.
    FReply HandleDiscardHistoryButtonClicked( );

    // Callback for generating a row widget for the message list view.
    TSharedRef<ITableRow> HandleUndoListGenerateRow( TSharedPtr<FOdysseyTransaction> TransactionInfo, const TSharedRef<STableViewBase>& OwnerTable );

    // Callback for checking whether the specified undo list row transaction is applied.
    bool HandleUndoListRowIsApplied( int32 QueueIndex ) const;

    // Callback for selecting a message in the message list view.
    void HandleUndoListSelectionChanged( TSharedPtr<FOdysseyTransaction> TransactionInfo, ESelectInfo::Type SelectInfo );

    // Callback for getting the undo size text.
    FText HandleUndoSizeTextBlockText( ) const;

private:
    // Holds the undo list view.
    TSharedPtr<SListView<TSharedPtr<FOdysseyTransaction> > > UndoListView;

    FOdysseyUndoHistory* UndoHistory;
};
