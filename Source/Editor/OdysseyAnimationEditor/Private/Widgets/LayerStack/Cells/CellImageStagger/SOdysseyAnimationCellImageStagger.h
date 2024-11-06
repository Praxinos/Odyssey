// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"
#include "Widgets/Input/SSpinBox.h"

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationCellImageStagger
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationCellImageStagger)
        : _ShowContent(true)
        {}
        SLATE_ATTRIBUTE(bool, ShowContent)
        SLATE_ARGUMENT( TSharedPtr<FOdysseyAnimationEditorTimelinePosition>, TimelinePosition )
    SLATE_END_ARGS()

public:
    void Construct(const FArguments& iArgs, UOdysseyAnimationCellImageStagger* iCell);
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const;

private:
    const FSlateBrush* GetBehaviourBrush() const;
    EVisibility GetContentVisibility() const;

    int GetReach() const;

    int GetClampedReach() const;
    int GetStaggerLength() const;
    void OnReachValueChanged(int iReach);
    void OnReachValueCommited(int iReach, ETextCommit::Type iType);
    void OnReachBeginSliderMovement();
    void OnReachEndSliderMovement(int iReach);

    TSharedRef<SWidget> GetBehaviourMenuContent();

    void MapActions(TSharedPtr<FUICommandList> iCommandList);
    void BuildContextMenu(FMenuBuilder& iMenuBuilder);

    void SetBehaviour(EOdysseyAnimationCellImageStaggerBehaviour iBehaviour);
    bool CanSetBehaviour(EOdysseyAnimationCellImageStaggerBehaviour iBehaviour) const;
    bool IsBehaviour(EOdysseyAnimationCellImageStaggerBehaviour iBehaviour) const;

private:
    UOdysseyAnimationCellImageStagger* mCell;
    TAttribute<bool> mShowContent;
    TSharedPtr<FOdysseyAnimationEditorTimelinePosition> mTimelinePosition;
    FText mSetReachTransactionName;

    bool mIsEditingReach;
    struct FReachData
    {
        int mReach;
    } mReachData;
    TSharedPtr<SSpinBox<int>> mReachSpinBox;
};