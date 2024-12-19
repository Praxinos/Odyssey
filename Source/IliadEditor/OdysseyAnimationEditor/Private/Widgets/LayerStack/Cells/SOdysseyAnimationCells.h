// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"

class FOdysseyAnimationCellsMutator;
class FOdysseyAnimationTimelineTool;
class FOdysseyAnimationEditorTimelinePosition;
/**
 * Implements a layer row widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationCells
    : public SCompoundWidget
{
    DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SWidget>, FOnCreateCellWidget, UOdysseyAnimationCell*)

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationCells)
        : _ShowHandles(false)
        {}
        SLATE_ARGUMENT(TSharedPtr<FOdysseyAnimationEditorTimelinePosition>, TimelinePosition)
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
    virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;

private:
    const TArray<UOdysseyAnimationCell*>& GetCells() const;

    TSharedRef<SWidget> CreateCellWidget(int iCellIndex);
    TSharedRef<SWidget> CreateTimingHandleWidget(int iCellIndex);
    TSharedRef<SWidget> CreateExposureHandleWidget(int iCellIndex);
    TSharedRef<SWidget> CreateAddCellsHandleRightWidget();
    TSharedRef<SWidget> CreateAddCellsHandleLeftWidget();
    TSharedRef<SWidget> CreateCellBreakIndicatorWidget(int iCellIndex);

    void RefreshCells();
    void RefreshTempCells();
    void AddCellSection(int iCellIndex);
    void AddTempCellSection(bool iPost);

    void SelectAllFrames();
    void DeleteSelectedFrames();

private:
    void OnCellsChanged();

private:
    //Widget Methods
    float GetOffset() const;
    void UpdateHandlesVisibility();

    //EVisibility GetFrameSelectorVisibility() const;

    EVisibility GetCellVisibility(UOdysseyAnimationCell* iCell) const;
    float GetCellHeight() const;
    float GetCellExposure(UOdysseyAnimationCell* iCell) const;

    EVisibility GetExposureHandleVisibility(UOdysseyAnimationCell* iCell) const;
    void OnExposureHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, int iCellIndex);
    void OnExposureHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void OnExposureHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent);

    EVisibility GetTimingHandleVisibility(UOdysseyAnimationCell* iCell) const;
    void OnTimingHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, int iCellIndex);
    void OnTimingHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void OnTimingHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent);

    EVisibility GetAddCellsHandleRightVisibility() const;
    EVisibility GetAddCellsHandleLeftVisibility() const;
    void OnAddCellsHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, bool iIsRightHandle);
    void OnAddCellsHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void OnAddCellsHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent);

    EVisibility GetCellBreakIndicatorVisibility(UOdysseyAnimationCell* iCell) const;
    EVisibility GetCellBreakIndicatorCopyVisibility(UOdysseyAnimationCell* iCell) const;
    EVisibility GetCellBreakIndicatorBlankVisibility(UOdysseyAnimationCell* iCell) const;
    float GetCellBreakIndicatorOffset(UOdysseyAnimationCell* iCell) const;
    float GetCellBreakIndicatorWidth(UOdysseyAnimationCell* iCell) const;

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

private:
    TSharedPtr<FOdysseyAnimationEditorTimelinePosition> mTimelinePosition;
    class UOdysseyAnimationLayer* mAnimationLayer;
    TSharedPtr<FOdysseyAnimationTimelineTool> mTool;

    //Cells creation management (add cells handles)
    FOnCreateCellWidget mOnCreateCellWidget;

    //Handles visibility management
    TAttribute<bool> mShowHandles;
    bool mLockHandlesVisibility;
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
        int mCellIndex;
        double mMousePosition;
        int mInitialExposure;
    } mExposureHandleDragData;

    struct
    {
        int mCellIndex;
        int mMinOffset;
        bool mHasMaxOffset;
        int mMaxOffset;
        double mMousePosition;

        int mInitialOffset;
        TMap<UOdysseyAnimationCell*, int> mAffectedCells;
    } mTimingHandleDragData;

    struct
    {
        int mMinOffset;
        int mMaxOffset;
        bool mHasMaxOffset;
        double mMousePosition;
        bool mIsRightHandle;

        int mInitialOffset;
        TMap<UOdysseyAnimationCell*, int> mAffectedCells;
    } mAddCellsHandleDragData;
};
