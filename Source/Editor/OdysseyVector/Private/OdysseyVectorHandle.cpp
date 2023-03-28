#include "OdysseyVectorHandle.h"

FOdysseyVectorHandle::~FOdysseyVectorHandle()
{
}

FOdysseyVectorHandle::FOdysseyVectorHandle()
{
}

void
FOdysseyVectorHandle::Init( double iX, double iY )
{
    FOdysseyVectorPoint::Init( iX, iY, 0.0f );
}

::ULIS::FVec2D&
FOdysseyVectorHandle::GetCoords()
{
    return mCoords;
}
