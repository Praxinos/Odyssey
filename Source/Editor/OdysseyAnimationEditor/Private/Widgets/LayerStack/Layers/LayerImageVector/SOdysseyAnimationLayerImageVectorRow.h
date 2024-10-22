// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LayerStack/Layers/SOdysseyAnimationLayerRow.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineLightTableKey.h"

/**
 * Implements a layer row widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerImageVectorRow
    : public SOdysseyAnimationLayerRow
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerImageVectorRow)
        {}
		SLATE_ATTRIBUTE(int, CurrentFrame)
		SLATE_ARGUMENT( TSharedPtr<FOdysseyAnimationEditorTimelinePosition>, TimelinePosition )
		SLATE_ARGUMENT( TSharedPtr<FOdysseyAnimationEditorTimelineCellSelection>, TimelineCellSelection )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(
        const FArguments& iArgs,
        const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
        class UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector
    );

protected:
	virtual TArray<TSharedPtr<SWidget>> GenerateMainRowHeaderOptionWidgets() override;

private:
    void OnIsColoredCheckStateChanged( ECheckBoxState iState );
    void OnIsWireframeCheckStateChanged( ECheckBoxState iState );
    ECheckBoxState GetIsColoredIsChecked() const;
    ECheckBoxState GetIsWireframeIsChecked() const;

private:
    class UOdysseyAnimationLayerImageVector* mAnimationLayerImageVector;
};
