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
FInbetweenerDrawing::GetAnimationCellIndex()
{
    uint32 tagCellIndex = inbetweenerTag->GetSourceAnimationCellIndex();

    return (int32)tagCellIndex + (int32)( GetIndex() * (int)inbetweenerTag->GetInterpolationDirection());
}
