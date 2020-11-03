// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Styling/SlateColor.h"
#include "Widgets/SWidget.h"
#include "Layout/Margin.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Text/STextBlock.h"
#include "Editor/Transactor.h"
#include "Widgets/Views/SListView.h"
#include "OdysseyStyleSet.h"


#define LOCTEXT_NAMESPACE "SOdysseyUndoHistoryTableRow"

/**
 * Implements a row widget for the undo history list for a painterEditor.
 */
class ODYSSEYWIDGETS_API SOdysseyUndoHistoryTableRow
    : public SMultiColumnTableRow<TSharedPtr<int32> >
{
public:

    SLATE_BEGIN_ARGS(SOdysseyUndoHistoryTableRow) { }
        SLATE_ATTRIBUTE(bool, IsApplied)
        SLATE_ARGUMENT(int32, QueueIndex)
        SLATE_ARGUMENT(FName, TransactionName)
    SLATE_END_ARGS()

public:

    /**
     * Constructs the widget.
     *
     * @param InArgs The construction arguments.
     */
    void Construct( const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView )
    {
        IsApplied = InArgs._IsApplied;
        QueueIndex = InArgs._QueueIndex;

        Title = FText::Format(LOCTEXT("UndoHistoryTableRowTitleF", "{0}"), FText::FromName( InArgs._TransactionName ) );

        SMultiColumnTableRow<TSharedPtr<int32> >::Construct(FSuperRowType::FArguments()
                                                            .Style(&FOdysseyStyle::GetWidgetStyle<FTableRowStyle>("OdysseyLayerStack.AlternatedRows")),
                                                             InOwnerTableView);
    }

public:

    virtual TSharedRef<SWidget> GenerateWidgetForColumn( const FName& ColumnName ) override
    {
        if (ColumnName == "Title")
        {
            return SNew(SBox)
                .Padding(FMargin(4.0f, 0.0f))
                [
                    SNew(STextBlock)
                        .ColorAndOpacity(this, &SOdysseyUndoHistoryTableRow::HandleTitleTextColorAndOpacity)
                        .Text(Title)
                ];
        }

        return SNullWidget::NullWidget;
    }

private:

    // Callback for getting the color of the 'Title' text.
    FSlateColor HandleTitleTextColorAndOpacity( ) const
    {
        if (IsApplied.Get())
        {
            return FSlateColor::UseForeground();
        }

        return FSlateColor::UseSubduedForeground();
    }

private:

    // Holds an attribute that determines whether the transaction in this row is applied.
    TAttribute<bool> IsApplied;

    // Holds the transaction's index in the transaction queue.
    int32 QueueIndex;

    // Holds the transaction's title text.
    FText Title;
};


#undef LOCTEXT_NAMESPACE
