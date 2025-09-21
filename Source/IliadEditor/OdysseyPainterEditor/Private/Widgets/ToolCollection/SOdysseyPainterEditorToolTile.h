// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/STileView.h"

#include "Tools/OdysseyPainterEditorTool.h"

enum class EDropIndicatorSide
{
    None,
    Left,
    Right
};

class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorToolTile
    : public STableRow<UOdysseyPainterEditorTool*>
{
/*
public:
    DECLARE_DELEGATE_OneParam( FOnToolSelected, UOdysseyPainterEditorTool* );*/

public:
    SLATE_BEGIN_ARGS(SOdysseyPainterEditorToolTile)
        {}
        SLATE_ARGUMENT( UOdysseyPainterEditorTool*, Tool )
        SLATE_ARGUMENT( UOdysseyToolCollection*, ToolCollection)
        //SLATE_EVENT( FOnToolSelected, OnToolSelected )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyPainterEditorToolTile();
    SOdysseyPainterEditorToolTile();

    void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTable);

public:
    virtual FReply OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
    virtual void OnDragLeave(const FDragDropEvent& DragDropEvent) override;
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

private:
    void OnToolCheckStateChanged(ECheckBoxState InValue, UOdysseyPainterEditorTool* iTool);
    EVisibility ToolVisibility(UOdysseyPainterEditorTool* iTool) const;
    ECheckBoxState IsToolChecked(UOdysseyPainterEditorTool* iTool) const;
    FText ToolTooltip(UOdysseyPainterEditorTool* iTool) const;

private:
    UOdysseyPainterEditorTool* mTool = nullptr;
    UOdysseyToolCollection* mCollection = nullptr;
    mutable EDropIndicatorSide mDropSide = EDropIndicatorSide::None;
    //FOnToolSelected mOnToolSelected;
};
