// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyMediaVector.h"

FOdysseyMediaVector::~FOdysseyMediaVector()
{
}

FOdysseyMediaVector::FOdysseyMediaVector(FOdysseyVectorGroupPaint* iScene)
    : mScene( iScene )
{
}

FOdysseyVectorGroupPaint*
FOdysseyMediaVector::GetScene() const
{
    return mScene;
}
