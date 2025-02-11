// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class UOdysseyPalette;

/**
 * Implements the Palette widget
 */
class ODYSSEYPALETTE_API SOdysseyPaletteSetComboBox
    : public SComboButton
{
    SLATE_DECLARE_WIDGET(SOdysseyPaletteSetComboBox, SComboButton)

public:
    DECLARE_DELEGATE_OneParam(FOnCurrentSetSelected, int)

public:
    SLATE_BEGIN_ARGS(SOdysseyPaletteSetComboBox)
        : _IsReadOnly(false)
        {}
        SLATE_ARGUMENT(bool, IsReadOnly)
        SLATE_ATTRIBUTE(UOdysseyPalette*, Palette)
        SLATE_ATTRIBUTE(int, CurrentSet)
        SLATE_EVENT(FOnCurrentSetSelected, OnCurrentSetSelected)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyPaletteSetComboBox();
    SOdysseyPaletteSetComboBox();

    void Construct(const FArguments& InArgs);

public:
    UOdysseyPalette* GetPalette() const;
    static void BuildMenu(FMenuBuilder& iMenuBuilder, UOdysseyPalette* iPalette, int iCurrentSet, bool iIsReadOnly, FOnCurrentSetSelected iOnCurrentSetSelected);

private:
    void OnPaletteChanged();
    void OnCurrentSetChanged();

    TSharedRef<SWidget> GetMenuContent();
    FText GetCurrentSetName() const;

protected:
    TSlateAttribute<UOdysseyPalette*> mPaletteAttribute;
    UOdysseyPalette* mPalette;
    TSlateAttribute<int> mCurrentSetAttribute;
    int mCurrentSet;
    FOnCurrentSetSelected mOnCurrentSetSelected;
    bool mIsReadOnly;
};
