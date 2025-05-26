// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyAnimationLayer.h"

class FOdysseyAnimationCellsMutator;
class FOdysseyAnimationTimelineTool;
class FOdysseyPainterEditorAnimationTimelinePosition;
class FOdysseyAnimationTimelineCellsShortcuts;
class FOdysseyAnimationTimelineCellImageStaggerShortcuts;

/**
 * Implements a layer row widget
 */
class SOdysseyAnimationCells
    : public SCompoundWidget
{
    SLATE_DECLARE_WIDGET(SOdysseyAnimationCells, SCompoundWidget)

public:
    DECLARE_DELEGATE_OneParam(FOnTransactCurrentFrame, TOptional<int>)
    DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SWidget>, FOnCreateCellWidget, UOdysseyLayerCell*)

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationCells)
        : _CurrentFrame(0)
        , _ShowHandles(false)
        {}
        SLATE_ARGUMENT(TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>, TimelinePosition)
        SLATE_ARGUMENT(TSharedPtr<FExtender>, ContextMenuExtender)
        SLATE_ATTRIBUTE(TArray<UOdysseyLayerCell*>, Cells)
        SLATE_ATTRIBUTE(int, CurrentFrame)
        SLATE_ATTRIBUTE(bool, ShowHandles)
        SLATE_EVENT(FOnTransactCurrentFrame, OnTransactCurrentFrame)
        SLATE_EVENT(FOnCreateCellWidget, OnCreateCellWidget)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyAnimationCells();
    SOdysseyAnimationCells();
    void Construct(
        const FArguments& iArgs,
        class UOdysseyAnimationLayer* iLayer
    );

public:
    //SWidget overrides
    virtual bool SupportsKeyboardFocus() const override;
    virtual FReply OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iEvent) override;
    virtual FReply OnMouseMove(const FGeometry& iGeometry, const FPointerEvent& iEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent) override;
    virtual FReply OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) override;
    //virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;

protected:
    //Context Menu
    TSharedPtr<SWidget> OnContextMenuOpening();
    void MapActions(TSharedPtr<FUICommandList> iCommandList);

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

private:
    struct FCellItem
    {
        UOdysseyLayerCell* mCell = nullptr;
    };

private:
    TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FCellItem> iCell, const TSharedRef<STableViewBase>& iOwnerTable);

    TSharedRef<SWidget> CreateCellWidget(int iCellIndex);
    TSharedRef<SWidget> CreateTimingHandleWidget();
    //TSharedRef<SWidget> CreateExposureHandleWidget(int iCellIndex);
    TSharedRef<SWidget> CreateAddCellsHandleWidget();
    TSharedRef<SWidget> CreateCellBreakIndicatorWidget();

    //void RefreshCells();
    void RefreshTempCells();
    //void AddCellSection(int iCellIndex);
    void AddTempCellSection(bool iPost);

    void SelectAllFrames();
    void DeleteSelectedFrames();

private:
    void OnCellsChanged();

