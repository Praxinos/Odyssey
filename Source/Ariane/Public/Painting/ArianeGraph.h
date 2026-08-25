// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
#include "Math/Vector2D.h"
// Ariane
// Blend2D
#include <blend2d.h>

struct FArianeObject;
struct FArianePoint;
struct FArianeSegment;

class ARIANE_API FArianeGraph
{
public:
    struct FSection;
    struct FEdge;
    struct FNode;
    struct FCycle;
    struct FPath;

    struct FExplorationPair
    {
        FSection* ReturnSection;
        FNode* DepartNode;
        uint32 DepartNodeIndex;
        FSection* DepartSection;
        double SectionLength;

        virtual ~FExplorationPair();
        FExplorationPair();
        FExplorationPair( FSection* ReturnSection
                        , FNode* DepartVertex
                        , uint32 DepartVertexIndex
                        , FSection* DepartSection );
    };

    struct ARIANE_API FSectionLinkInfo
    {
        FSection* Section;
        uint32 SectionNodeIndex;

        // copy constructor
        FSectionLinkInfo( const FSectionLinkInfo& SectionLinkInfo )
        {
            this->Section          = SectionLinkInfo.Section;
            this->SectionNodeIndex = SectionLinkInfo.SectionNodeIndex;
        }

        FSectionLinkInfo( FSection* InSection, uint32 InSectionNodeIndex )
        {
            Section = InSection;
            SectionNodeIndex = InSectionNodeIndex;
        }

       FVector2D GetVector();
    };

    struct ARIANE_API FPoint
    {
        static uint32 StaticClass(){ return 0x336f5643; }; // crc32 FArianeGraph::FPoint
        virtual uint32 GetClass(){ return StaticClass(); };

        virtual ~FPoint();
        FPoint( const FVector2D& InPosition, bool bProjected, const FVector& InOriginalWorlPosition );

        FVector2D Position;
        bool bProjected;
        FVector OriginalWorlPosition;
    };

    // a node represents a vertex or a segment's fraction point
    struct ARIANE_API FNode : public FPoint
    {
        static uint32 StaticClass(){ return  0xf19b5156; }; // crc32 FArianeGraph::FNode
        virtual uint32 GetClass() override { return StaticClass(); };

        virtual ~FNode();
        FNode( const FVector2D& InPosition, bool bProjected, const FVector& InOriginalWorlPosition );
        void AddSection( FSection* Section, uint32 SectionNodeIndex );
        void RemoveSection( FSection* Section, uint32 SectionNodeIndex );
        FSectionLinkInfo* GetSectionLinkInfo( FSection* Section, uint32 SectionNodeIndex );
        FSectionLinkInfo* GetCycleNextSection( FSectionLinkInfo* LastSectionLinkInfo, double Orientation );
        FSectionLinkInfo* GetOtherSectionLinkInfo( FSectionLinkInfo* LastSectionLinkInfo );
        FSection* GetSection( FEdge* Edge );
        FEdge *GetOtherEdge( FEdge* Edge );
        int32 GetIndex( FEdge* Edge );
        void BuildExplorationPairs( TArray<FExplorationPair>& OutExplorationPairsArray );
        void UnlinkPendantSections();

        TArray<FSectionLinkInfo> SectionLinkInfos;
        FEdge* Edges[2];
        uint32 EdgeCount;
    };

    struct ARIANE_API FIntersection
    {
        virtual ~FIntersection();
        FIntersection( FNode* InNode, float InEdgeT );

        FNode* Node;
        float EdgeT;
    };

    struct ARIANE_API FNodeIntersection : public FNode
    {
        static uint32 StaticClass(){ return 0xb1317d8a; }; // crc32 FArianeGraph::FNodeIntersection
        virtual uint32 GetClass() override { return StaticClass(); };

        // small temporary structure will allow us to alloc the intersection vertices in one go.
        // for X-Junction
        struct XRecord
        {
            FEdge* Edge0;
            FEdge* Edge1;
            double Edge0T;
            double Edge1T;
            FVector2D Position;

