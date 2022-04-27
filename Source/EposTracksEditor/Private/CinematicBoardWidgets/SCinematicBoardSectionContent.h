// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/SCompoundWidget.h"

//---

class FCinematicBoardSection;
class SCinematicBoardSectionTitle;

class EPOSTRACKSEDITOR_API SCinematicBoardSectionContent
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionContent )
        {}
        //SLATE_EVENT(FOnDrop, OnDrop)
        //SLATE_EVENT(FOnDragEnter, OnDragEnter)
        //SLATE_EVENT(FOnDrop, OnDragOver)
        //SLATE_EVENT(FOnDragLeave, OnDragLeave)
        //SLATE_EVENT(FOnDragDetected, OnDragDetected)
    SLATE_END_ARGS()

    // Construct the widget
    void Construct(const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection);

    virtual void OnMouseEnter( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent );
    virtual void OnMouseLeave( const FPointerEvent& MouseEvent );
    //virtual FReply OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent);
    //virtual FReply OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent);
    //virtual void OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent);
    //virtual void OnDragLeave(const FDragDropEvent& DragDropEvent);
    //virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent);
    //virtual FReply OnDrop(const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent);
    //virtual FReply OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent);
    virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;

public:
    EVisibility OptionalWidgetsVisibility() const;

public:
    virtual void EnterRename();

private:
    TWeakPtr<FCinematicBoardSection> mBoardSection;

    TSharedPtr<SCinematicBoardSectionTitle> mWidgetTitle;

    EVisibility mOptionalWidgetsVisibility { EVisibility::Hidden };

    //FOnDrop mOnDrop;
    //FOnDragEnter mOnDragEnter;
    //FOnDrop mOnDragOver;
    //FOnDragLeave mOnDragLeave;
    //FOnDragDetected mOnDragDetected;
};
