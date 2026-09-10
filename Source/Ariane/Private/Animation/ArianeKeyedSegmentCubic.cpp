// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Headers
#include "ArianeKeyedSegmentCubic.h"
#include "ArianeSegmentCubic.h"

FArianeKeyedSegmentCubic::~FArianeKeyedSegmentCubic()
{
}

FArianeKeyedSegmentCubic::FArianeKeyedSegmentCubic()
{
}

FArianeKeyedSegmentCubic::FArianeKeyedSegmentCubic( FArianeSegmentCubic* CubicSegment )
    : FArianeKeyedSegment( CubicSegment )
{
    HandlePositions[0] = CubicSegment->GetHandle((uint32)0)->GetPosition();
    HandlePositions[1] = CubicSegment->GetHandle((uint32)1)->GetPosition();
}

const FVector&
FArianeKeyedSegmentCubic::GetHandlePosition(uint32 Index) const
{
    return HandlePositions[Index];
}
