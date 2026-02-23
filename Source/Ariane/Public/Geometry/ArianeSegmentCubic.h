// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"
// Ariane headers
#include "ArianeSegment.h"
#include "ArianeHandleSegment.h"

class FArianeHandleSegment;

class ARIANE_API FArianeSegmentCubic : public FArianeSegment
{
    public:
        ~FArianeSegmentCubic();
        FArianeSegmentCubic( FArianeObject* Owner, FArianeVertex* iVertex0, FArianeVertex* iVertex1 );

    protected:
        FArianeHandleSegment Handle0;
        FArianeHandleSegment Handle1;
};
