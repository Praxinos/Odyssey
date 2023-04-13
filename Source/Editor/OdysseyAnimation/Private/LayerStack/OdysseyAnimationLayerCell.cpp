// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/OdysseyAnimationLayerCell.h"

#define LOCTEXT_NAMESPACE "FOdysseyAnimationLayerCell"

FOdysseyAnimationLayerCell::~FOdysseyAnimationLayerCell()
{
}

FOdysseyAnimationLayerCell::FOdysseyAnimationLayerCell(UOdysseyLayer* iLayer)
    : mLayer(iLayer)
    , mLength(1)
{
}

UOdysseyLayer*
FOdysseyAnimationLayerCell::GetLayer()
{
    return mLayer;
}

void
FOdysseyAnimationLayerCell::SetLength(uint32 iLength)
{
    mLength = iLength;
}

uint32
FOdysseyAnimationLayerCell::GetLength() const
{
    return mLength;
}

void
FOdysseyAnimationLayerCell::PostLoad()
{

}

void
FOdysseyAnimationLayerCell::PostDuplicate()
{

}

TSharedPtr<IOdysseyHandle>
FOdysseyAnimationLayerCell::Preload(int iFrameIndex)
{
    return nullptr;
}

void
FOdysseyAnimationLayerCell::Serialize(FArchive& Ar)
{
    Ar << mLength;
}

#undef LOCTEXT_NAMESPACE