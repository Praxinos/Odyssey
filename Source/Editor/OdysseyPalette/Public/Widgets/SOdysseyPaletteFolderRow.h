// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "SOdysseyPaletteEntryRow.h"

/**
 * Implements an entry row widget
 */
class ODYSSEYPALETTE_API SOdysseyPaletteFolderRow
    : public SOdysseyPaletteEntryRow
{
public:
    SLATE_BEGIN_ARGS(SOdysseyPaletteFolderRow)
        {}
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(const FArguments& iArgs, const TSharedRef<SOdysseyPaletteTreeView>& iOwnerTableView, class UOdysseyPaletteEntryFolder* iFolderEntry);

private:
    virtual TSharedRef<SWidget> GenerateHeaderWidget() override;

private:
    class UOdysseyPaletteEntryFolder* mFolderEntry;
};
