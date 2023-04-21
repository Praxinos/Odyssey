// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/OdysseyAnimationCell.h"

#define LOCTEXT_NAMESPACE "FOdysseyAnimationCell"

FOdysseyAnimationCell::~FOdysseyAnimationCell()
{
}

FOdysseyAnimationCell::FOdysseyAnimationCell()
    : mLength(1)
{
}

void
FOdysseyAnimationCell::SetLength(int iLength)
{
    //TODO: mLength should be moved to the Layer (Layer Image Raster)
    mLength = iLength;
}

int
FOdysseyAnimationCell::GetLength() const
{
    return mLength;
}

void
FOdysseyAnimationCell::PostLoad()
{

}

void
FOdysseyAnimationCell::PostDuplicate()
{

}

void
FOdysseyAnimationCell::Serialize(FArchive& Ar)
{
    Ar << mLength;
}

#undef LOCTEXT_NAMESPACE