            virtual ~XRecord();
            XRecord( const FVector2D& InPosition
                   , FPath* InPath0
                   , FEdge* InEdge0
                   , double InEdge0T
                   , FPath* InPath1
                   , FEdge* InEdge1
                   , double InEdge1T );
        };

        virtual ~FNodeIntersection();
        FNodeIntersection( const FVector2D& Position
                         , FEdge* InEdge0
                         , double InEdge0T
                         , FEdge* InEdge1
                         , double InEdge1T );

        bool SelfIntersects();

        FIntersection Intersections[2];
        FEdge* Edges[2];
    };

    struct ARIANE_API FFraction
    {
        virtual ~FFraction();
        FFraction( double InFromT, double InToT, FPoint* InPoint0, FPoint* InPoint1 );

        float FromT;
        float ToT;
        FPoint* Points[2];
        FBox2D BBox;
    };


    // and edge represents a segment
    struct ARIANE_API FEdge
    {
        virtual uint32 GetClass() = 0;

        virtual ~FEdge();
        FEdge( FNode* InNode0
             , FNode* InNode1
             , uint32 InFractionCount
             , FFraction* InFractions
             , double InLength );
        FNode* GetOtherNode( FNode* Node );

        virtual FVector GetOriginalWorlPosition( float T ) = 0;
        void CreateSections( TArray<FSection>& SectionBuffer
                           , bool bStitchShortSections
                           , TArray<FSection*>& ShortSections );
        void AddIntersection ( FIntersection* iIntersection );

        //FArianeSegment* Segment;
        FNode* Nodes[2];
        FFraction* Fractions;
        uint32 FractionCount;
        uint32 IntersectionSlotCount;
        TArray<FIntersection*> Intersections;
        //FSection* Sections;
        //uint32 SectionCount;
        FBox2D BBox;
        double Length;
    };

    struct ARIANE_API FEdgeLinear : FEdge
    {
        static uint32 StaticClass(){ return  0xffabad9f; }; // crc32 FArianeGraph::FEdgeLinear
        virtual uint32 GetClass() override { return StaticClass(); };

        virtual ~FEdgeLinear();
        FEdgeLinear( FNode* InNode0
                   , FNode* InNode1
                   , FFraction* InFractions );

        virtual FVector GetOriginalWorlPosition( float T ) override;
    };

    struct ARIANE_API FEdgeCubic : FEdge
    {
        static uint32 StaticClass(){ return 0x7893373f; }; // crc32 FArianeGraph::FEdgeCubic
        virtual uint32 GetClass() override { return StaticClass(); };

        virtual ~FEdgeCubic();
        FEdgeCubic( FNode* InNode0
                  , const FVector2D& Handle0Position
                  , const FVector& OriginalHandle0Position
                  , const FVector2D& Handle1Position
                  , const FVector& OriginalHandle1Position
                  , FNode* InNode1
                  , uint32 InFractionCount
                  , FFraction* InFractions );

        virtual FVector GetOriginalWorlPosition( float T ) override;

        FVector2D Bezier[4];
        FVector OriginalHandlePosition[2];
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

    struct ARIANE_API FSection
    {
        static uint32 StaticClass(){ return 0xa1735422; }; // crc32 FArianeGraph::FSection
        virtual uint32 GetClass() = 0;

        virtual ~FSection();

       /**
         * @brief constructor
         * @param InEdge the segment it belongs to
         * @param InNode0 end point 0
         * @param InNode1 end point 1
         */
        FSection( FEdge* InEdge
                , FNode* InNode0
                , FNode* InNode1
                , double InEdgeT0
                , double InEdgeT1 );
       /**
         * @brief Get the node at the other end.
         * @param Node node at this end.
         * return a pointer to the node at the other end.
         */
        FNode* GetOtherNode( FNode* Node );

       /**
         * @brief Block the section for traversal from the node index passed as parameter.
         * @param NodeIndex
         */
        void UnBlock( uint32 NodeIndex );

       /**
         * @brief Unblock the section for traversal from the node index passed as parameter.
         * @param NodeIndex
         */
        void Block( uint32 NodeIndex );

       /**
         * @brief Check the blocking status of this section from the node passed as parameter.
         * @param NodeIndex
         * @return true or false
         */
        bool IsBlocked( uint32 NodeIndex );

