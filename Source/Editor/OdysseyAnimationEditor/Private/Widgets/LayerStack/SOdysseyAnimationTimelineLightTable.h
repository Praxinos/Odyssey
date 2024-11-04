// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyImageRenderingAbility.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineLightTableKey.h"

class UOdysseyAnimationLayer;
class UOdysseyAnimation;
class UOdysseyAnimationCell;

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineLightTable
    : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineLightTable)
	{}
		SLATE_ARGUMENT(TSharedPtr<FOdysseyAnimationEditorTimelinePosition>, TimelinePosition)
	SLATE_END_ARGS()

	void Construct(
		const FArguments& InArgs,
		UOdysseyAnimationLayer* iLayer
	);

private:
    void OnCurrentFrameChanged(UOdysseyAnimation* iAnimation);
    void OnImageRenderingChanged(const FOdysseyImageRenderingChangedEvent& iEvent);
    void Update();

private:
	UOdysseyAnimationLayer* mLayer = nullptr;
	UOdysseyAnimationCell* mCurrentCell = nullptr;
};
