// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"

class FOdysseyAnimationEditorExtension;
class FOdysseyAnimationCellsMutator;
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
        SLATE_EVENT(FOnCreateCellWidget, OnCreateCellWidget)
        SLATE_ATTRIBUTE(bool, ShowHandles)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyAnimationCells();
    SOdysseyAnimationCells();
    void Construct(
        const FArguments& iArgs, 
        FOdysseyAnimationEditorExtension* iExtension,
        class UOdysseyAnimationLayer* iLayer
    );

public:
    //SWidget overrides
	virtual bool SupportsKeyboardFocus() const override;
    virtual FReply OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iEvent) override;
    virtual FReply OnMouseMove(const FGeometry& iGeometry, const FPointerEvent& iEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent) override;
    virtual FReply OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) override;

private:
    const TArray<UOdysseyAnimationCell*>& GetCells() const;

    TSharedRef<SWidget> CreateCellWidget(int iCellIndex);
    TSharedRef<SWidget> CreateTimingHandleWidget(int iCellIndex);
    TSharedRef<SWidget> CreateLengthHandleWidget(int iCellIndex);
    TSharedRef<SWidget> CreateAddCellsHandleRightWidget();
    TSharedRef<SWidget> CreateAddCellsHandleLeftWidget();
    TSharedRef<SWidget> CreateCellBreakIndicatorWidget(int iCellIndex);

    void RefreshCells();
    void AddCellSection(int iCellIndex);
    void AddTempCellSection();

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
    float GetCellLength(UOdysseyAnimationCell* iCell) const;

    EVisibility GetLengthHandleVisibility(UOdysseyAnimationCell* iCell) const;
    void OnLengthHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, int iCellIndex);
    void OnLengthHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void OnLengthHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent);

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
    float GetCellBreakIndicatorOffset(UOdysseyAnimationCell* iCell) const;

private:
    FOdysseyAnimationEditorExtension* mExtension;
    class UOdysseyAnimationLayer* mAnimationLayer;

    //Cells creation management (add cells handles)
    FOnCreateCellWidget mOnCreateCellWidget;

    //Handles visibility management
    TAttribute<bool> mShowHandles;
    bool mLockHandlesVisibility;
    UOdysseyAnimationCell* mHoveredCell; //cell that can display its length handle
    TArray<UOdysseyAnimationCell*> mTimingHandleCells; //cells that can display their timing handle

    //Box containing the cells widgets
    TSharedPtr<SHorizontalBox> mCellsBox;

    //Handles brushes
    const FSlateBrush* mTimingHandleBrush;
    const FSlateBrush* mLengthHandleBrush;
    const FSlateBrush* mAddCellsHandleRightBrush;
    const FSlateBrush* mAddCellsHandleLeftBrush;
    const FSlateBrush* mCellBreakIndicatorBrush;

private:
    //Events structures
    FVector2D mMousePosition;
    uint32 mNumTempCellsToPrepend;
    uint32 mNumTempCellsToAppend;

    struct
    {
        int mCellIndex;
        double mMousePosition;
        int mInitialLength;
    } mLengthHandleDragData;

    struct
    {
        int mCellIndex;
        int mMinOffset;
        bool mHasMaxOffset;
        int mMaxOffset;
        double mMousePosition;

		int mInitialOffset;
		TArray<UOdysseyAnimationCell*> mAffectedCells;
		TArray<int> mAffectedCellsInitialLength;
    } mTimingHandleDragData;

    struct
    {
        int mMinOffset;
        int mMaxOffset;
        bool mHasMaxOffset;
        double mMousePosition;
        bool mIsRightHandle;

		int mInitialOffset;
		TArray<UOdysseyAnimationCell*> mAffectedCells;
		TArray<int> mAffectedCellsInitialLength;
    } mAddCellsHandleDragData;
};
