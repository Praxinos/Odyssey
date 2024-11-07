// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "FOdysseyPalette.h"

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

UOdysseyPalette* FOdysseyPalette::GetPalette() const
{
    return mPalette;
}

void FOdysseyPalette::SetPalette(UOdysseyPalette* iColorPalette)
{
    mPalette = iColorPalette;
}
