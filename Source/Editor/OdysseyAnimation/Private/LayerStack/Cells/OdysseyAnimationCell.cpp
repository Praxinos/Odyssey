// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/OdysseyAnimationCell.h"

#define LOCTEXT_NAMESPACE "FOdysseyAnimationCell"

FOdysseyAnimationCell::FOnLengthChanged&
FOdysseyAnimationCell::OnLengthChanged()
{
    static FOnLengthChanged& onLengthChanged;
    return onLengthChanged;
}

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
    mLength = iLength;
    OnLengthChanged().Broadcast(this);
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