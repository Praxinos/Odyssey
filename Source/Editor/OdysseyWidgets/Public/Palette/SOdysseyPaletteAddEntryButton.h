// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SComboButton.h"

class UOdysseyPalette;

/**
 * Implements the Layer stack widget
 */
class ODYSSEYWIDGETS_API SOdysseyPaletteAddEntryButton : public SComboButton
{
public:

    SLATE_BEGIN_ARGS(SOdysseyPaletteAddEntryButton)
        {}
        SLATE_ATTRIBUTE( UOdysseyPalette*, Palette )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyPaletteAddEntryButton();
    void Construct(const FArguments& InArgs);

private:
    //PRIVATE API
    TSharedRef<SWidget> MakeMenu();
    void AddEntryFromClass(FAssetData iAssetData);

private:
    TAttribute<UOdysseyPalette*> mPalette;
    
};
