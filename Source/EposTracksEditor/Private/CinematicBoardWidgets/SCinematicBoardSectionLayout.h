// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "Widgets/SCompoundWidget.h"

//---

class FCinematicBoardSection;

class EPOSTRACKSEDITOR_API SCinematicBoardSectionLayout
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionLayout )
        {}
        SLATE_NAMED_SLOT( FArguments, Title )
        SLATE_NAMED_SLOT( FArguments, Camera )
        SLATE_NAMED_SLOT( FArguments, Thumbnails )
        SLATE_NAMED_SLOT( FArguments, Planes )
        //SLATE_EVENT(FOnDrop, OnDrop)
        //SLATE_EVENT(FOnDragEnter, OnDragEnter)
        //SLATE_EVENT(FOnDrop, OnDragOver)
        //SLATE_EVENT(FOnDragLeave, OnDragLeave)
        //SLATE_EVENT(FOnDragDetected, OnDragDetected)
    SLATE_END_ARGS()

    // Construct the widget
    void Construct(const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection);
    //virtual FReply OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent);
    //virtual FReply OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent);
    //virtual void OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent);
    //virtual void OnDragLeave(const FDragDropEvent& DragDropEvent);
    //virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent);
    //virtual FReply OnDrop(const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent);
    //virtual FReply OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent);
    virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;

public:

private:
    TWeakPtr<FCinematicBoardSection> mBoardSection;

    //FOnDrop mOnDrop;
    //FOnDragEnter mOnDragEnter;
    //FOnDrop mOnDragOver;
    //FOnDragLeave mOnDragLeave;
    //FOnDragDetected mOnDragDetected;
};
