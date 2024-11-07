// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

/* class FOdysseyAnimationEditorExtension;
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineCellSelection
    : public SCompoundWidget
{
public:
    DECLARE_DELEGATE_RetVal(FReply, FOnDragged)

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineCellSelection)
        : _Content()
    {}
        SLATE_DEFAULT_SLOT( FArguments, Content )
        SLATE_ARGUMENT(TSharedPtr<FOdysseyAnimationCell>, Cell)
        SLATE_EVENT(FOnDragged, OnDragged)
    SLATE_END_ARGS()

    SOdysseyAnimationTimelineCellSelection();

    void Construct(
        const FArguments& InArgs,
        FOdysseyAnimationEditorExtension* iExtension);

    // SWidget interface
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
    // End of SWidget interfacepublic:

private:
    bool IsSelected() const;
    bool IsSelectionCursor() const;

    FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    FReply OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) override;

private:
    TAttribute<TArray<TSharedPtr<FOdysseyAnimationCell>>> mSelectedCells;

    FOdysseyAnimationEditorExtension* mExtension;
    TSharedPtr<FOdysseyAnimationCell> mCell;
    FOnDragged mOnDragged;
}; */
