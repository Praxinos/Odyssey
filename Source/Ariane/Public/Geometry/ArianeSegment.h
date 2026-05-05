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
        struct FractionStep
        {
            ~FractionStep(){};
            FractionStep( FArianePoint* InPoint, float InT, float InRadius )
                : Point( InPoint )
                , T ( InT )
                , Radius( InRadius )
            {
            };

            FArianePoint* Point;
            float T;
            float Radius;
        };

        struct Fraction
        {
            ~Fraction(){};
            Fraction( FractionStep* S0, FractionStep* S1 )
                : Steps { S0, S1 }
            {
            };

            FractionStep* Steps[2];
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
        const TArray<FractionStep>& GetFractionSteps();

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
        const TArray<Fraction>& GetFractions();

        /** Get the fraction count */
        uint32 GetFractionCount();

        /**
         * @brief Get the tangent vector at parametric value T
         * @param T
         * @param bNormalize normalize the result
         * @return the tangent vector at parametric value T
         */
        FVector GetTangentVectorAt( double T, bool bNormalize );

        FVector GetVectorLeavingFromVertex( FArianeVertex* Vertex, bool bNormalize );

        /** Get the segment's bounds ( including polygonal geometry ) */
        const FBoxSphereBounds& GetBounds();

        /** Update the segment */
        void Update();

        /** Invalidate the segment */
        void Invalidate();
        bool IsInvalidated();

        const FGuid& GetGuid();

        FVector GetPointAt( double T );
        FVector GetNormalAt( double T );
        virtual FArianeSegment* Extract( FArianeObject* NewSegmentOwner
                                       , FArianeVertex* NewSegmentVertex0
                                       , float T0
                                       , FArianeVertex* NewSegmentVertex1
                                       , float T1 );

    protected:
        /** Update the segment's bounds */
        void UpdateBounds();

        /** Update the segment */
        void Init();

    public:
        UPROPERTY( EditAnywhere )
        FArianeVertexID Vertices[2];

        UPROPERTY( EditAnywhere )
        FGuid Guid;

        UPROPERTY( EditAnywhere )
        FArianeObjectID OwnerID;

    protected:
        TArray<FractionStep> FractionSteps;
        TArray<Fraction> Fractions;
        TArray<FDynamicMeshVertex> ModelVertexCache;
        TArray<uint32> IndexCache;
        FBoxSphereBounds Bounds;
        double Length;
        bool bInvalidated;
};
