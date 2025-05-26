// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorPaletteSet.h"

#include "Palette/OdysseyPaletteEntryColor.h"

FOdysseyPainterEditorPaletteSet::FOdysseyPainterEditorPaletteSet()
    : mPalette(nullptr)
    , mSet(0)
{
}

FOdysseyPainterEditorPaletteSet::FOdysseyPainterEditorPaletteSet(UOdysseyPalette* iPalette, int iSet)
    : mPalette(iPalette)
    , mSet(iSet)
{
}

void
FOdysseyPainterEditorPaletteSet::Reset()
{
    mPalette = nullptr;
    mSet = 0;
}

UOdysseyPalette*
FOdysseyPainterEditorPaletteSet::GetPalette() const
{
    return mPalette;
}

int
FOdysseyPainterEditorPaletteSet::GetSet() const
{
    return mSet;
}

void
FOdysseyPainterEditorPaletteSet::SetPalette(UOdysseyPalette* iPalette)
{
    mPalette = iPalette;
}

void
FOdysseyPainterEditorPaletteSet::SetSet(int iSet)
{
    mSet = iSet;
}

bool
FOdysseyPainterEditorPaletteSet::operator==( const FOdysseyPainterEditorPaletteSet& iRhs ) const
{
    return mPalette == iRhs.mPalette && mSet == iRhs.mSet;
}

/*


FOdysseyPainterEditorPaletteEntryColor::FOdysseyPainterEditorPaletteEntryColor()
    : mEntry(nullptr)
    , mSet(0)
{

}

FOdysseyPainterEditorPaletteEntryColor::FOdysseyPainterEditorPaletteEntryColor(UOdysseyPaletteEntryColor* iEntry, int iSet)
    : mEntry(iEntry)
    , mSet(iSet)
{

}

UOdysseyPaletteEntryColor*
FOdysseyPainterEditorPaletteEntryColor::GetEntry() const
{
    return mEntry;
}

int
FOdysseyPainterEditorPaletteEntryColor::GetSet() const
{
    return mSet;
}

void
FOdysseyPainterEditorPaletteEntryColor::SetEntry(UOdysseyPaletteEntryColor* iEntry)
{
    mEntry = iEntry;
}

void
FOdysseyPainterEditorPaletteEntryColor::SetSet(int iSet)
{
    mSet = iSet;
}

void
FOdysseyPainterEditorPaletteEntryColor::Reset()
{
    mEntry = nullptr;
    mSet = 0;
}

FOdysseyPainterEditorPaletteSet
FOdysseyPainterEditorPaletteEntryColor::GetPaletteSet() const
{
    return FOdysseyPainterEditorPaletteSet(mEntry ? mEntry->GetPalette() : nullptr, mSet);
}

bool
FOdysseyPainterEditorPaletteEntryColor::operator==( const FOdysseyPainterEditorPaletteEntryColor& iRhs ) const
{
    return mEntry == iRhs.mEntry && mSet == iRhs.mSet;
} */
