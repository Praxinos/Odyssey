// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"

class FArianeObject;
class FArianeSegment;

class ARIANE_API FArianeHandleSegment
{
    public:
        ~FArianeHandleSegment();
        FArianeHandleSegment( FArianeSegment* iOwnerSegment, double iX, double iY, double iZ );
        FArianeHandleSegment( FArianeSegment* iOwnerSegment, const FVector& iPosition );

        void SetPosition( double iX, double iY, double iZ );
        void SetPosition( const FVector& iPosition );
        const FVector& GetPosition();

        void SetOwner( FArianeObject* iOwner );

    protected:
        FArianeSegment* OwnerSegment;
        FVector Position;

        TArray<uint32> Indices;
};
