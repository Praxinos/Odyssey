// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SButton.h"

class FOdysseyAnimationEditorExtension;

//////////////////////////////////////////////////////////////////////////
// SOdysseyAnimationLightTable
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLightTable : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SOdysseyAnimationLightTable)
		{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, FOdysseyAnimationEditorExtension* iExtension);

	SOdysseyAnimationLightTable();
	~SOdysseyAnimationLightTable();

private:
	//SWidget overrides
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
	TSharedPtr<FOdysseyAnimationLightTable> GetLightTable() const;

	void RequestRebuild();
	void Rebuild();
	
	void OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack);
	void OnKeyIsActivatedCheckStateChanged(ECheckBoxState iState, int iKeyIndex);
	void OnKeyOpacitySliderValueChanged(float iValue, int iKeyIndex );

	ECheckBoxState GetKeyIsActivated( int iKeyIndex ) const;
	float GetKeyOpacity( int iKeyIndex ) const;

private:
	FOdysseyAnimationEditorExtension* mExtension;
	TSharedPtr<SHorizontalBox> mSlidersBox;
	bool mRebuildRequested;
};
