// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"

class FOdysseyAnimationCell;
/**
 * Implements a layer row widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationCells
    : public SCompoundWidget
{
    DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SWidget>, FOnCreateCellWidget, TSharedPtr<FOdysseyAnimationCell>)
    DECLARE_DELEGATE_RetVal(TSharedRef<FOdysseyAnimationCell>, FOnCreateCell)

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationCells)
        : _ShowHandles(false)
        {}
        SLATE_EVENT(FOnCreateCellWidget, OnCreateCellWidget)
        SLATE_EVENT(FOnCreateCell, OnCreateCell)
        SLATE_ATTRIBUTE(bool, ShowHandles)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyAnimationCells();
    SOdysseyAnimationCells();
    void Construct(
        const FArguments& iArgs, 
        FOdysseyAnimationEditorExtension* iExtension,
        class UOdysseyAnimationLayer* iLayer,
        TSharedPtr<FOdysseyAnimationCellsContainer> iCellsContainer
    );

public:
    //SWidget overrides
	virtual bool SupportsKeyboardFocus() const override;
    virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;
    virtual FReply OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iEvent) override;
    virtual FReply OnMouseMove(const FGeometry& iGeometry, const FPointerEvent& iEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent) override;
    virtual FReply OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) override;

private:
    void RequestRefresh();
    void RequestRebuild();

    TSharedRef<SWidget> CreateCellWidget(int iCellIndex);
    TSharedRef<SWidget> CreateTimingHandleWidget(int iCellIndex);
    TSharedRef<SWidget> CreateLengthHandleWidget(int iCellIndex);
    TSharedRef<SWidget> CreateAddCellsHandleRightWidget();
    TSharedRef<SWidget> CreateAddCellsHandleLeftWidget();

    void RefreshWidgets();
    //void InsertCellSection(int iIndex, TSharedPtr<FCellData> iCellData);
    void AddCellSection(int iCellIndex);

    void SelectAllFrames();
    void DeleteSelectedFrames();

private:
    void OnCellsChanged();

private:
    //Widget Methods
    float GetOffset() const;
    void UpdateHandlesVisibility();

    //EVisibility GetFrameSelectorVisibility() const;

    EVisibility GetCellVisibility(int iCellIndex) const;
    float GetCellHeight() const;
    float GetCellLength(int iCellIndex) const;

    EVisibility GetLengthHandleVisibility(int iCellIndex) const;
    void OnLengthHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, int iCellIndex);
    void OnLengthHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void OnLengthHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent);

    EVisibility GetTimingHandleVisibility(int iCellIndex) const;
    void OnTimingHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, int iCellIndex);
    void OnTimingHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void OnTimingHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent);

    EVisibility GetAddCellsHandleRightVisibility() const;
    EVisibility GetAddCellsHandleLeftVisibility() const;
    void OnAddCellsHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, bool iIsRightHandle);
    void OnAddCellsHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void OnAddCellsHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent);

private:
    FOdysseyAnimationEditorExtension* mExtension;
    class UOdysseyAnimationLayer* mAnimationLayer;
    TSharedPtr<FOdysseyAnimationCellsContainer> mCellsContainer;

    //Cells creation management (add cells handles)
    FOnCreateCellWidget mOnCreateCellWidget;
    FOnCreateCell mOnCreateCell;

    //Handles visibility management
    TAttribute<bool> mShowHandles;
    bool mLockHandlesVisibility;
    TSharedPtr<FOdysseyAnimationCell> mLengthHandleCell; //cell that can display its length handle
    TArray<TSharedPtr<FOdysseyAnimationCell>> mTimingHandleCells; //cells that can display their timing handle

    //Box containing the cells widgets
    TSharedPtr<SHorizontalBox> mCellsBox;

    //Handles brushes
    const FSlateBrush* mTimingHandleBrush;
    const FSlateBrush* mLengthHandleBrush;
    const FSlateBrush* mAddCellsHandleRightBrush;
    const FSlateBrush* mAddCellsHandleLeftBrush;

private:
    //Events structures
    FVector2D mMousePosition;
    bool mIsRefreshPending;
    TSharedPtr<FOdysseyAnimationCellsMutator> mCellsMutator;

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
    } mTimingHandleDragData;

    /* struct
    {
        TArray<TSharedPtr<FCellData>> mEditedCellData;
        int mFirstCellToRemove;
        int mNumCellsToRemove;
        int mMinOffset;
        int mMaxOffset;
        bool mHasMaxOffset;
        int mOffset;
        double mMousePosition;
        bool mIsRightHandle;
    } mAddCellsHandleDragData; */
};
