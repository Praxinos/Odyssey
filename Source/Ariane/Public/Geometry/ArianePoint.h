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
        virtual ~FArianePoint();
        FArianePoint(){};

        /**
         * @brief Constructor
         * @param X coordinate along the X axis.
         * @param Y coordinate along the Y axis.
         * @param Z coordinate along the Z axis.
         */
        FArianePoint( double X, double Y, double Z );

        /**
         * @brief Constructor
         * @param InPosition vertex's position.
         */
        FArianePoint( const FVector& InPosition );

        /** Get the vertex's position */
        const FVector& GetPosition();

        /**
         * @brief Set the vertex's position in space.
         * @param X coordinate along the X axis.
         * @param Y coordinate along the Y axis.
         * @param Z coordinate along the Z axis.
         */
        void SetPosition( double X, double Y, double Z );

        /**
         * @brief Set the vertex's position in space.
         * @param InPosition vertex's position.
         */
        void SetPosition( const FVector& InPosition );

        /** Run any object-specific task required immediately after undoing / redoing */
        virtual void PostEditUndo(){};

    protected:
        UPROPERTY( EditAnywhere )
        FVector Position;
};
