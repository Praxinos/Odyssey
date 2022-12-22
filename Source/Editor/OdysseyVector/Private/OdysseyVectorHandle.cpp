#include "OdysseyVectorHandle.h"

UOdysseyVectorHandle::~UOdysseyVectorHandle()
{
}

UOdysseyVectorHandle::UOdysseyVectorHandle()
{
}

void
UOdysseyVectorHandle::Init( double iX, double iY )
{
    UOdysseyVectorPoint::Init( iX, iY, 0.0f );
}
