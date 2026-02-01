// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "ArianePoint.h"

class FArianeObject;
class FArianeSegment;

class ARIANE_API FArianeVertex : public FArianePoint
{
    public:
        ~FArianeVertex();
        FArianeVertex( double X, double Y, double Z );
        FArianeVertex( const FVector& iPosition );

        void AddSegment( FArianeSegment* iSegment );
        void RemoveSegment( FArianeSegment* iSegment );

        void SetOwner( FArianeObject* iOwner );
        const TArray<FArianeSegment*>& GetSegments();

    protected:
        FArianeObject* Owner;

        double Radius;

        TArray<FArianeSegment*> Segments;
};
