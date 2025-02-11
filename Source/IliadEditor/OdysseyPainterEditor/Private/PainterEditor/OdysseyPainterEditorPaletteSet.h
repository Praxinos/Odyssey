// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"

class UOdysseyPalette;
class UOdysseyPaletteEntryColor;

class FOdysseyPainterEditorPaletteSet
{
public:
    FOdysseyPainterEditorPaletteSet();
    FOdysseyPainterEditorPaletteSet(UOdysseyPalette* iPalette, int iSet);

public:
    bool operator==( const FOdysseyPainterEditorPaletteSet& iRhs ) const;

public:
    void Reset();

    UOdysseyPalette* GetPalette() const;
    int GetSet() const;

    void SetPalette(UOdysseyPalette* iPalette);
    void SetSet(int iSet);

private:
    UOdysseyPalette* mPalette = nullptr;
    int mSet = 0;
};

/* class FOdysseyPainterEditorPaletteEntryColor
{
public:
    FOdysseyPainterEditorPaletteEntryColor();
    FOdysseyPainterEditorPaletteEntryColor(UOdysseyPaletteEntryColor* iEntry, int iSet);

public:
    bool operator==( const FOdysseyPainterEditorPaletteEntryColor& iRhs ) const;

public:
    void Reset();

    UOdysseyPaletteEntryColor* GetEntry() const;
    int GetSet() const;

    void SetEntry(UOdysseyPaletteEntryColor* iEntry);
    void SetSet(int iSet);

    FOdysseyPainterEditorPaletteSet GetPaletteSet() const;

private:
    UOdysseyPaletteEntryColor* mEntry;
    int mSet;
}; */