private:
    //Widget Methods
    float GetOffset() const;
    void UpdateHoveredCell();

    //EVisibility GetFrameSelectorVisibility() const;

    EVisibility GetCellVisibility(UOdysseyLayerCell* iCell) const;
    float GetCellHeight() const;
    float GetCellExposure(UOdysseyLayerCell* iCell) const;

    EVisibility GetExposureHandleVisibility(UOdysseyLayerCell* iCell) const;
    void OnExposureHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void OnExposureHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void OnExposureHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent);

    EVisibility GetTimingHandleVisibility(UOdysseyLayerCell* iCell) const;
    void OnTimingHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void OnTimingHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void OnTimingHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent);

    EVisibility GetAddCellsHandleRightVisibility() const;
    EVisibility GetAddCellsHandleLeftVisibility() const;
    void OnAddCellsHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, bool iIsRightHandle);
    void OnAddCellsHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void OnAddCellsHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent);

    EVisibility GetCellBreakIndicatorVisibility() const;
    EVisibility GetCellBreakIndicatorBlankVisibility() const;
    float GetCellBreakIndicatorOffset() const;
    float GetCellBreakIndicatorWidth() const;

    const FSlateBrush* GetPreBehaviourBrush() const;
    TSharedRef<SWidget> GetPreBehaviourMenuContent();
    void SetPreBehaviour(EOdysseyLayerImagePostBehaviour iBehaviour);
    bool CanSetPreBehaviour(EOdysseyLayerImagePostBehaviour iBehaviour) const;
    bool IsPreBehaviour(EOdysseyLayerImagePostBehaviour iBehaviour) const;

    const FSlateBrush* GetPostBehaviourBrush() const;
    TSharedRef<SWidget> GetPostBehaviourMenuContent();
    void SetPostBehaviour(EOdysseyLayerImagePostBehaviour iBehaviour);
    bool CanSetPostBehaviour(EOdysseyLayerImagePostBehaviour iBehaviour) const;
    bool IsPostBehaviour(EOdysseyLayerImagePostBehaviour iBehaviour) const;

    float MousePositionToFrame(float iX) const;
    float FrameToMousePosition(float iFrame) const;

    void RefreshItemsSource();

private:
    TSlateAttribute<TArray<UOdysseyLayerCell*>> mCells;
    TSharedPtr<UE::Slate::Containers::TObservableArray<TSharedPtr<FCellItem>>> mItemsSource;

    TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition> mTimelinePosition;
    class UOdysseyAnimationLayer* mAnimationLayer;
    TSharedPtr<FOdysseyAnimationTimelineTool> mTool;

    //Cells creation management (add cells handles)
    FOnCreateCellWidget mOnCreateCellWidget;

    //Handles visibility management
    TAttribute<bool> mShowHandles;
    UOdysseyLayerCell* mHoveredCell; //cell that can display its exposure handle

    //Box containing the cells widgets
    bool mNeedsCellsRefresh = false;
    TSharedPtr<SHorizontalBox> mCellsBox;
    TSharedPtr<SHorizontalBox> mTempPreCellsBox;
    TSharedPtr<SHorizontalBox> mTempPostCellsBox;

    //Handles brushes
    const FSlateBrush* mTimingHandleBrush;
    const FSlateBrush* mExposureHandleBrush;
    const FSlateBrush* mAddCellsHandleRightBrush;
    const FSlateBrush* mAddCellsHandleLeftBrush;
    const FSlateBrush* mCellBreakIndicatorBrush;
    const FSlateBrush* mCellBreakIndicatorExtendedBrush;

private:
    //Events structures
    FVector2D mMousePosition;
    uint32 mNumTempCellsToPrepend;
    uint32 mNumTempCellsToAppend;

    struct
    {
        bool mIsDragging = false;
        UOdysseyLayerCell* mCell = nullptr;
        double mMousePosition = 0;
        int mInitialExposure = 0;
    } mExposureHandleDragData;

    struct
    {
        bool mIsDragging = false;
        UOdysseyLayerCell* mCell = nullptr;
        int mMinOffset = 0;
        bool mHasMaxOffset = false;
        int mMaxOffset = 0;
        double mMousePosition = 0;

        int mInitialOffset = 0;
        TMap<UOdysseyLayerCell*, int> mAffectedCells;
    } mTimingHandleDragData;

    struct
    {
        bool mIsDragging = false;
        int mMinOffset;
        int mMaxOffset;
        bool mHasMaxOffset;
        double mMousePosition;
        bool mIsRightHandle;

        int mInitialOffset;
        TMap<UOdysseyLayerCell*, int> mAffectedCells;
    } mAddCellsHandleDragData;

    TSharedPtr<FOdysseyAnimationTimelineCellsShortcuts> mAnimationTimelineCellsShortcuts;
    TSharedPtr<FOdysseyAnimationTimelineCellImageStaggerShortcuts> mAnimationTimelineCellImageStaggerShortcuts;
    TAttribute<int> mCurrentFrame;
    FOnTransactCurrentFrame mOnTransactCurrentFrame;
    TSharedPtr<FExtender> mContextMenuExtender;
};
