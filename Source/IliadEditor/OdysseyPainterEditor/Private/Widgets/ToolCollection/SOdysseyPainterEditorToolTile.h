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
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyPainterEditorToolTile)
        {}
        SLATE_ARGUMENT( UOdysseyPainterEditorTool*, Tool )
        SLATE_ARGUMENT( UOdysseyToolCollection*, ToolCollection)
        SLATE_ARGUMENT( FOdysseyPainterEditor*, Editor)
        //SLATE_EVENT( FOnToolSelected, OnToolSelected )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyPainterEditorToolTile();
    SOdysseyPainterEditorToolTile();

    void Construct(const FArguments& InArgs);

public:
    FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
    FReply OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

    virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
    virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
    virtual void OnDragLeave(const FDragDropEvent& DragDropEvent) override;

    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

private:
    FLinearColor GetTileColor() const;
    TSharedRef<SWidget> BuildContextMenu();
    bool CanDeleteTool() const;
    void OnDeleteTool();
    bool CanDuplicateTool() const;
    void OnDuplicateTool();
    bool CanChangeIcon() const;
    void OnChangeIcon();

    void OnTextureSelected(const FAssetData& AssetData);
    void OnStyleIconSelected(FName StyleIconName);

    /*
private:
    void OnToolCheckStateChanged(ECheckBoxState InValue, UOdysseyPainterEditorTool* iTool);
    EVisibility ToolVisibility(UOdysseyPainterEditorTool* iTool) const;
    ECheckBoxState IsToolChecked(UOdysseyPainterEditorTool* iTool) const;
    FText ToolTooltip(UOdysseyPainterEditorTool* iTool) const;*/

private:
    UOdysseyPainterEditorTool* mTool = nullptr;
    UOdysseyToolCollection* mCollection = nullptr;
    FOdysseyPainterEditor* mEditor = nullptr;

    mutable EDropIndicatorSide mDropSide = EDropIndicatorSide::None;
    //FOnToolSelected mOnToolSelected;

    TWeakPtr<SWindow> PickerWindowPtr;
};
