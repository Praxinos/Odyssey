// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Input/Reply.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"

class ITableRow;
class STableViewBase;
class UStoryNote;

/** A custom widget that displays all notes */
class SNotesInViewport
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SNotesInViewport )
        {}
        SLATE_ARGUMENT( const TArray<TWeakObjectPtr<UStoryNote>>*, ListItemsSource )
    SLATE_END_ARGS()

    /** Construct this widget */
    void Construct(const FArguments& InArgs);

public:
    void RefreshList();

private:
    TSharedRef<ITableRow> MakeNoteRow( TWeakObjectPtr<UStoryNote> iItem, const TSharedRef<STableViewBase>& iOwnerTable );

private:
    TSharedPtr<SListView<TWeakObjectPtr<UStoryNote>>> mWidgetList;
};

//---

/** A custom widget that displays all notes */
class SNotesAsOverlay
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SNotesAsOverlay )
        {}
        SLATE_ARGUMENT( const TArray<TWeakObjectPtr<UStoryNote>>*, ListItemsSource )
    SLATE_END_ARGS()

    /** Construct this widget */
    void Construct(const FArguments& InArgs);

public:
    void RefreshList();

private:
    TSharedRef<ITableRow> MakeNoteRow( TWeakObjectPtr<UStoryNote> iItem, const TSharedRef<STableViewBase>& iOwnerTable );

private:
    TSharedPtr<SListView<TWeakObjectPtr<UStoryNote>>> mWidgetList;
};
