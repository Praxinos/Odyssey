// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyAnimationLayer.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineOutOfPegsKey.h"
#include "Widgets/Animation/Timeline/Layers/SOdysseyAnimationLayerTimeline.h"

class UOdysseyAnimationLayer;
class FOdysseyAnimationTimelineCellsShortcuts;
class FOdysseyAnimationTimelineCellImageStaggerShortcuts;
class SOdysseyLayerStackTreeView;
class FOdysseyPainterEditorAnimationTimelinePosition;
class FOdysseyAnimationTimelineTool;
class FOdysseyPainterEditor;

/**
 * Implements a layer row widget
 */
class SOdysseyAnimationLayerImageTimeline
    : public SOdysseyAnimationLayerTimeline
{
public:
    DECLARE_DELEGATE_OneParam(FOnTransactCurrentFrame, TOptional<int>)

public:
    // Construction / Destruction
    virtual ~SOdysseyAnimationLayerImageTimeline();
    SOdysseyAnimationLayerImageTimeline();

    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerImageTimeline)
        : _CurrentFrame(0)
        {}
        SLATE_ARGUMENT( TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>, TimelinePosition )
        SLATE_ATTRIBUTE(int, CurrentFrame)
        SLATE_EVENT(FOnTransactCurrentFrame, OnTransactCurrentFrame)
        SLATE_EVENT(SOdysseyAnimationTimelineOutOfPegsKey::FOnActivateOutOfPegs, OnActivateOutOfPegs)
        SLATE_EVENT(FSimpleDelegate, OnInactivateOutOfPegs)
        SLATE_EVENT(SOdysseyAnimationTimelineOutOfPegsKey::FOnIsOutOfPegsChecked, OnIsOutOfPegsChecked)
    SLATE_END_ARGS()

public:
    void Construct(
        const FArguments& iArgs,
        const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
        UOdysseyAnimationLayer* iLayer
    );

protected:
    virtual TSharedRef<SWidget> GenerateWidgetForRow( const FName& iRow, const FName& iColumn ) override;
    virtual TSharedRef<SWidget> GenerateWidget( const FName& iRow, const FName& iColumn ) override;
    virtual float GetRowHeight(FName iRow) const override;
    virtual EVisibility GetRowVisibility(FName iRow) const override;
    virtual FMargin GetRowPadding(FName iRow) const override;

    TSharedRef<SWidget> GenerateMainRowTimelineWidget();
    TSharedRef<SWidget> GenerateLighttableRowTimelineWidget();
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
    virtual TSharedRef<SWidget> OnGenerateCellWidget(UOdysseyLayerCell* iCell) = 0;
    virtual void BuildContextMenu(TSharedRef<FUICommandList> CommandList, FMenuBuilder& MenuBuilder);

protected:
    //TArray<TSharedPtr<FOdysseyAnimationCell>> GetSelectedCells() const;
    //FReply OnCellsSelectionDragged();

    EVisibility GetLighttableVisibility() const;

    FOptionalSize GetCellsHeight() const;
    FOptionalSize GetLighttableHeight() const;
    bool DisplayOptions() const;

    bool GetShowCellsHandles() const;

    float MousePositionToFrame(float iX) const;
    float FrameToMousePosition(float iFrame) const;

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
    TAttribute<int> mCurrentFrame;
    FOnTransactCurrentFrame mOnTransactCurrentFrame;
    TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition> mTimelinePosition;
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
};
