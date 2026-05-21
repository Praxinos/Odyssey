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
        virtual ~FArianeVertex();
        FArianeVertex();

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

        /** Get this vertex's Guid */
        const FGuid& GetGuid();

        /**
         * @brief Get a segment connected to this vertex and that is NOT the one passed as a parameter
         * @param Segment the segment that should not be returned
         */
        FArianeSegment* GetOtherSegment( FArianeSegment* Segment );

        /** Check the vertex belongs to a chain */
        bool IsChained() { return bChained; };

        /** Check if the vertex belongs to a chain */
        void SetChained( bool bInChained ) { bChained = bInChained; };

        /** Get the first segment attached to this vertex **/
        FArianeSegment* GetFirstSegment();

        /**
         * @brief Get the index of this vertex on the segment passed as a parameter
         * @param Segment
         * @result either 0, 1 or -1 if the vertex does not belong to the segment
         */
        int32 GetIndex( FArianeSegment* Segment );

        /** Set the vertex ID. Differs from the Guid. For algorithmic use only: if you need to store an index in an array, for example **/
        void SetID( uint32 InID );

        /** Set the vertex ID. For algorithmic use only **/
        uint32 GetID();

        /** Set the vertex's radius **/
        void SetRadius( double InRadius );

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
        uint32 ID;

        bool bChained : 1;
};
