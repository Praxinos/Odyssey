// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "SOdysseyDropWidgetEvents.h"

class ODYSSEYWIDGETS_API SOdysseyDropWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyDropWidget)
        {}
        SLATE_DEFAULT_SLOT(FArguments, Content)
        SLATE_EVENT(FOnDrop, OnDrop)
        SLATE_EVENT(FOnDragEnter, OnDragEnter)
        SLATE_EVENT(FOnDrop, OnDragOver)
        SLATE_EVENT(FOnDragLeave, OnDragLeave)
    SLATE_END_ARGS()

    // Construct the widget
    void Construct(const FArguments& InArgs);
    virtual void OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent);
    virtual void OnDragLeave(const FDragDropEvent& DragDropEvent);
    virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent);
    virtual FReply OnDrop(const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent);

private:
    FOnDrop mOnDrop;
    FOnDragEnter mOnDragEnter;
    FOnDrop mOnDragOver;
    FOnDragLeave mOnDragLeave;
};
