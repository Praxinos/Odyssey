// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"

class FOdysseyAnimationEditorExtension;
class UOdysseyAnimationLayer;
class FOdysseyAnimationTimelineCellsShortcuts;
class FOdysseyAnimationTimelineCellImageStaggerShortcuts;

/**
 * Implements a layer row widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerImageTimeline
    : public SCompoundWidget
{
public:
    // Construction / Destruction
    virtual ~SOdysseyAnimationLayerImageTimeline();
    SOdysseyAnimationLayerImageTimeline();

    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerImageTimeline)
        : _DisplayOptions(false)
        {}
        SLATE_ATTRIBUTE(bool, DisplayOptions)
    SLATE_END_ARGS()

protected:
    void Construct(
        const FArguments& iArgs,
        FOdysseyAnimationEditorExtension* iExtension,
        UOdysseyAnimationLayer* iLayer
    );

public:
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
    virtual FReply OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iEvent) override;
    virtual FReply OnMouseMove(const FGeometry& iGeometry, const FPointerEvent& iEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent) override;
    virtual FReply OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iEvent) override;
    virtual void OnDragEnter(const FGeometry& iGeometry, const FDragDropEvent& iEvent) override;
    virtual void OnDragLeave(const FDragDropEvent& iEvent) override;
    virtual FReply OnDragOver(const FGeometry& iGeometry, const FDragDropEvent& iEvent) override;
    virtual FReply OnDrop(const FGeometry& iGeometry, const FDragDropEvent& iEvent) override;

    virtual FReply OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent ) override;
    virtual FReply OnKeyUp( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent ) override;

protected:
    virtual TSharedRef<SWidget> OnGenerateCellWidget(UOdysseyAnimationCell* iCell) = 0;

protected:
    //TArray<TSharedPtr<FOdysseyAnimationCell>> GetSelectedCells() const;
    //FReply OnCellsSelectionDragged();

    EVisibility GetLightTableVisibility() const;

    FOptionalSize GetCellsHeight() const;
    FOptionalSize GetLightTableHeight() const;
    bool DisplayOptions() const;

    bool GetShowCellsHandles() const;

protected:
    //Context Menu
    virtual void BuildContextMenu(FMenuBuilder& iMenuBuilder);
    virtual TSharedPtr<FExtender> ExtendContextMenu();
    virtual void MapActions(TSharedPtr<FUICommandList> iCommandList, int iFrame);

private:
    void RemoveCellMark();
    bool CanRemoveCellMark() const;
    void SetCellMark( int iMarkId );
    bool CanSetCellMark() const;
    bool IsCellMarkChecked( int iMarkId ) const;

    TSharedRef<SWidget> CreateCellMarkMenuWidget(int iMarkId);
    void BuildCellsMarksSubMenu(FMenuBuilder& iMenuBuilder);

    FReply OnContextMenuMinusButtonClicked();
    FReply OnContextMenuPlusButtonClicked();

protected:
    FOdysseyAnimationEditorExtension* mExtension;
    UOdysseyAnimationLayer* mLayer;

    enum eDragState
    {
        kDrag_None, //The drop will do nothin
        kDrag_Move, //The drop will move the selected cells
        kDrag_Copy, //The drop will copy the selected cells
    };

    bool mIsDraggingOver;
    eDragState mDragState;
    int mDragPosition;
    TAttribute<bool> mDisplayOptions;

    TSharedPtr<FOdysseyAnimationTimelineCellsShortcuts> mAnimationTimelineCellsShortcuts;
    TSharedPtr<FOdysseyAnimationTimelineCellImageStaggerShortcuts> mAnimationTimelineCellImageStaggerShortcuts;
};
