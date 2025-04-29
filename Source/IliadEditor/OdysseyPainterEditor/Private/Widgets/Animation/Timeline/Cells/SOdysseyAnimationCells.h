// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyAnimationLayer.h"

class FOdysseyAnimationCellsMutator;
class FOdysseyAnimationTimelineTool;
class FOdysseyPainterEditorAnimationTimelinePosition;

/**
 * Implements a layer row widget
 */
class SOdysseyAnimationCells
    : public SCompoundWidget
{
    SLATE_DECLARE_WIDGET(SOdysseyAnimationCells, SCompoundWidget)

    DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SWidget>, FOnCreateCellWidget, UOdysseyAnimationCell*)

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationCells)
        : _ShowHandles(false)
        {}
        SLATE_ATTRIBUTE(TArray<UOdysseyAnimationCell*>, Cells)
        SLATE_ARGUMENT(TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>, TimelinePosition)
        SLATE_EVENT(FOnCreateCellWidget, OnCreateCellWidget)
        SLATE_ATTRIBUTE(bool, ShowHandles)
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


private:
    struct FCellItem
    {
        UOdysseyAnimationCell* mCell = nullptr;
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

    EVisibility GetCellVisibility(UOdysseyAnimationCell* iCell) const;
    float GetCellHeight() const;
    float GetCellExposure(UOdysseyAnimationCell* iCell) const;

    EVisibility GetExposureHandleVisibility(UOdysseyAnimationCell* iCell) const;
    void OnExposureHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void OnExposureHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void OnExposureHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent);

    EVisibility GetTimingHandleVisibility(UOdysseyAnimationCell* iCell) const;
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
    void SetPreBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour);
    bool CanSetPreBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour) const;
    bool IsPreBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour) const;

    const FSlateBrush* GetPostBehaviourBrush() const;
    TSharedRef<SWidget> GetPostBehaviourMenuContent();
    void SetPostBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour);
    bool CanSetPostBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour) const;
    bool IsPostBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour) const;

    float MousePositionToFrame(float iX) const;
    float FrameToMousePosition(float iFrame) const;

    void RefreshItemsSource();

private:
    TSlateAttribute<TArray<UOdysseyAnimationCell*>> mCells;
    TSharedPtr<UE::Slate::Containers::TObservableArray<TSharedPtr<FCellItem>>> mItemsSource;

    TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition> mTimelinePosition;
    class UOdysseyAnimationLayer* mAnimationLayer;
    TSharedPtr<FOdysseyAnimationTimelineTool> mTool;

    //Cells creation management (add cells handles)
    FOnCreateCellWidget mOnCreateCellWidget;

    //Handles visibility management
    TAttribute<bool> mShowHandles;
    UOdysseyAnimationCell* mHoveredCell; //cell that can display its exposure handle
    TArray<UOdysseyAnimationCell*> mTimingHandleCells; //cells that can display their timing handle

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
        UOdysseyAnimationCell* mCell = nullptr;
        double mMousePosition = 0;
        int mInitialExposure = 0;
    } mExposureHandleDragData;

    struct
    {
        bool mIsDragging = false;
        UOdysseyAnimationCell* mCell = nullptr;
        int mMinOffset = 0;
        bool mHasMaxOffset = false;
        int mMaxOffset = 0;
        double mMousePosition = 0;

        int mInitialOffset = 0;
        TMap<UOdysseyAnimationCell*, int> mAffectedCells;
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
        TMap<UOdysseyAnimationCell*, int> mAffectedCells;
    } mAddCellsHandleDragData;
};
