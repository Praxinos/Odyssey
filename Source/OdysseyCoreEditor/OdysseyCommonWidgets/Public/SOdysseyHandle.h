// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"

class ODYSSEYCOMMONWIDGETS_API SOdysseyHandle : public SCompoundWidget
{
public:
    DECLARE_DELEGATE_TwoParams(FOnDragEvent, const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)

public:
    SLATE_BEGIN_ARGS(SOdysseyHandle)
    {}
        SLATE_DEFAULT_SLOT(FArguments, Content)
        SLATE_EVENT(FOnDragEvent, OnDragStarted)
        SLATE_EVENT(FOnDragEvent, OnDragged)
        SLATE_EVENT(FOnDragEvent, OnDragStopped)
    SLATE_END_ARGS()

public:
    // Construct the widget
    void Construct(const FArguments& InArgs);

public:
    //SWidget overrides
    virtual FReply OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) override;
    virtual FReply OnMouseMove(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) override;

protected:
    virtual TOptional<EMouseCursor::Type> GetCursor() const override;

private:
    bool mIsDragging;
    FOnDragEvent mOnDragStarted;
    FOnDragEvent mOnDragged;
    FOnDragEvent mOnDragStopped;
};
