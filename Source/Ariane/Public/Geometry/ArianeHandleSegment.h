// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"
// Ariane headers
#include "ArianePoint.h"

#include "ArianeHandleSegment.generated.h"

struct FArianeObject;
struct FArianeSegment;

USTRUCT(BlueprintType)
struct ARIANE_API FArianeHandleSegment : public FArianePoint
{
    GENERATED_BODY()

    public:
        ~FArianeHandleSegment();
        FArianeHandleSegment(){};

        /**
         * @brief Constructor
         * @param InOwnerSegment handle's owner segment
         * @param X the desired handle's X position
         * @param Y the desired handle's Y position
         * @param Z the desired handle's Z position
         */
        FArianeHandleSegment( FArianeSegment* iOwnerSegment, double X, double Y, double Z );

        /**
         * @brief Constructor
         * @param InOwnerSegment handle's owner segment
         * @param InPosition the desired handle's position
         */
        FArianeHandleSegment( FArianeSegment* InOwnerSegment, const FVector& InPosition );

        /**
         * @brief Set the handle's position
         * @param X the desired handle's X position
         * @param Y the desired handle's Y position
         * @param Z the desired handle's Z position
         */
        void SetPosition( double X, double Y, double Z );

        /**
         * @brief Set the handle's position
         * @param InPosition the desired handle's position
         */
        void SetPosition( const FVector& InPosition );

        /**
         * @brief Get the handle's position
         * @return The handle's position
         */
        const FVector& GetPosition();

        /**
         * @brief Set the handle's owner segment
         * @param the handle's owner segment
         */
        void SetOwnerSegment( FArianeSegment* InOwnerSegment );

    protected:
        FArianeSegment* OwnerSegment;
};