        FVector2D GetVector( uint32 NodeIndex );

        virtual FVector2D GetPointAt( double t ) = 0;
        virtual FVector2D GetTangentAt( double t, bool iNormalize ) = 0;

        bool IsLinked();
        void Link();
        void Unlink( bool bRestore  );
        FCycle* GetCycle( uint32 CycleID );
        void AddCycle( FCycle* Cycle );
        FCycle* GetOtherCycle( FCycle* iCycle );
        bool HasCycle( FCycle* iCycle );
        void SetErased( bool iErased );
        bool IsErased();
        double GetLength();
        bool IsValid();
        double GetT( uint32 Index );
        void Merge( uint32 PartnerID );
        void Stitch();
        FEdge* GetEdge();
        double GetEdgeT( uint32 Index );

       /**
         * @brief Get a vector tangent to this section, starting at this node.
         * @param NodeIndex index the node (0 or 1)
         * @param bStraight
         * @param bNormalize normalize the vector or not
         * return a vector tangent to this section, starting at this node.
         */
        //virtual FVector2D GetVectorFromNode( uint32 NodeIndex, bool bStraight, bool bNormalize ) = 0;

        double GetSegmentT( uint32 iIndex );
        void Print();
        void LinkWithoutStitching();
        void UnlinkWithoutStitching();
        //bool IsGap();

    //protected:
        FEdge* Edge;
        FNode* Nodes[2];
        // These are the nodes before stitching.
        // Used by the eraser tool in "section mode"
        FNode* OriginalNodes[2];
        uint32 Flags;
        uint32 CycleCount;
        FCycle* Cycles[2]; // there are 2 cycles per section at most. No need for a complicated container.
        double Length;
        double EdgeT[2];
        // vectors at endpoint;
        FVector2D Vector[2];

    //private:
        static const uint32 BLOCKNODE0 = ( 1 << 0 );
        static const uint32 BLOCKNODE1 = ( 1 << 1 );
        static const uint32 LINKED     = ( 1 << 2 );
        static const uint32 ERASED     = ( 1 << 3 );
        static const uint32 GAP        = ( 1 << 4 );
    };

    struct ARIANE_API FSectionLinear : public FSection
    {
        static uint32 StaticClass(){ return 0x2cb54896; }; // crc32 FArianeGraph::FSectionLinear
        virtual uint32 GetClass() override { return StaticClass(); };

        virtual ~FSectionLinear();
        FSectionLinear( FEdgeLinear* InLinearEdge
                      , FNode* InNode0
                      , FNode* InNode1
                      , double InEdgeT0
                      , double InEdgeT1
                      , bool bStitchShortSections
                      , TArray<FSection*>& OutShortSections );

        //virtual FVector2D GetVectorFromNode( uint32 NodeIndex, bool bStraight, bool bNormalize ) override;
        virtual FVector2D GetPointAt( double t ) override;
        virtual FVector2D GetTangentAt( double t, bool iNormalize ) override;
    };

    struct ARIANE_API FSectionCubic : public FSection
    {
        static uint32 StaticClass(){ return 0xb082c558; }; // crc32 FArianeGraph::FSectionCubic
        virtual uint32 GetClass() override { return StaticClass(); };

        virtual ~FSectionCubic();
        FSectionCubic( FEdgeCubic* InCubicEdge
                     , FNode* InNode0
                     , FNode* InNode1
                     , double InEdgeT0
                     , double InEdgeT1
                     , bool bStitchShortSections
                     , TArray<FSection*>& OutShortSections );

        //virtual FVector2D GetVectorFromNode( uint32 NodeIndex, bool bStraight, bool bNormalize ) override;
        virtual FVector2D GetPointAt( double t ) override;
        virtual FVector2D GetTangentAt( double t, bool iNormalize ) override;

        FVector2D* GetBezier();
        FVector2D Bezier[4];
    };



/* Later
    struct FSectionCubic : public FSectionCubic
    {
    };
*/

    struct ARIANE_API FPath
    {
        virtual ~FPath();
        FPath( uint32 InNodeCount
             , FNode* InNodes
             , uint32 InLinearEdgeCount
             , uint32 InCubicEdgeCount
             , FEdge** InEdges );

