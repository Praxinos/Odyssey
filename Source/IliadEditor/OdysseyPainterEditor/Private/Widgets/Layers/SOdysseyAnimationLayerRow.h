// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SOdysseyLayerRow.h"

class SOdysseyAnimationLayerStack;
class FOdysseyPainterEditorAnimationTimelinePosition;

/**
 * Implements a layer row widget
 */
class SOdysseyAnimationLayerRow
    : public SOdysseyLayerRow
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerRow)
        {}
        SLATE_ARGUMENT( TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>, TimelinePosition )
    SLATE_END_ARGS()

public:
    void Construct(
        const FArguments& iArgs,
        const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
        class UOdysseyAnimationLayer* iLayer
    );

protected:
    virtual TSharedRef<SWidget> GenerateWidget( const FName& iRow, const FName& iColumn ) override;
    virtual TArray<TSharedPtr<SWidget>> GenerateMainRowHeaderOptionWidgets() override;

private:
    TSharedRef<SWidget> GenerateLighttableRowHeaderWidget();
    TSharedRef<SWidget> GenerateOutOfPegsRowHeaderWidget();
    void OnLighttableCheckStateChanged(ECheckBoxState iState);
    ECheckBoxState GetLighttableIsChecked() const;

    TSharedRef<SWidget> GenerateCellNamesRowHeaderWidget();
    void OnCellNamesCheckStateChanged( ECheckBoxState iState );
    ECheckBoxState GetCellNamesIsChecked() const;

protected:
    UOdysseyAnimationLayer* mLayer;
    TAttribute<int> mCurrentFrame;
    TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition> mTimelinePosition;
};
