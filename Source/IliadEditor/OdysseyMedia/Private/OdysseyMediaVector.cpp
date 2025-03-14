// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyMediaVector.h"

IMPLEMENT_ODYSSEY_MEDIA(FOdysseyMediaVector)

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
