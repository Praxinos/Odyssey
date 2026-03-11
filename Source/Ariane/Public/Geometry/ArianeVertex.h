// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "ArianeID.h"
#include "ArianePoint.h"
#include "ArianeVertex.generated.h"

struct FArianeObject;
struct FArianeSegment;
class UArianePainting3DComponent;

USTRUCT(BlueprintType)
struct ARIANE_API FArianeVertex : public FArianePoint
{
    GENERATED_BODY()

    public:
        ~FArianeVertex();
        FArianeVertex(){};

        /**
         * @brief Constructor
         * @param InPosition vertex's position
         * @param InNormal vertex's normal vector
         * @param InRadius vertex's radius
         */
        FArianeVertex( FArianeObject* Owner, const FVector& InPosition, const FVector& InNormal, double InRadius );

        /**
         * @brief Add a segment to the list of segments
         * @param Segment
         */
        void AddSegment( FArianeSegment* Segment );

        /**
         * @brief Remove a segment from the list of segments
         * @param Segment
         */
        void RemoveSegment( FArianeSegment* iSegment );

        /** Invalidate connected segments */
        void InvalidateSegments();

        /** Invalidate this vertex. It will invalidate connected segments as well */
        void Invalidate();

        /** Get connected segments */
        const TArray<FArianeSegment*>& GetSegments();

        /** Get this vertex's owner */
        FArianeObject* GetOwner();

        /** Get this vertex's radius */
        double  GetRadius();

        /**
         * @brief Set this vertezx's normal vector
         * @param InNormal the normal vector
         */
        void SetNormal( const FVector& InNormal );

        /** Get this vertex's normal vector */
        const FVector& GetNormal();

    public:
        UPROPERTY( EditAnywhere )
        FGuid Guid;

        UPROPERTY( EditAnywhere )
        FArianeObjectID OwnerID;

        UPROPERTY( EditAnywhere )
        double Radius;

        UPROPERTY( EditAnywhere )
        FVector Normal;

    protected:
        TArray<FArianeSegment*> Segments;
};