        void ToNodeIndicesAndSections( TArray<uint32>& OutNodeIndices
                                     , TArray<FSection*>& OutSections );
        bool IsLoop();
        uint32 GetEdgeCount();

        bool bHasIntersections;
        uint32 NodeCount;
        FNode* Nodes; // we dont use TArray, for performance reasons
        uint32 LinearEdgeCount;
        uint32 CubicEdgeCount;
        FEdge** Edges; // we dont use TArray, for performance reasons
    };

    struct ARIANE_API FCycle
    {
        /**
         * @brief build the cycle from vertices/sections passed as parameter. Sections can belong to different paths.
         * @param iVertexArray
         * @param iSectionArray drawing flags.
         */
        void Build();

        //void ToBucketArray( std::vector<FCycle*>& iCyleArray
        //                  , std::vector<FOdysseyVectorBucket*>& oBucketArray );

        /**
         * @brief destructor.
         */
        virtual ~FCycle();

        /**
         * @brief constructor.
         * @param NodeIndices the node array as indexes (0 or 1).
         * @param Sections the section array.
         */
         FCycle( const TArray<uint32>& NodeIndices
               , const TArray<FSection*>& Sections );

        /**
         * @brief Test whether or not this cycle fits entirely within the cycle passed as parameter.
         * @param iParentCandidate the candidate parent cycle.
         */
        bool FitsIn( FCycle* ParentCandidate );

        /**
         * @brief Gets this cycle's bounding box
         * @return the bounding box as a rectangle
         */
        //::ULIS::FRectD GetBBox( bool iWorld );

        /**
         * @brief Get the attached bucket, if any.
         * @return the attached bucket or nullptr if none.
         */
        //FOdysseyVectorBucket* GetBucket();

        /**
         * @brief Get this cycle's parent cycle.
         * @return this cycle's parent cycle.
         */
        FCycle* GetParentCycle();

        /**
         * @brief Gets the bucket that should color this cycle without being its official bucket.
         * @return a pointer to the propagated bucket
         */
        //FOdysseyVectorBucket* GetPropagatedBucket();

        /**
         * @brief collision test with coordinates passed as parameters.
         * @param iX local coordinate on X-Axis.
         * @param iY local coordinate on Y-Axis.
         */
        bool HitTest( double iLocalX, double iLocalY );

        /**
         * @brief Merge this cycle's contour path with the contour path of the cycle passed as parameter
         * thus creating a combined path.
         * @param iMergeCycle the cycle that will be merged.
         */
        void Merge( FCycle* MergeCycle );

        /**
         * @brief Gets a propagated bucket from any neighbour cycle
         * @return true if it got any, false otherwise
         */
        //void PropagateBucket( std::vector<FOdysseyVectorCycle*>& oNextCycleArray );

        /**
         * @brief Attach a bucket. Can be nullptr.
         * @param iBucket the bucket to attach.
         */
        //void SetBucket( FOdysseyVectorBucket* iBucket );

        /**
         * @brief Set this cycle's parent cycle (the cycle that this one fits in).
         * @param iParent a pointer to this cycle's parent cycle.
         */
        void SetParentCycle( FCycle *InParentCycle );

        /**
         * @brief Sets a bucket that should color this cycle without being its official bucket.
         * @param iPropagatedBucket the propagated bucket
         */
        //void SetPropagatedBucket( FOdysseyVectorBucket* iPropagatedBucket );

        /**
         * @brief Stroke the path using BLend2D API. The context (path width, color) can be set before calling this method.
         * @param iWorld true if it should be drawn in world coordinates, false otherwise
         */
        //void StrokePath( BLContext* iBLContext, bool iWorld );

    private :
        void PropagateBucket( TArray<FSection*> iSectionArray
                            , TArray<FCycle*>& oNextCycleArray );

        bool HasNode( FNode* Node );

