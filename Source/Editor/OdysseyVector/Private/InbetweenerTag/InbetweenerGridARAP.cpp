#include "InbetweenerTag/InbetweenerGridARAP.h"
#include "OdysseyVectorTagInbetweener.h"

FInbetweenerGridARAP::FInbetweenerGridARAP( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                          , uint32 iNumQuadX
                                          , uint32 iNumQuadY )
    : FInbetweenerGrid( iInbetweenerTag, iNumQuadX, iNumQuadY )
{
}

void
FInbetweenerGridARAP::Make( uint32 iNumQuadX, uint32 iNumQuadY )
{
    FInbetweenerGrid::Make( iNumQuadX, iNumQuadY );
}
