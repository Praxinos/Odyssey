// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class ODYSSEYCOREEDITOR_API SOdysseyEvents
    : public SCompoundWidget
{
public:
    DECLARE_DELEGATE_RetVal_TwoParams(FReply, FOnDragDetected, const FGeometry&, const FDragDropEvent&)
    DECLARE_DELEGATE_RetVal_TwoParams(FReply, FOnDragOver, const FGeometry&, const FDragDropEvent&)
    DECLARE_DELEGATE_RetVal_TwoParams(FReply, FOnDrop, const FGeometry&, const FDragDropEvent&)

    DECLARE_DELEGATE_TwoParams(FOnDragEnter, const FGeometry&, const FDragDropEvent&)
    DECLARE_DELEGATE_OneParam(FOnDragLeave, const FDragDropEvent&)

public:
    SLATE_BEGIN_ARGS(SOdysseyEvents)
        {}

        SLATE_DEFAULT_SLOT( FArguments, Content )

        SLATE_EVENT( FPointerEventHandler, OnMouseButtonDown )
        SLATE_EVENT( FPointerEventHandler, OnMouseButtonUp )
        SLATE_EVENT( FPointerEventHandler, OnMouseMove )
        SLATE_EVENT( FPointerEventHandler, OnMouseButtonDoubleClick )

        SLATE_EVENT( FPointerEventHandler, OnDragDetected )
        SLATE_EVENT( FOnDragEnter, OnDragEnter )
        SLATE_EVENT( FOnDragOver, OnDragOver )
        SLATE_EVENT( FOnDrop, OnDrop )
        SLATE_EVENT( FOnDragLeave, OnDragLeave )

    SLATE_END_ARGS()

public:
    void Construct(const FArguments& iArgs);

private:
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

    virtual FReply OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual void OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
    virtual void OnDragLeave(const FDragDropEvent& DragDropEvent) override;
    virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
    virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;

private:
    FPointerEventHandler mOnMouseButtonDown;
    FPointerEventHandler mOnMouseButtonUp;
    FPointerEventHandler mOnMouseMove;
    FPointerEventHandler mOnMouseButtonDoubleClick;

    FPointerEventHandler mOnDragDetected;
    FOnDragEnter mOnDragEnter;
    FOnDragOver mOnDragOver;
    FOnDrop mOnDrop;
    FOnDragLeave mOnDragLeave;
};
