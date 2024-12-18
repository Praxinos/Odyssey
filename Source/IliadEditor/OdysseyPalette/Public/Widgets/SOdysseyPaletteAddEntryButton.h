// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class UOdysseyPalette;
class UOdysseyPaletteEntry;

/**
 * Implements the Layer stack widget
 */
class ODYSSEYPALETTE_API SOdysseyPaletteAddEntryButton
    : public SCompoundWidget
{
public:
    DECLARE_DELEGATE_OneParam(FOnAdded, UOdysseyPaletteEntry*);

public:

    SLATE_BEGIN_ARGS(SOdysseyPaletteAddEntryButton)
        {}
        SLATE_ATTRIBUTE( UOdysseyPalette*, Palette )
        SLATE_EVENT( FOnAdded, OnAdded )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyPaletteAddEntryButton();
    void Construct(const FArguments& InArgs);

private:
    //PRIVATE API
    FReply AddColorEntry();
    FReply AddMaterialEntry();
    FReply AddFolderEntry();
    TSharedRef<SWidget> MakeMenu();
    void AddEntryFromClass(FAssetData iAssetData);

private:
    TAttribute<UOdysseyPalette*> mPalette;
    FOnAdded mOnAdded;
};
