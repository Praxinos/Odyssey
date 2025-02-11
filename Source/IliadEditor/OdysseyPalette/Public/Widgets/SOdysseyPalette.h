// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"

class UOdysseyPalette;
class UOdysseyPaletteEntryColor;

/**
 * Implements the mesh selector
 */
class ODYSSEYPALETTE_API SOdysseyPalette : public SCompoundWidget
{
    SLATE_DECLARE_WIDGET(SOdysseyPalette, SCompoundWidget)

public:
    DECLARE_DELEGATE_OneParam(FOnCurrentColorEntryChanged, UOdysseyPaletteEntryColor*)
    DECLARE_DELEGATE_OneParam(FOnCurrentSetSelected, int)

public:
    SOdysseyPalette();

    SLATE_BEGIN_ARGS(SOdysseyPalette)
        {}
        SLATE_ATTRIBUTE(UOdysseyPalette*, Palette)
        SLATE_ATTRIBUTE(UOdysseyPaletteEntryColor*, CurrentColorEntry)
        SLATE_ATTRIBUTE(int, CurrentSet)
        SLATE_EVENT(FOnCurrentColorEntryChanged, OnCurrentColorEntryChanged)
        SLATE_EVENT(FOnCurrentSetSelected, OnCurrentSetSelected)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(const FArguments& InArgs);

public:
    UOdysseyPalette* GetPalette() const;
    EVisibility GetTreeViewVisibility() const;

private:
    UOdysseyPaletteEntryColor* GetPaletteCurrentColorEntry() const;
    void OnPaletteCurrentColorEntrySelected(UOdysseyPaletteEntryColor* iEntry);

private:
    // Private data
    TSlateAttribute<UOdysseyPalette*> mPalette;
};
