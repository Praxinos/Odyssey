// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyMask.h"

FOdysseyMask::~FOdysseyMask()
{
}

FOdysseyMask::FOdysseyMask()
{
}

void FOdysseyMask::AddFromPointsAndBlock(TArray<FVector2D> iPoints, ::ULIS::FBlock* iBlock)
{
}

void FOdysseyMask::ClearMask()
{
    for( int i = mMaskZones.Num(); i > 0; i-- )
    {
        delete mMaskZones[i].mBlock;
    }
    mMaskZones.Empty();
}
