// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SButton.h"

class UOdysseyLayerStack;
class UOdysseyAnimationLayerStack;

//////////////////////////////////////////////////////////////////////////
// SOdysseyLighttable
class SOdysseyLighttable : public SCompoundWidget
{
    SLATE_DECLARE_WIDGET(SOdysseyLighttable, SCompoundWidget)

public:
    SLATE_BEGIN_ARGS(SOdysseyLighttable)
        {}
        SLATE_ATTRIBUTE( UOdysseyAnimationLayerStack*, LayerStack )
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    SOdysseyLighttable();
    ~SOdysseyLighttable();

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
    TSharedPtr<SHorizontalBox> mSlidersBox;
    bool mRebuildRequested;
    TSlateAttribute<UOdysseyAnimationLayerStack*> mLayerStack;
};
