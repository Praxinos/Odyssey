// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "IndexTypes.h"
#include "ArianePoint.h"

class FArianeObject;
class FArianeVertex;


class ARIANE_API FArianeSegment
{
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
        FArianeSegment( FArianeVertex* iVertex0, FArianeVertex* iVertex1 );

        void SetOwner( FArianeObject* iOwner );

        void Link();
        void Unlink();

        FArianeVertex* GetVertex(uint32 Index);
        const TArray<FArianePoint*>& GetFractionPoints();

        void AllocateCache( uint32 VertexCount, uint32 TriangleCount );

        const TArray<FModelVertex>& GetModelVertexCache();
        const TArray<uint32>& GetIndexCache();
        const TArray<Fraction>& GetFractionCache();

        FVector GetVectorAt( double T, bool bNormalize );

        uint32 GetFractionCount();
        FVector GetAverageVectorAt( double T );
        FVector GetVectorLeavingFromVertex( FArianeVertex* Vertex, bool bNormalize );
        uint32 GetFractionPointT( uint32 FractionPointIndex );

    protected:
        FArianeObject* Owner;
        FArianeVertex* Vertices[2];

        TArray<float> FractionPointsT;
        TArray<FArianePoint*> FractionPoints;
        TArray<Fraction> FractionCache;
        TArray<FModelVertex> ModelVertexCache;
        TArray<uint32> IndexCache;
};
