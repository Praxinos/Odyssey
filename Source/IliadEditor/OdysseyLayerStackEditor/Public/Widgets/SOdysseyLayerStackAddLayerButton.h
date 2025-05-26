// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

class UOdysseyLayerStack;
class UOdysseyLayer;

/**
 * Implements the Layer stack widget
 */
class ODYSSEYLAYERSTACKEDITOR_API SOdysseyLayerStackAddLayerButton
    : public SCompoundWidget
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
