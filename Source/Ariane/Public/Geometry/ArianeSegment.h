// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "IndexTypes.h"
#include "DynamicMeshBuilder.h"
// Ariane Headers
#include "ArianeVertex.h"
#include "ArianeID.h"

#include "ArianeSegment.generated.h"

struct FArianeObject;
struct FArianeVertex;

USTRUCT(BlueprintType)
struct ARIANE_API FArianeSegment
{
    GENERATED_BODY()

    public:
        struct FFractionStep
        {
            ~FFractionStep(){};
            FFractionStep( FArianePoint* InPoint, float InT, float InRadius )
                : Point( InPoint )
                , T ( InT )
                , Radius( InRadius )
            {
            };

            FArianePoint* Point;
            float T;
            float Radius;
        };

        struct FFraction
        {
            ~FFraction(){};
            FFraction( FFractionStep* S0, FFractionStep* S1 )
                : Steps { S0, S1 }
                , Length( ( S1->Point->GetPosition() - S0->Point->GetPosition() ).Length() )
            {
            };

            FFractionStep* Steps[2];
            double Length;
        };

    public:
        virtual ~FArianeSegment();
        FArianeSegment();

        /**
         * @brief Constructor
         * @param Owner owner object
         * @param Vertex0 first vertex
         * @param Vertex1 second vertex
         */
        FArianeSegment( FArianeObject* InOwner, FArianeVertex* Vertex0, FArianeVertex* Vertex1 );

        /** Get the segment's owner object */
        FArianeObject* GetOwner();

        /** Run any object-specific task required immediately after loading */
        virtual void PostLoad();

        /** Run any object-specific task required immediately after undoing / redoing */
        virtual void PostEditUndo();

        /** Add this segment to the connected vertices' list of segments */
        void Link();

        /** Remove this segment to the connected vertices' list of segments */
        void Unlink();

        /**
         * @brief Get the vertex corresponding to the index passed as an argument
         * @param Index MUST be 0 or 1
         * @return a pointer to the vertex
         */
        FArianeVertex* GetVertex( uint32 Index );

        /**
         * @brief Get the vertex that is at the oppossite of the one passed as an argument
         * @param Vertex
         * @return a pointer to the vertex opposite vertex
         */
        FArianeVertex* GetOtherVertex( FArianeVertex* Vertex );

        /** Get an array of fraction points composing the segment */
        const TArray<FFractionStep>& GetFractionSteps();

        /**
         * @brief Allocate FDynamicMeshVertex cache and Index cache for building a polygonal shape
         * @param VertexCount number of ModelVertex requested
         * @param TriangleCount number of Triangles requested
         */
        void AllocateCache( uint32 VertexCount, uint32 TriangleCount );

        /** Get the FDynamicMeshVertex cache */
        const TArray<FDynamicMeshVertex>& GetModelVertexCache();

        /** Get the Index cache */
        const TArray<uint32>& GetIndexCache();

        /** Get the fraction cache */
        const TArray<FFraction>& GetFractions();

        /** Get the fraction count */
        uint32 GetFractionCount();

        /**
         * @brief Get the tangent vector at parametric value T
         * @param T
         * @param bNormalize normalize the result
         * @return the tangent vector at parametric value T
         */
        virtual FVector GetTangentVectorAt( double T, bool bNormalize );

        virtual FVector GetVectorLeavingFromVertex( FArianeVertex* Vertex, bool bNormalize );

        /** Get the segment's bounds ( including polygonal geometry ) */
        const FBoxSphereBounds& GetBounds();

        /** Update the segment */
        virtual void Update();

        /** Invalidate the segment */
        void Invalidate();

        /** Get the segment's invalidation status */
        bool IsInvalidated();

        /** Get the segment's Guid */
        const FGuid& GetGuid();

        /**
         * @brief Get the point at parameter T
         * @param T must be on interval 0.0 - 1.0
         * @return the position of the point
         */
        virtual FVector GetPointAt( double T );

        /**
         * @brief Get the normal at parameter T, interpolated from both endpoints
         * @param T must be on interval 0.0 - 1.0
         * @return the normal of the point
         */
        virtual FVector GetNormalAt( double T );

        /**
         * @brief extract a smaller segment from this segment
         * @param NewSegmentOwner the owner for the new segment
         * @param NewSegmentVertex0 the vertex 0 to attach to the new segment
         * @param T0 parametric value for the endpoint 0
         * @param NewSegmentVertex1 the vertex 1 to attach to the new segment
         * @param T0 parametric value for the endpoint 1
         * @return the newly created segment
         */
        virtual FArianeSegment* Extract( FArianeObject* NewSegmentOwner
                                       , FArianeVertex* NewSegmentVertex0
                                       , float T0
                                       , FArianeVertex* NewSegmentVertex1
                                       , float T1 );

        /** Get segment length **/
        double GetLength();

    protected:
        /** Update the segment's bounds */
        void UpdateBounds();

        /** Update the segment */
        void Init();

    public:
        UPROPERTY( EditAnywhere, meta = (IgnoreForMemberInitializationTest) )
        FGuid Guid;

        UPROPERTY( EditAnywhere )
        FArianeObjectID OwnerID;

        UPROPERTY( EditAnywhere )
        FArianeVertexID Vertices[2];

    protected:
        TArray<FFractionStep> FractionSteps;
        TArray<FFraction> Fractions;
        TArray<FDynamicMeshVertex> ModelVertexCache;
        TArray<uint32> IndexCache;
        FBoxSphereBounds Bounds;
        double Length;
        bool bInvalidated;
};
