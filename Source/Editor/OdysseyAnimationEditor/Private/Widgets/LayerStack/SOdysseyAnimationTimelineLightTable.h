// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyImageRenderingAbility.h"

class UOdysseyAnimationLayer;
class FOdysseyAnimationEditorExtension;
class UOdysseyAnimation;

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineLightTable
	: public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineLightTable)
	{}
	SLATE_END_ARGS()

	SOdysseyAnimationTimelineLightTable();

	void Construct(
		const FArguments& InArgs,
		UOdysseyAnimationLayer* iLayer,
		FOdysseyAnimationEditorExtension* iExtension);

private:
	TSharedRef<SWidget> CreateKeyWidget(int iCellOffset);
	void OnCurrentFrameChanged(UOdysseyAnimation* iAnimation);
	void OnImageRenderingChanged(const FOdysseyImageRenderingChangedEvent& iEvent);
	void Update();
	float GetLightTableKeyRemainingLength(int iCellOffset) const;
	EVisibility GetLightTableKeyVisibility(int iCellOffset) const;
	bool GetLightTableKeyIsActivated(int iCellOffset) const;
	float GetCurrentCellLength() const;
	const FSlateBrush* GetOutOfPegsButtonImage(int iCellOffset) const;
	void OnOutOfPegsCheckStateChanged(ECheckBoxState InValue, int iCellOffset);
    ECheckBoxState IsOutOfPegsChecked(int iCellOffset) const;

private:
	UOdysseyAnimationLayer* mLayer;
	FOdysseyAnimationEditorExtension* mExtension;
	int mCurrentCellIndex;
};