    public :
        BLPath ContourPath;
        BLPath CombinedPath;
        //FOdysseyVectorBucket* mBucket;
        //FOdysseyVectorBucket* mPropagatedBucket;
        TArray<uint32> ContourNodeIndices;
        TArray<FSection*> ContourSections;
        TArray<FSection*> InnerSections;
        TArray<FCycle*> Children;
        FCycle* ParentCycle;
        FBox2D BBox;
        //bool mPropagated;
    };

public:
    virtual ~FArianeGraph();
    FArianeGraph();

    void Solve( const FVector& ViewOrigin, const FPlane& ProjectionPlane, const TArray<FArianeObject*>& Objects, double GapTolerance );
    //Build( TArray<FOdysseyVectorObject*> Objects ); // Later
    FCycle* PickCycle( const FVector& RayOrigin, const FVector& RayDirection );
    void HighlightCycle( FCycle* Cycle );
    const FPlane& GetProjectionPlane();
    FVector GetNodeWorldPositionOnPlane( FNode* Node );

protected:
    static FSectionLinkInfo* FindNextSectionLinkInfo( FSectionLinkInfo* LastSectionLinkInfo
                                                    , TArray<FSectionLinkInfo>& CandidateSections
                                                    , double Orientation );
    static double GetCycleNormalVector( TArray<uint32>& NodeIndexArray, TArray<FSection*>& SectionArray );

    void Import( const FVector& ViewOrigin, const TArray<FArianeObject*>& Objects );
    void Build();
    void SimplifyGraph();
    void Explore( FExplorationPair* ExplorationPair );
    uint32 FindPath( FSection* ReturnSection
                   , uint32 SectionNodeIndex
                   , FSection* Section
                   , TArray<uint32>& OutNodeIndexArray
                   , TArray<FSection*>& OutSectionArray
                   , double Orientation
                   , uint32 Depth );
    void Intersect( TArray<FPath*>& SectionnablePaths
                  , uint32& OutTotalLinearSectionCount
                  , uint32& OutTotalCubicSectionCount );
    void IntersectEdgeWithPath( FPath* Path, FEdge* Edge, FPath* IntersectedPath );
    void IntersectEdges( FPath* Path0
                       , FEdge* Edge0
                       , FPath* Path1
                       , FEdge* Edge1
                       , const FVector2D& Edge1MinWithTolerance
                       , const FVector2D& Edge1MaxWithTolerance
                       , TArray<FNodeIntersection::XRecord>& OutIntersectionRecordArray );
    void IntersectPath( FPath* Path );
    void CreatePathSections( FPath* Path, TArray<FSection*>& ShortSections );
    void CreateEdgeSections( FEdge* Edge, TArray<FSection*>& ShortSections );
    void CreateEdgeSection( FEdge* Edge
                          , FNode* Node0
                          , FNode* Node1
                          , double SectionNode0EdgeT
                          , double SectionNode1EdgeT
                          , TArray<FSection*>& ShortSections );
    void MergeCycles();
    void OrderCycles();
    static void UnlinkPendantSectionsRecursively( FSection* Section, FNode* Node );

protected:
    static const uint32 NOCYCLE  = 0;
    static const uint32 BLOCKED  = 1;
    static const uint32 HASCYCLE = 2;

    FPlane ProjectionPlane;
    FVector ProjectionPlaneOrigin;
    FQuat LocalToWorldRotationQuat;
    FQuat WorldToLocalRotationQuat;
    FCriticalSection Mutex;
    // we store everything in one buffer per type, for performance
    TArray<FPoint> PointBuffer;
    TArray<FNode> NodeBuffer;
    TArray<FFraction> FractionBuffer;
    TArray<FEdgeLinear> LinearEdgeBuffer;
    TArray<FEdgeCubic> CubicEdgeBuffer;
    TArray<FEdge*> Edges;
    TArray<FSectionLinear> LinearSectionBuffer;
    TArray<FSectionCubic> CubicSectionBuffer;
    TArray<FSection*> Sections;
    TArray<FNodeIntersection> IntersectionNodeBuffer;
    TArray<FPath> PathBuffer;
    TArray<FNodeIntersection::XRecord> XIntersectionRecordArray;
    //TArray<FEdgeCubic> CubicSegmentBuffer;
    TArray<FCycle*> Cycles;
    bool bMultithreaded;
    double GapTolerance;
};
