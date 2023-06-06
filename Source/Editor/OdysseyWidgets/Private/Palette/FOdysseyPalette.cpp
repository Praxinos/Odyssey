// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "FOdysseyPalette.h"

#define LOCTEXT_NAMESPACE "OdysseyPalette"


/////////////////////////////////////////////////////
// FOdysseyMeshSelector
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPalette::FOdysseyPalette() :
    mPalette(nullptr)
{
}


FOdysseyPalette::~FOdysseyPalette()
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Getter / Setter

UOdysseyPalette* FOdysseyPalette::GetColorPalette() const
{
    return mPalette;
}

void FOdysseyPalette::SetColorPalette(UOdysseyPalette* iColorPalette)
{
    mPalette = iColorPalette;
}

void FOdysseyPalette::AddNewPaletteEntry()
{
/*
    int32 num = mPalette->mPaletteEntries.Num() + 1;
    FString formattedString = FString::Printf(TEXT("Entry %d"), num);
    FName newEntryText = FName(*formattedString);

    //mPalette->mPaletteMap.Add(newEntryText, FColor(200, 200, 100));*/
}

//CALLBACKS -------------------------------------------


#undef LOCTEXT_NAMESPACE
