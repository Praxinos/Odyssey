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
    DECLARE_DELEGATE_OneParam(FOnPaletteChanged, UOdysseyPalette*)
    DECLARE_DELEGATE_OneParam(FOnCurrentColorEntrySelected, UOdysseyPaletteEntryColor*)
    DECLARE_DELEGATE_OneParam(FOnCurrentSetSelected, int)

public:
    SOdysseyPalette();

    SLATE_BEGIN_ARGS(SOdysseyPalette)
        {}
        SLATE_ATTRIBUTE(UOdysseyPalette*, Palette)
        SLATE_ATTRIBUTE(UOdysseyPaletteEntryColor*, CurrentColorEntry)
        SLATE_ATTRIBUTE(int, CurrentSet)
        SLATE_EVENT(FOnPaletteChanged, OnPaletteChanged)
        SLATE_EVENT(FOnCurrentColorEntrySelected, OnCurrentColorEntrySelected)
        SLATE_EVENT(FOnCurrentSetSelected, OnCurrentSetSelected)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(const FArguments& InArgs);

public:
    class UOdysseyPalette* GetPalette() const;
    EVisibility GetTreeViewVisibility() const;

private:
    // Private internal callbacks
    void OnObjectChanged(const FAssetData& AssetData);
    FString ObjectPath() const;

private:
    class UOdysseyPaletteEntryColor* GetPaletteCurrentColorEntry() const;
    void OnPaletteCurrentColorEntrySelected(class UOdysseyPaletteEntryColor* iEntry);

private:
    void OnPaletteChanged();

private:
    // Private data
    TSharedPtr< FAssetThumbnailPool > mAssetThumbnailPool;

    TSlateAttribute<UOdysseyPalette*> mPaletteAttribute;
    UOdysseyPalette* mPalette;
    FOnPaletteChanged mOnPaletteChanged;
};
