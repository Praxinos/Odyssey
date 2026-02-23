// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeSegmentCubic.h"
#include "ArianeVertex.h"

FArianeSegmentCubic::~FArianeSegmentCubic()
{
}

FArianeSegmentCubic::FArianeSegmentCubic( FArianeObject* Owner, FArianeVertex* iVertex0, FArianeVertex* iVertex1 )
    : FArianeSegment( Owner, iVertex0, iVertex1 )
    , Handle0( this, iVertex0->GetPosition() )
    , Handle1( this, iVertex1->GetPosition() )
{
}
