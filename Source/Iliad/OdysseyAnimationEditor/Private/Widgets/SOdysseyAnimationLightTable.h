// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SButton.h"

class FOdysseyAnimationEditorExtension;
class UOdysseyLayerStack;
class UOdysseyAnimationLayerStack;

//////////////////////////////////////////////////////////////////////////
// SOdysseyAnimationLightTable
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLightTable : public SCompoundWidget
{
    SLATE_DECLARE_WIDGET(SOdysseyAnimationLightTable, SCompoundWidget)

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLightTable)
        {}
        SLATE_ATTRIBUTE( UOdysseyAnimationLayerStack*, LayerStack )
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, FOdysseyAnimationEditorExtension* iExtension);

    SOdysseyAnimationLightTable();
    ~SOdysseyAnimationLightTable();

private:
    //SWidget overrides
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
    void RequestRebuild();
    void Rebuild();

    void OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack);

    void OnPreviousKeyIsActivatedCheckStateChanged(ECheckBoxState iState, int iKeyIndex);
    void OnNextKeyIsActivatedCheckStateChanged(ECheckBoxState iState, int iKeyIndex);
    void OnPreviousKeyOpacitySliderValueChanged(float iValue, int iKeyIndex );
    void OnNextKeyOpacitySliderValueChanged(float iValue, int iKeyIndex );

    ECheckBoxState GetPreviousKeyIsActivated( int iKeyIndex ) const;
    ECheckBoxState GetNextKeyIsActivated( int iKeyIndex ) const;
    float GetPreviousKeyOpacity( int iKeyIndex ) const;
    float GetNextKeyOpacity( int iKeyIndex ) const;

    TSharedRef<SWidget> GeneratePreviousKeyWidget(int iKeyIndex);
    TSharedRef<SWidget> GenerateNextKeyWidget(int iKeyIndex);

private:
    FOdysseyAnimationEditorExtension* mExtension;
    TSharedPtr<SHorizontalBox> mSlidersBox;
    bool mRebuildRequested;
    TSlateAttribute<UOdysseyAnimationLayerStack*> mLayerStack;
};
