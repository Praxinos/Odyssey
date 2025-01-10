// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineOutOfPegsKey.h"
#include "Widgets/LayerStack/Layers/SOdysseyAnimationLayerTimeline.h"

class UOdysseyAnimationLayer;
class FOdysseyAnimationTimelineCellsShortcuts;
class FOdysseyAnimationTimelineCellImageStaggerShortcuts;
class SOdysseyLayerStackTreeView;
class FOdysseyAnimationEditorTimelinePosition;
class FOdysseyAnimationTimelineTool;
class FOdysseyPainterEditor;

/**
 * Implements a layer row widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerImageTimeline
    : public SOdysseyAnimationLayerTimeline
{
public:
    // Construction / Destruction
    virtual ~SOdysseyAnimationLayerImageTimeline();
    SOdysseyAnimationLayerImageTimeline();

    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerImageTimeline)
        : _PainterEditor(nullptr)
        {}
        SLATE_ARGUMENT( TSharedPtr<FOdysseyAnimationEditorTimelinePosition>, TimelinePosition )
        SLATE_EVENT(SOdysseyAnimationTimelineOutOfPegsKey::FOnActivateOutOfPegs, OnActivateOutOfPegs)
        SLATE_EVENT(FSimpleDelegate, OnInactivateOutOfPegs)
        SLATE_EVENT(SOdysseyAnimationTimelineOutOfPegsKey::FOnIsOutOfPegsChecked, OnIsOutOfPegsChecked)
        SLATE_ATTRIBUTE(FOdysseyPainterEditor*, PainterEditor)
    SLATE_END_ARGS()

public:
    void Construct(
        const FArguments& iArgs,
        const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
        UOdysseyAnimationLayer* iLayer
    );

protected:
    virtual TSharedRef<SWidget> GenerateWidgetForColumn( const FName& InColumnName ) override;
    virtual TSharedRef<SWidget> GenerateWidget( const FName& iRow, const FName& iColumn ) override;
    virtual FOptionalSize GetRowHeight(FName iRow) const override;
    virtual EVisibility GetRowVisibility(FName iRow) const override;
    virtual FMargin GetRowPadding(FName iRow) const override;

    TSharedRef<SWidget> GenerateMainRowTimelineWidget();
    TSharedRef<SWidget> GenerateLightTableRowTimelineWidget();
    TSharedRef<SWidget> GenerateOutOfPegsRowTimelineWidget();

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

public:
    UOdysseyAnimationLayer* GetLayer() const;

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

    float MousePositionToFrame(float iX) const;
    float FrameToMousePosition(float iFrame) const;

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
    UOdysseyAnimationLayer* mLayer;
    TSharedPtr<FOdysseyAnimationEditorTimelinePosition> mTimelinePosition;
    SOdysseyAnimationTimelineOutOfPegsKey::FOnActivateOutOfPegs mOnActivateOutOfPegs;
    FSimpleDelegate mOnInactivateOutOfPegs;
    SOdysseyAnimationTimelineOutOfPegsKey::FOnIsOutOfPegsChecked mOnIsOutOfPegsChecked;

    TSharedPtr<FOdysseyAnimationTimelineTool> mTool;

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
    TAttribute<FOdysseyPainterEditor*> mEditor;
};
