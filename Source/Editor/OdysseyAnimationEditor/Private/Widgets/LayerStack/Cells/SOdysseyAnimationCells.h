// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

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
        {}
        SLATE_EVENT(FOnCreateCellWidget, OnCreateCellWidget)
        SLATE_EVENT(FOnCreateCell, OnCreateCell)
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

private:
    struct FCellData;

public:
    //SWidget overrides
	virtual bool SupportsKeyboardFocus() const override;
    virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;

private:
    void RequestRefresh();
    void RequestRebuild();

    TSharedRef<SWidget> CreateCellWidget(TSharedPtr<FCellData> iCellData);
    TSharedRef<SWidget> CreateTimingHandleWidget(TSharedPtr<FCellData> iCellData);
    TSharedRef<SWidget> CreateLengthHandleWidget(TSharedPtr<FCellData> iCellData);
    TSharedRef<SWidget> CreateAddCellsHandleRightWidget();
    TSharedRef<SWidget> CreateAddCellsHandleLeftWidget();

    void BuildCellsData();
    TSharedPtr<FCellData> InsertCellData(int iIndex, TSharedPtr<FOdysseyAnimationCell> iCell, int iCellIndex);
    TSharedPtr<FCellData> AddCellData(TSharedPtr<FOdysseyAnimationCell> iCell, int iCellIndex);
    void RemoveCellData(int iIndex);

    void RefreshWidgets();
    void InsertCellSection(int iIndex, TSharedPtr<FCellData> iCellData);
    void AddCellSection(TSharedPtr<FCellData> iCellData);
    void RemoveCellSection(TSharedPtr<FCellData> iCellData);

    void SelectAllFrames();
    void DeleteSelectedFrames();

private:
    void OnCellsChanged();

private:
    //Widget Methods
    float GetOffset() const;

    //EVisibility GetFrameSelectorVisibility() const;

    EVisibility GetCellVisibility(TSharedPtr<FCellData> iCellData) const;
    float GetCellHeight() const;
    float GetCellLength(TSharedPtr<FCellData> iCellData) const;

    EVisibility GetLengthHandleVisibility(TSharedPtr<FCellData> iCellData) const;
    void OnLengthHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, TSharedPtr<FCellData> iCellData);
    void OnLengthHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void OnLengthHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent);

    EVisibility GetTimingHandleVisibility(TSharedPtr<FCellData> iCellData) const;
    void OnTimingHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, TSharedPtr<FCellData> iCellData);
    void OnTimingHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void OnTimingHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent);

    EVisibility GetAddCellsHandleRightVisibility() const;
    EVisibility GetAddCellsHandleLeftVisibility() const;
    void OnAddCellsHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, bool iIsRightHandle);
    void OnAddCellsHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    void OnAddCellsHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent);

    FReply OnCellsMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iEvent);
    FReply OnCellsMouseMove(const FGeometry& iGeometry, const FPointerEvent& MouseEvent);
    FReply OnCellsMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& MouseEvent);

private:
    FOdysseyAnimationEditorExtension* mExtension;
    class UOdysseyAnimationLayer* mAnimationLayer;
    TSharedPtr<FOdysseyAnimationCellsContainer> mCellsContainer;

    FOnCreateCellWidget mOnCreateCellWidget;
    FOnCreateCell mOnCreateCell;

    TSharedPtr<SBorder> mCellsBorder;
    TSharedPtr<SHorizontalBox> mCellsBox;
    TSharedPtr<SHorizontalBox> mHandlesBox;

    const FSlateBrush* mTimingHandleBrush;
    const FSlateBrush* mLengthHandleBrush;
    const FSlateBrush* mAddCellsHandleRightBrush;
    const FSlateBrush* mAddCellsHandleLeftBrush;
	
	TSharedRef<FUICommandList> mCommandList;

private:
    //Events structures
    bool mIsRefreshPending;
    bool mIsRebuildPending;
    bool mOffsettingLayer;
    int  mOffset;
    bool mEditingOffset;

    struct FCellData
    {
        TSharedPtr<FOdysseyAnimationCell> mCell;
        TSharedPtr<SWidget> mCellSectionWidget;
        TSharedPtr<SWidget> mHandlesSectionWidget;
        int mCellIndex;
        bool mIsVisible;
        bool mIsTimingHandleVisible;
        bool mIsLengthHandleVisible;
        bool mEditingLength;
        int  mLength;
    };
    TArray<TSharedPtr<FCellData>> mCellsData;

    struct
    {
        double mMousePosition;
    } mLayerOffsetData;

    struct
    {
        TSharedPtr<FCellData> mCellData;
        double mMousePosition;
    } mLengthHandleDragData;

    struct
    {
        TSharedPtr<FCellData> mCellData;
        TArray<TSharedPtr<FCellData>> mEditedCellData;
        int mFirstCellToRemove;
        int mNumCellsToRemove;

        int mMinOffset;
        bool mHasMaxOffset;
        int mMaxOffset;
        double mMousePosition;
    } mTimingHandleDragData;

    struct
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
    } mAddCellsHandleDragData;
};
