// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "SOdysseyAnimationLayerRow.h"

class SOdysseyAnimationTimelineInbetweeningHeader;

class FOdysseyPainterEditorAnimationTimelinePosition;
class FOdysseyPainterEditor;
/**
 * Implements a layer row widget
 */
class SOdysseyAnimationLayerImageVectorRow
    : public SOdysseyAnimationLayerRow
{
public:
    DECLARE_DELEGATE_OneParam(FOnTransactCurrentFrame, TOptional<int>)

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerImageVectorRow)
        {}
        SLATE_ATTRIBUTE(int, CurrentFrame)
        SLATE_ARGUMENT( TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>, TimelinePosition )
        SLATE_EVENT(FOnTransactCurrentFrame, OnTransactCurrentFrame)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(
        const FArguments& iArgs,
        const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
        class UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector
    );

public:
    TSharedPtr<SOdysseyAnimationTimelineInbetweeningHeader> GetInbetweeningHeader();

protected:
    virtual TSharedRef<SWidget> GenerateWidget( const FName& iRow, const FName& iColumn ) override;
    virtual EVisibility GetRowVisibility(FName iRow) const;
    virtual TArray<TSharedPtr<SWidget>> GenerateMainRowHeaderOptionWidgets() override;
    TSharedRef<SWidget> GenerateInbetweeningRowHeaderWidget();

private:
    void OnIsColoredCheckStateChanged( ECheckBoxState iState );
    void OnIsWireframeCheckStateChanged( ECheckBoxState iState );
    ECheckBoxState GetIsColoredIsChecked() const;
    ECheckBoxState GetIsWireframeIsChecked() const;

private:
    TSharedPtr<SOdysseyAnimationTimelineInbetweeningHeader> mInbetweeningHeader;
    class UOdysseyAnimationLayerImageVector* mAnimationLayerImageVector;
};
