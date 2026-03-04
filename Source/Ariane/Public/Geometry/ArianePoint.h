// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"

#include "ArianePoint.generated.h"

USTRUCT(BlueprintType)
struct ARIANE_API FArianePoint
{
    GENERATED_BODY()

    public:
        ~FArianePoint();
        FArianePoint(){};
        FArianePoint( double iX, double iY, double iZ );
        FArianePoint( const FVector& iPosition );

        const FVector& GetPosition();

        virtual void PostEditUndo(){};

    public:
        UPROPERTY( EditAnywhere )
        FVector Position;
};
