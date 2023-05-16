// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SComboButton.h"

class UOdysseyLayerStack;

/**
 * Implements the Layer stack widget
 */
class ODYSSEYLAYERSTACKEDITOR_API SOdysseyLayerStackAddLayerButton : public SComboButton
{
public:
    DECLARE_DELEGATE_OneParam(FOnAdded, UOdysseyLayer*);

public:

    SLATE_BEGIN_ARGS(SOdysseyLayerStackAddLayerButton)
        {}
        SLATE_ATTRIBUTE( UOdysseyLayerStack*, LayerStack )
        SLATE_EVENT( FOnAdded, OnAdded )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyLayerStackAddLayerButton();
    void Construct(const FArguments& InArgs);

private:
    //PRIVATE API
    TSharedRef<SWidget> MakeMenu();
    void AddLayerFromClass(FAssetData iAssetData);

private:
    TAttribute<UOdysseyLayerStack*> mLayerStack;
    FOnAdded mOnAdded;
};
