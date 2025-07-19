// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

class UOdysseyPalette;

/**
 * Implements the Palette widget
 */
class ODYSSEYPALETTEEDITOR_API SOdysseyPaletteSetComboBox
    : public SComboButton
{
    SLATE_DECLARE_WIDGET(SOdysseyPaletteSetComboBox, SComboButton)

public:
    DECLARE_DELEGATE_OneParam(FOnCurrentSetSelected, FGuid)

public:
    SLATE_BEGIN_ARGS(SOdysseyPaletteSetComboBox)
        : _IsReadOnly(true)
        {}
        SLATE_ARGUMENT(bool, IsReadOnly)
        SLATE_ATTRIBUTE(UOdysseyPalette*, Palette)
        SLATE_ATTRIBUTE(FGuid, CurrentSet)
        SLATE_EVENT(FOnCurrentSetSelected, OnCurrentSetSelected)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyPaletteSetComboBox();
    SOdysseyPaletteSetComboBox();

    void Construct(const FArguments& InArgs);

public:
    UOdysseyPalette* GetPalette() const;
    static void BuildMenu(FMenuBuilder& iMenuBuilder, UOdysseyPalette* iPalette, FGuid iCurrentSet, bool iIsReadOnly, FOnCurrentSetSelected iOnCurrentSetSelected);

private:
    void OnPaletteChanged();
    void OnCurrentSetChanged();

    TSharedRef<SWidget> GetMenuContent();
    FText GetCurrentSetName() const;

protected:
    TSlateAttribute<UOdysseyPalette*> mPaletteAttribute;
    UOdysseyPalette* mPalette;
    TSlateAttribute<FGuid> mCurrentSetAttribute;
    FGuid mCurrentSet;
    FOnCurrentSetSelected mOnCurrentSetSelected;
    bool mIsReadOnly;
};
