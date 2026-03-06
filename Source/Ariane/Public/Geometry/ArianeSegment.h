// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "IndexTypes.h"
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
        struct Fraction
        {
            ~Fraction(){};
            Fraction( FArianePoint* P0, FArianePoint* P1 )
                : Points { P0, P1 }
            {
            };

            FArianePoint* Points[2];
        };

    public:
        ~FArianeSegment();
        FArianeSegment();
        FArianeSegment( FArianeObject* Owner, FArianeVertex* iVertex0, FArianeVertex* iVertex1 );

        void SetOwner( FArianeObject* iOwner );
        FArianeObject* GetOwner();
        virtual void PostLoad();
        virtual void PostEditUndo();
        void Link();
        void Unlink();

        FArianeVertex* GetVertex( uint32 Index );
        FArianeVertex* GetOtherVertex( FArianeVertex* Vertex );
        const TArray<FArianePoint*>& GetFractionPoints();

        void AllocateCache( uint32 VertexCount, uint32 TriangleCount );

        const TArray<FModelVertex>& GetModelVertexCache();
        const TArray<uint32>& GetIndexCache();
        const TArray<Fraction>& GetFractionCache();

        FVector GetTangentVectorAt( double T, bool bNormalize );

        uint32 GetFractionCount();
        FVector GetAverageVectorAt( double T );
        FVector GetVectorLeavingFromVertex( FArianeVertex* Vertex, bool bNormalize );
        float GetFractionPointT( uint32 FractionPointIndex );

        const FBoxSphereBounds& GetBounds();
        void UpdateBounds();
        void Update();
        void Init();
        void Invalidate();

    public:
        UPROPERTY( EditAnywhere )
        FArianeVertexID Vertices[2];

        UPROPERTY( EditAnywhere )
        FGuid Guid;

        UPROPERTY( EditAnywhere )
        FArianeObjectID OwnerID;

    protected:
        TArray<float> FractionPointsT;
        TArray<float> FractionPointsRadius;
        TArray<FArianePoint*> FractionPoints;
        TArray<Fraction> FractionCache;
        TArray<FModelVertex> ModelVertexCache;
        TArray<uint32> IndexCache;
        FBoxSphereBounds Bounds;
        double Length;
};
