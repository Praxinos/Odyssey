// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
#include "Math/Vector2D.h"
// Ariane

struct FArianeObject;
struct FArianePoint;
struct FArianeSegment;

class ARIANE_API FArianeGraph
{
    struct FSection;
    struct FEdge;
    struct FNode;

    struct FPoint
    {
        FPoint( const FVector2D& InPosition, bool bProjected, const FVector& InOriginalPosition );

        FVector2D Position;
        bool bProjected;
        FVector OriginalPosition;
    };

    // a node represents a vertex or a segment's fraction point
    struct FNode : public FPoint
    {
        FNode( const FVector2D& InPosition, bool bProjected, const FVector& InOriginalPosition );

        TArray<FSection*> Sections;
        uint32 EdgeCount;
    };

    struct FIntersection
    {
        FIntersection( FNode* InNode, float InEdgeT );

        FNode* Node;
        float EdgeT;
    };

    struct FNodeIntersection : public FNode
    {
        // small temporary structure will allow us to alloc the intersection vertices in one go.
        // for X-Junction
        struct XRecord
        {
            FEdge* Edge0;
            FEdge* Edge1;
            double Edge0T;
            double Edge1T;
            FVector2D Position;

            XRecord( const FVector2D& InPosition
                   , FEdge* InEdge0
                   , double InEdge0T
                   , FEdge* InEdge1
                   , double InEdge1T );
        };

        FNodeIntersection( const FVector2D& Position
                         , FEdge* InEdge0
                         , double InEdge0T
                         , FEdge* InEdge1
                         , double InEdge1T );

        bool SelfIntersects();

        FIntersection Intersections[2];
        FEdge* Edges[2];
    };

    struct FFraction
    {
        FFraction( double InFromT, double InToT, FPoint* InPoint0, FPoint* InPoint1 );

        float FromT;
        float ToT;
        FPoint* Points[2];
        FBox2D BBox;
    };


    // and edge represents a segment
    struct FEdge
    {
        FEdge( FNode* InNode0
             , FNode* InNode1
             , FFraction* InFractions );

        FVector GetOriginalPosition( float T );

        FArianeSegment* Segment;

        FNode* Nodes[2];
        FFraction* Fractions;
        uint32 FractionCount;
        uint32 IntersectionSlot;
        TArray<FIntersection*> Intersections;
        FBox2D BBox;
    };

/* Later
    struct FEdgeCubic : public FEdge
    {
    public:
        FEdgeCubic( FNode* Vertex0, FVector FNode* Vertex1 );
    protected
        FVector2D Handles[2];
    };
*/

    struct FSection
    {
        int dummy;
    };

/* Later
    struct FSectionCubic : public FSectionCubic
    {
    };
*/

    struct FPath
    {
        FPath( uint32 InNodeCount, FNode* InNodes, uint32 InEdgeCount, FEdge* InEdges );

        uint32 NodeCount;
        FNode* Nodes; // we dont use TArray, for performance reasons
        uint32 EdgeCount;
        FEdge* Edges; // we dont use TArray, for performance reasons
    };

public:
    FArianeGraph();

    void Build( const FVector& ViewOrigin, const FPlane& ProjectionPlane, const TArray<FArianeObject*>& Objects );
    //Build( TArray<FOdysseyVectorObject*> Objects ); // Later

protected:
    void Import( const FVector& ViewOrigin, const FPlane& ProjectionPlane, const TArray<FArianeObject*>& Objects );
    void Intersect();
    void IntersectEdgeWithPath( FEdge* Edge, FPath* Path );
    void IntersectEdges( FEdge* Edge0
                       , FEdge* Edge1
                       , const FVector2D& Edge1MinWithTolerance
                       , const FVector2D& Edge1MaxWithTolerance
                       , TArray<FNodeIntersection::XRecord>& OutIntersectionRecordArray );

protected:
    FCriticalSection Mutex;
    // we store everything in one buffer per type, for performance
    TArray<FPoint> PointBuffer;
    TArray<FNode> NodeBuffer;
    TArray<FFraction> FractionBuffer;
    TArray<FEdge> EdgeBuffer;
    TArray<FSection> SectionBuffer;
    TArray<FNodeIntersection> IntersectionNodeBuffer;
    TArray<FPath> PathBuffer;
    TArray<FNodeIntersection::XRecord> XIntersectionRecordArray;
    //TArray<FEdgeCubic> CubicSegmentBuffer;
    bool bMultithreaded;
    double GapTolerance;
};
