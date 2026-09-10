// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Headers
#include "ArianeKeyedSegment.h"
#include "ArianeSegment.h"

FArianeKeyedSegment::~FArianeKeyedSegment()
{
}

FArianeKeyedSegment::FArianeKeyedSegment()
{
}


FArianeKeyedSegment::FArianeKeyedSegment( FArianeSegment* Segment )
    : Guid( Segment->GetGuid() )
{
}

const FGuid&
FArianeKeyedSegment::GetGuid()
{
    return Guid;
}
