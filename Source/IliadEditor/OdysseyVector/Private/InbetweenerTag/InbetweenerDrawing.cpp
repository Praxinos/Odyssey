// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "InbetweenerTag/InbetweenerDrawing.h"
#include "OdysseyVectorTagInbetweener.h"

FInbetweenerDrawing::FInbetweenerDrawing( FOdysseyVectorTagInbetweener* iInbetweenerTag )
    : inbetweenerTag( iInbetweenerTag )
{
}

uint32
FInbetweenerDrawing::GetIndex()
{
    return this - &inbetweenerTag->GetDrawingBuffer()[0];
}

int32
FInbetweenerDrawing::GetCellIndex()
{
    uint32 tagCellIndex = inbetweenerTag->GetSourceCellIndex();

    return (int32)tagCellIndex + (int32)( GetIndex() * (int)inbetweenerTag->GetInterpolationDirection());
}
