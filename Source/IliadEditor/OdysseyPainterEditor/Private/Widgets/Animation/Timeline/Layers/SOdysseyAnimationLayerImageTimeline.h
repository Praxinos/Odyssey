// IDDN.FR.001.060015.014.S.X.2019.000.00000
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
class SOdysseyPainterEditorVectorMassModifierView;
class FOdysseyVectorGroupPaint;
class SOdysseyEvents;

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
    virtual bool IsRowEnabled(FName iRow) const;

    TSharedRef<SWidget> GenerateMainRowTimelineWidget();
    TSharedRef<SWidget> GenerateLighttableRowTimelineWidget();
    TSharedRef<SWidget> GenerateOutOfPegsRowTimelineWidget();
    TSharedRef<SWidget> GenerateCellNamesRowTimelineWidget();

public:
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

    virtual FReply OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent ) override;
    virtual FReply OnKeyUp( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent ) override;

private:
    FReply OnSubRowMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, FName iRow);
    FReply OnSubRowMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, FName iRow);
    FReply OnSubRowMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, FName iRow);

    FReply OnSubRowDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, FName iRow);
    void OnSubRowDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent, FName iRow);
    void OnSubRowDragLeave(const FDragDropEvent& DragDropEvent, FName iRow);
    FReply OnSubRowDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent, FName iRow);
    FReply OnSubRowDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent, FName iRow);

private:
    FReply OnMainSubRowMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
    FReply OnMainSubRowMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
    FReply OnMainSubRowMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
    FReply OnMainSubRowDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
    void OnMainSubRowDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent);
    void OnMainSubRowDragLeave(const FDragDropEvent& DragDropEvent);
    FReply OnMainSubRowDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent);
    FReply OnMainSubRowDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent);

public:
    UOdysseyAnimationLayer* GetLayer() const;

protected:
    virtual TSharedRef<SWidget> OnGenerateCellWidget(UOdysseyLayerCell* iCell) = 0;
    virtual void BuildContextMenu(TSharedRef<FUICommandList> CommandList, FMenuBuilder& MenuBuilder);
    FReply MassModifierAcceptProperties( TSharedRef<SOdysseyPainterEditorVectorMassModifierView> iObjectView);
    void MassModifierWindowClosed( const TSharedRef<SWindow>& iWindow
                                 , TSharedRef<SOdysseyPainterEditorVectorMassModifierView> objectView );
    void MassModifier();

protected:
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

    EVisibility GetRowDisabledColorVisibility(FName iRow) const;
    const FSlateBrush* GetRowDisabledColorValue(FName iRow) const;

protected:
    UOdysseyAnimationLayer* mLayer;
    TAttribute<int> mCurrentFrame;
    FOnTransactCurrentFrame mOnTransactCurrentFrame;
    TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition> mTimelinePosition;
    SOdysseyAnimationTimelineOutOfPegsKey::FOnActivateOutOfPegs mOnActivateOutOfPegs;
    FSimpleDelegate mOnInactivateOutOfPegs;
    SOdysseyAnimationTimelineOutOfPegsKey::FOnIsOutOfPegsChecked mOnIsOutOfPegsChecked;

    TMap<FName, TSharedPtr<SOdysseyEvents>> mEventWidgets;

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
