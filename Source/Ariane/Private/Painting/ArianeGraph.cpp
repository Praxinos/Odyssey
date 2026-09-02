// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeGraph.h"
#include "ArianeCore.h"
#include "ArianeObject.h"
#include "ArianePath.h"
#include "ArianeVertex.h"
#include "ArianeSegment.h"

// Some explanations are needed here, as this is by far the most complex process
// of Odyssey's vector features. The principles is to find cycles chordless determined by
// the intersected paths. To do so we have a multi-step process :
// - find intersections
// - Build a graph by creating sections
// - find chordless cycles by exploring sections :
//    -> To find chordless cycles, we have to always go the same way, either
//       always left or always right, it does not matter but we always go the same way.
//       This is the basic principle of the method. To find the correct way, we simply compute
//       the cross product, which will be either positive or negative relative to the direction.
//       The only case when we take the negative-direction is when there is no positive-direction.
//
//         Let's say we only take the first section going to the right :
//
//             /              Sections B and A are both at the right side of section S
//            /               This can be determined by computing the cross products SxA and SxB
//           B                However, the closest section to segment S is section A. This can be
//          /                 Determined by the dot product. The biggest dot product wins (S.A).
//         /_____ A______
//         o
//         |
//         |
//         S
//         |
//         |
//
//          But what if there is no section going the right way ?
//         Then we choose the section with the biggest dot product
//
//                 B
//                  \         Here, the smallest dot product wins (S.B).
//      _____ A______\
//                   o
//                   |
//                   |
//                   S
//                   |
//                   |
//
// See functions :
//    FArianeGraph::FNode::GetCycleNextSection()
//
// Using these principles, we are guaranteed to always turn in the right direction.
// Then, to tell whether or not we have found a cycle, we just check that the last node
// we met is the same as the first node we explored the graph from.
//
// Another important technique is blocking the sections once they were explored in one way.
// Indeed, a set of connected cycles always face the same direction because their vertices "turn"
// in the same direction. It is the same as face orientation on a 3D-Mesh.
//
//      o______________o______________o
//      |   ------->   |   ------->   |     What do we notice here ? Although cycles have the same
//      |  ^        |  |  ^        |  |     orientation (cross product facing in the same direction),
//      |  |        |  A  |        |  |     sections A, B, C, D are never "explored" twice in the same
//      |  |        v  |  |        v  |     direction (look at the arrows above and below). We use
//      |   <-------   |   <-------   |     this as an advantage to prevent double detection of the
//      o_______C______o_______D______o     same cycle, which will speed up things. Each time a cycle
//      |   ------->   |   ------->   |     is detected, its sections are blocked one-way, guaranteing
//      |  ^        |  |  ^        |  |     that there will be no other detection. By and by, the whole
//      |  |        |  B  |        |  |     graph exploration simplifies itself.
//      |  |        v  |  |        v  |
//      |   <-------   |   <-------   |
//      o______________o______________o
//
// See functions:
//     FArianeGraph::FSection::Block()
//
//                  Some requirements :
//
//         the algorithm had to work even in this case:
//          ______________________________
//         |                              |
//         |           _______            |
//         |          |       |           |
//         |           \     /            |
//         |            \ o /             |
//         \             / \              /
//          \___________/   \____________/
//
//          Here there are 2 chordless cycles :
//
//                      Cycle 1
//          _______________________________
//         |...............................|
//         |............_______............|
//         |...........|       |...........|
//         |............\     /............|
//         |.............\ o /.............|
//         \............../ \............../
//          \____________/   \____________/
//
//                      Cycle 2
//                      _______
//                     |.......|
//                      \...../
//                       \ o /
//
//  But a naive approach could first detect this cycle below, because
// at intersection point o, there would indeed be a loop detection.
//          _______________________________
//         |...............................|
//         |...............................|
//         |...............................|
//         |...............................|
//         |...............o...............|
//         \............../ \............../
//          \____________/   \____________/
//
// What is the solution ? Detecting a loop only by comparing its
// initial and final vertices is not enough, we also have to check if the
// recursive exploration process has ended on a section that is allowed.
// This is why we introduced the concept of exploration pairs. Exploration pairs
// consists in a "depart section", a node, and an "return section". So, for each
// intersection point, we first determine as many exploration pairs as sections connected
// to this node. At intersection point o, we would have 4 exploration pairs :
//
//                        1.      2.        3.          4.
//                      \                      /     \     /
//                       \ o       o        o /       \ o /
//                        /       / \        \
//                       /       /   \        \
//
// See functions:
//     FArianeGraph::FNode::BuildExplorationPairs()
//
// A cycle is detected only if the last section matches the allowed return section
// OR, if the allowed return section was removed from the graph, then any return
// section is allowed. Indeed, there are orphaned section (that leads to nowhere),
// and we get rid of them because the would cause problems. E.g :
//              o
//              |
//              |
//       o______o______o
//              |
//              |
//              o
//
// Here there is no cycle. But if we explore those sections, we would find one, and it would look
// like this cross. So we get rid of these kind of sections. How ? Simply get rid of any section
// whose vertices are not connected to another section. This implies that this is a multi-pass
// process, as we can have such cases :
//
//              o
//              |       <--- deletion at first pass
//              |
//              o
//              |       <--- deletion at second pass
//              |
//       o______o______o
//       |      |      |
//       |      |      |      <--- keep those sections.
//       o____  o______o
//
// See functions:
//        FArianeGraph::SimplifyGraph()
//
// When a cycle is detected, it is not guaranteed that it will be correctly oriented. Indeed,
// a contour can be detected as a cycle, we always took the best section possible, but the
//  overall cycle isn't correctly oriented :
//
//      <-----------------------------
//  |                                      ^
//  |    o______________o______________o   |
//  |    |   ------->   |   ------->   |   |
//  |    |  ^        |  |  ^        |  |   |
//  |    |  |        |  A  |        |  |   |
//  |    |  |        v  |  |        v  |   |
//  |    |   <-------   |   <-------   |   |
//  |    o_______C______o_______D______o   |
//  |    |   ------->   |   ------->   |   |
//  |    |  ^        |  |  ^        |  |   |
//  |    |  |        |  B  |        |  |   |
//  |    |  |        v  |  |        v  |   |
//  |    |   <-------   |   <-------   |   |
//  |    o______________o______________o   |
//  v
//      ------------------------------>
//
// The contour cycle detected was the only possibility, but in the end
// is not well oriented. That's why, for any cycle we find, we always
// have to check its overall orientation anyways. This one will be discarded.
//
// See functions:
//    static GetCycleNormalVector()
//
// Stay focused, it's not over yet !
//
// Once we have detected the cycles, how to deal with cycles that are the one inside the other ?
// we have to check which one fits into which one and then merge them :
//
// See functions:
//     FArianeGraph::OrderCycles()
//     FArianeGraph::MergeCycles()
//
//  That's basically it !

//static
FArianeGraph::FSectionLinkInfo*
FArianeGraph::FindNextSectionLinkInfo( FSectionLinkInfo* LastSectionLinkInfo
                                     , TArray<FSectionLinkInfo>& CandidateSections
                                     , double Orientation )
{
    FSectionLinkInfo* RightRet = nullptr;
    FSectionLinkInfo* WrongRet = nullptr;

    if( CandidateSections.Num() )
    {
        FVector2D LastSectionVector = LastSectionLinkInfo->GetVector();
        double MinDot =  DBL_MAX;
        double MaxDot = -DBL_MAX;
        TArray<FSectionLinkInfo*> RightSideSections;
        TArray<FSectionLinkInfo*> WrongSideSections;
        uint32 SectionCount = CandidateSections.Num();

        RightSideSections.Reserve( SectionCount );
        WrongSideSections.Reserve( SectionCount );

        for( FSectionLinkInfo& CandidateSectionInfo : CandidateSections )
        {
            if( &CandidateSectionInfo != LastSectionLinkInfo )
            {
                if( CandidateSectionInfo.GetVector().SquaredLength() )
                {
                    double CrossProduct = FVector2D::CrossProduct( -LastSectionVector
                                                                  , CandidateSectionInfo.GetVector() );

                    if( ( CrossProduct * Orientation >= 0.0f ) )
                    {
                        RightSideSections.Add( &CandidateSectionInfo );
                    }

                    if( ( CrossProduct * Orientation <= 0.0f ) )
                    {
                        WrongSideSections.Add( &CandidateSectionInfo );
                    }
                }
            }
        }

        for( FSectionLinkInfo* RightSideSectionLinkInfo : RightSideSections )
        {
            FSection* Section = RightSideSectionLinkInfo->Section;
            FVector2D SectionVector = RightSideSectionLinkInfo->GetVector();
            double Dot = LastSectionVector.Dot( SectionVector );

            if( Dot > MaxDot )
            {
                RightRet = RightSideSectionLinkInfo;

                MaxDot = Dot;
            }
        }

        if( RightRet )
        {
            return RightRet;
        }

        for( FSectionLinkInfo* WrongSideSectionLinkInfo : WrongSideSections )
        {
            FSection* Section = WrongSideSectionLinkInfo->Section;
            FVector2D SectionVector = WrongSideSectionLinkInfo->GetVector();
            double Dot = LastSectionVector.Dot( SectionVector );

            if( Dot < MinDot )
            {
                WrongRet = WrongSideSectionLinkInfo;

                MinDot = Dot;
            }
        }

        if( WrongRet )
        {
            return WrongRet;
        }
    }

    return nullptr;
}

// find the overall orientation of the future cycle
//static
double
FArianeGraph::GetCycleNormalVector( TArray<uint32>& NodeIndexArray, TArray<FSection*>& SectionArray )
{
    double Z = 0;
    int32 ArraySize = SectionArray.Num();

    for( int i = 0; i < ArraySize; i++ )
    {
        int n = ( i + 1 ) % ArraySize;
        FSection* Sectioni = SectionArray[i];
        uint32 NodeIndex = NodeIndexArray[i];
        FNode* Node = Sectioni->Nodes[NodeIndexArray[i]];
        uint32 NextNodeIndex = ( NodeIndex == 0 ) ? 1 : 0;
        FNode* NextNode = Sectioni->Nodes[NextNodeIndex];

        FVector2D& NodeCoords = Node->Position;
        FVector2D& NextNodeCoords = NextNode->Position;

        if( ( Sectioni->GetClass() == FSectionLinear::StaticClass() ) )
        {
            Z += ( ( NodeCoords.X - NextNodeCoords.X ) * ( NodeCoords.Y + NextNodeCoords.Y ) );
        }

        if( Sectioni->GetClass() == FSectionCubic::StaticClass() )
        {
            double DeltaT = (double)NextNodeIndex - (double)NodeIndex;
            int Subdiv = 8;
            double StepT = DeltaT / Subdiv;
            double T0 = NodeIndex;

            // By relying only on start and end points of a section, we lack precision.
            // Here we rely on more acurate computation by getting intermediate points.
            for( int j = 0; j < Subdiv; j++ )
            {
                double T1 = T0 + StepT;
                // however at end points, we need the same coordinates for each section. Relying on T value does not guarantee that
                // due to imprecision and would fake the calculation. So, we use the value stored in viCoords and vnCoords.
                FVector2D P0Coords = ( j == 0          ) ? NodeCoords     : Sectioni->GetPointAt( T0 );
                FVector2D P1Coords = ( j == Subdiv - 1 ) ? NextNodeCoords : Sectioni->GetPointAt( T1 );

                Z += ( ( P0Coords.X - P1Coords.X ) * ( P0Coords.Y + P1Coords.Y ) );

                T0 += StepT;
            }
        }


// https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal
// Newell's method
        //z += ( ( viCoords.x - vnCoords.x ) * ( viCoords.y + vnCoords.y ) );
    }

    return Z;
}

FArianeGraph::FExplorationPair::~FExplorationPair()
{
}

FArianeGraph::FExplorationPair::FExplorationPair()
    : ReturnSection ( nullptr )
    , DepartNode ( nullptr )
    , DepartNodeIndex ( 0 )
    , DepartSection ( nullptr )
    , SectionLength ( 0.0f )
{
}

FArianeGraph::FExplorationPair::FExplorationPair( FSection* InReturnSection
                                                , FNode* InDepartNode
                                                , uint32 InDepartNodeIndex
                                                , FSection* InDepartSection )
    : ReturnSection ( InReturnSection )
    , DepartNode ( InDepartNode )
    , DepartNodeIndex ( InDepartNodeIndex )
    , DepartSection ( InDepartSection )
    // SectionLength is used for sorting exploration pairs
    , SectionLength ( ReturnSection->GetLength() + DepartSection->GetLength() )
{
}

FVector2D
FArianeGraph::FSectionLinkInfo::GetVector()
{
    return Section->GetVector( SectionNodeIndex );
}

FArianeGraph::FPoint::~FPoint()
{
}

FArianeGraph::FPoint::FPoint( const FVector2D& InPosition, bool bInProjected, const FVector& InOriginalWorlPosition )
    : Position ( InPosition )
    , bProjected ( bInProjected )
    , OriginalWorlPosition ( InOriginalWorlPosition )
{
}


FArianeGraph::FNode::~FNode()
{
}

FArianeGraph::FNode::FNode( const FVector2D& InPosition, bool bInProjected, const FVector& InOriginalWorlPosition )
    : FPoint( InPosition, bInProjected, InOriginalWorlPosition )
    , EdgeCount( 0 )
{
}

void
FArianeGraph::FNode::AddSection( FSection* Section, uint32 SectionNodeIndex )
{
    SectionLinkInfos.Emplace( Section, SectionNodeIndex );
}

FArianeGraph::FSection*
FArianeGraph::FNode::GetSection( FEdge* Edge )
{
    for( FSectionLinkInfo& SectionLinkInfo : SectionLinkInfos )
    {
        if( SectionLinkInfo.Section->GetEdge() == Edge )
        {
            return SectionLinkInfo.Section;
        }
    }

    return nullptr;
}

void
FArianeGraph::FNode::RemoveSection( FSection* Section, uint32 SectionNodeIndex )
{
    SectionLinkInfos.RemoveAll( [ Section
                                , SectionNodeIndex ]( FSectionLinkInfo& SectionLinkInfo )
        {
            return ( ( SectionLinkInfo.Section          == Section            )
                  && ( SectionLinkInfo.SectionNodeIndex == SectionNodeIndex ) );
        } );
}

FArianeGraph::FSectionLinkInfo*
FArianeGraph::FNode::GetSectionLinkInfo( FSection* Section, uint32 SectionNodeIndex )
{
   for( FSectionLinkInfo& SectionLinkInfo : SectionLinkInfos )
    {
        if( ( SectionLinkInfo.Section          == Section            )
         && ( SectionLinkInfo.SectionNodeIndex == SectionNodeIndex ) )
        {
            return &SectionLinkInfo;
        }
    }

    return nullptr;
}

FArianeGraph::FSectionLinkInfo*
FArianeGraph::FNode::GetOtherSectionLinkInfo( FSectionLinkInfo* LastSectionLinkInfo )
{
    for( FSectionLinkInfo& CandidateSectionInfo : SectionLinkInfos )
    {
        if( &CandidateSectionInfo != LastSectionLinkInfo )
        {
            return &CandidateSectionInfo;
        }
    }

    return nullptr;
}

FArianeGraph::FSectionLinkInfo*
FArianeGraph::FNode::GetCycleNextSection( FSectionLinkInfo* LastSectionLinkInfo
                                        , double Orientation )
{
    if( SectionLinkInfos.Num() == 2 )
    {
        return GetOtherSectionLinkInfo( LastSectionLinkInfo );
    }

    return FindNextSectionLinkInfo( LastSectionLinkInfo
                                  , SectionLinkInfos
                                  , Orientation );
}

FArianeGraph::FEdge*
FArianeGraph::FNode::GetOtherEdge( FEdge* InEdge )
{
    for( uint32 i = 0; i < EdgeCount; i++ )
    {
        if( Edges[i] != InEdge )
        {
            return Edges[i];
        }
    }

    return nullptr;
}

int32
FArianeGraph::FNode::GetIndex( FEdge* Edge )
{
    if( this == Edge->Nodes[0] )
    {
        return 0;
    }

    if( this == Edge->Nodes[1] )
    {
        return 1;
    }

    return -1;
}

void
FArianeGraph::FNode::BuildExplorationPairs( TArray<FExplorationPair>& OutExplorationPairsArray )
{
    FSectionLinkInfo* DepartSectionLinkInfo = nullptr;

    for( FSectionLinkInfo& ReturnSectionLinkInfo : SectionLinkInfos )
    {
        DepartSectionLinkInfo = FindNextSectionLinkInfo( &ReturnSectionLinkInfo
                                                       , SectionLinkInfos
                                                       , 1.0f );

        if( DepartSectionLinkInfo )
        {
            OutExplorationPairsArray.Emplace(  ReturnSectionLinkInfo.Section
                                             , this
                                             , DepartSectionLinkInfo->SectionNodeIndex
                                             , DepartSectionLinkInfo->Section );
        }
    }
}

void
FArianeGraph::FNode::UnlinkPendantSections()
{
    FNode* CurrentNode = this;

    while( CurrentNode->SectionLinkInfos.Num() == 1 )
    {
        FSection* PendantSection = CurrentNode->SectionLinkInfos[0].Section;

        PendantSection->Unlink( false );

        CurrentNode = PendantSection->GetOtherNode( CurrentNode );
    }
}

FArianeGraph::FIntersection::~FIntersection()
{
}

FArianeGraph::FIntersection::FIntersection( FNode* InNode, float InEdgeT )
    : Node ( InNode )
    , EdgeT ( InEdgeT )
{
}

FArianeGraph::FNodeIntersection::~FNodeIntersection()
{
}

FArianeGraph::FNodeIntersection::FNodeIntersection( const FVector2D& InPosition
                                                  , FEdge* InEdge0
                                                  , double InEdge0T
                                                  , FEdge* InEdge1
                                                  , double InEdge1T )
    : FNode ( InPosition, true, InEdge0->GetOriginalWorlPosition( InEdge0T ) )
    , Intersections { FIntersection( this, InEdge0T )
                    , FIntersection( this, InEdge1T ) }
    , Edges { InEdge0,  InEdge1 }
{
    // attach to edges (ordered, depends on T value)
    Edges[0]->AddIntersection( &Intersections[0] );
    Edges[1]->AddIntersection( &Intersections[1] );
}

bool
FArianeGraph::FNodeIntersection::SelfIntersects()
{
    return Edges[0] == Edges[1];
}

FArianeGraph::FNodeIntersection::XRecord::~XRecord()
{
}

FArianeGraph::FNodeIntersection::XRecord::XRecord( const FVector2D& InPosition
                                                 , FPath* InPath0
                                                 , FEdge* InEdge0
                                                 , double InEdge0T
                                                 , FPath* InPath1
                                                 , FEdge* InEdge1
                                                 , double InEdge1T )
    : Edge0 ( InEdge0 )
    , Edge1 ( InEdge1 )
    , Edge0T ( InEdge0T )
    , Edge1T ( InEdge1T )
    , Position ( InPosition )
{
    Edge0->IntersectionSlotCount++;
    Edge1->IntersectionSlotCount++;

    InPath0->bHasIntersections = true;
    InPath1->bHasIntersections = true;
}

FArianeGraph::FFraction::~FFraction()
{
}

FArianeGraph::FFraction::FFraction( double InFromT, double InToT, FPoint* InPoint0, FPoint* InPoint1 )
    : FromT ( InFromT )
    , ToT ( InToT )
    , Points { InPoint0, InPoint1 }
{
    BBox.Min.X = FMath::Min( Points[0]->Position.X, Points[1]->Position.X );
    BBox.Min.Y = FMath::Min( Points[0]->Position.Y, Points[1]->Position.Y );
    BBox.Max.X = FMath::Max( Points[0]->Position.X, Points[1]->Position.X );
    BBox.Max.Y = FMath::Max( Points[0]->Position.Y, Points[1]->Position.Y );
}

FArianeGraph::FEdge::~FEdge()
{
}

FArianeGraph::FEdge::FEdge( FNode* InNode0
                          , FNode* InNode1
                          , uint32 InFractionCount
                          , FFraction* InFractions
                          , double InLength )
    : Nodes { InNode0, InNode1 }
    , Fractions( InFractions )
    , FractionCount ( InFractionCount )
    , IntersectionSlotCount ( 0 )
    , Length ( InLength )
{
    Nodes[0]->Edges[Nodes[0]->EdgeCount++] = this;
    Nodes[1]->Edges[Nodes[1]->EdgeCount++] = this;
}

void
FArianeGraph::FEdge::AddIntersection ( FIntersection* InIntersection )
{
    int32 Index = Intersections.IndexOfByPredicate( [ this
                                                    , InIntersection ]( FIntersection* Intersection )
                                                    {
                                                        return ( Intersection->EdgeT > InIntersection->EdgeT );
                                                    } );

    if( Index == INDEX_NONE )
    {
        Intersections.Add( InIntersection );
    }
    else
    {
        Intersections.Insert( InIntersection, Index );
    }
}

FArianeGraph::FNode*
FArianeGraph::FEdge::GetOtherNode( FNode* Node )
{
    if( Nodes[0] == Node )
    {
        return Nodes[1];
    }

    if( Nodes[1] == Node )
    {
        return Nodes[0];
    }

    return nullptr;
}

FArianeGraph::FEdgeLinear::~FEdgeLinear()
{
}

FArianeGraph::FEdgeLinear::FEdgeLinear( FNode* InNode0
                                      , FNode* InNode1
                                      , FFraction* InFractions )
    : FEdge( InNode0, InNode1, 1, InFractions, ( InNode1->Position - InNode0->Position ).Length() )
{
    BBox.Min.X = FMath::Min( Nodes[0]->Position.X, Nodes[1]->Position.X );
    BBox.Min.Y = FMath::Min( Nodes[0]->Position.Y, Nodes[1]->Position.Y );
    BBox.Max.X = FMath::Max( Nodes[0]->Position.X, Nodes[1]->Position.X );
    BBox.Max.Y = FMath::Max( Nodes[0]->Position.Y, Nodes[1]->Position.Y );
}

FVector
FArianeGraph::FEdgeLinear::GetOriginalWorlPosition( float T )
{
    return Nodes[0]->OriginalWorlPosition + ( T * ( Nodes[1]->OriginalWorlPosition
                                              - Nodes[0]->OriginalWorlPosition ) );
}

FArianeGraph::FEdgeCubic::~FEdgeCubic()
{
}

FArianeGraph::FEdgeCubic::FEdgeCubic( FNode* InNode0
                                    , const FVector2D& Handle0Position
                                    , const FVector& OriginalHandle0Position
                                    , const FVector2D& Handle1Position
                                    , const FVector& OriginalHandle1Position
                                    , FNode* InNode1
                                    , uint32 InFractionCount
                                    , FFraction* InFractions )
    : FEdge( InNode0, InNode1, InFractionCount, InFractions, 0.0f )
{
    Bezier[0] = InNode0->Position;
    Bezier[1] = Handle0Position;
    Bezier[2] = Handle1Position;
    Bezier[3] = InNode1->Position;

    Length = FArianeCore::GetCubicBezierApproximateLength( Bezier, 8 );

    BBox.Min.X = FMath::Min( Bezier[0].X, FMath::Min3 ( Bezier[1].X, Bezier[2].X, Bezier[3].X ) );
    BBox.Min.Y = FMath::Min( Bezier[0].Y, FMath::Min3 ( Bezier[1].Y, Bezier[2].Y, Bezier[3].Y ) );
    BBox.Max.X = FMath::Max( Bezier[0].X, FMath::Max3 ( Bezier[1].X, Bezier[2].X, Bezier[3].X ) );
    BBox.Max.Y = FMath::Max( Bezier[0].Y, FMath::Max3 ( Bezier[1].Y, Bezier[2].Y, Bezier[3].Y ) );
}

FVector
FArianeGraph::FEdgeCubic::GetOriginalWorlPosition( float T )
{
    return ::ULIS::CubicBezierPointAtParameter<FVector>( Nodes[0]->OriginalWorlPosition
                                                       , OriginalHandlePosition[0]
                                                       , OriginalHandlePosition[1]
                                                       , Nodes[1]->OriginalWorlPosition
                                                       , T );
}

FArianeGraph::FPath::~FPath()
{
}

FArianeGraph::FPath::FPath( uint32 InNodeCount
                          , FNode* InNodes
                          , uint32 InLinearEdgeCount
                          , uint32 InCubicEdgeCount
                          , FEdge** InEdges )
    : bHasIntersections ( false )
    , NodeCount( InNodeCount )
    , Nodes( InNodes )
    , LinearEdgeCount( InLinearEdgeCount )
    , CubicEdgeCount( InCubicEdgeCount )
    , Edges( InEdges )
{
}

uint32
FArianeGraph::FPath::GetEdgeCount()
{
    return ( LinearEdgeCount + CubicEdgeCount );
}

bool
FArianeGraph::FPath::IsLoop()
{
    return ( GetEdgeCount() == NodeCount );
}

void
FArianeGraph::FPath::ToNodeIndicesAndSections( TArray<uint32>& OutNodeIndices
                                             , TArray<FSection*>& OutSections )
{
    if( GetEdgeCount() )
    {
        FEdge *Edge = Edges[0];
        FNode* FirstNode = Edge->Nodes[0];
        FNode* Node = FirstNode;

        do
        {
            FNode* NextNode = Edge->GetOtherNode( Node );
            FSection* Section = Node->GetSection( Edge );
            FEdge *NextEdge = NextNode->GetOtherEdge( Edge );

            OutNodeIndices.Add( Node->GetIndex( Edge ) );
            OutSections.Add( Section );

            Node = NextNode;
            Edge = NextEdge;
        }
        while( Edge && ( Node != FirstNode ) );
    }
}

FArianeGraph::~FArianeGraph()
{
}

FArianeGraph::FArianeGraph()
    : GapTolerance ( 0.0f )
{
#if PLATFORM_MAC
    bMultithreaded = false;
#else
    bMultithreaded = true;
#endif
}

// Edge-Edge intersection test. The test is performed using straight sub-edges
// as it would be too complicated to do maths using the parametric bezier and I'm not that smart.
// Actual intersections vertices are created in this method.
// A Tolerance value is accepted to test for near-intersections, that will be created later in the
// process.
void
FArianeGraph::IntersectEdges( FPath* Path0
                            , FEdge* Edge0
                            , FPath* Path1
                            , FEdge* Edge1
                            , const FVector2D& Edge1MinWithTolerance
                            , const FVector2D& Edge1MaxWithTolerance
                            , TArray<FNodeIntersection::XRecord>& OutIntersectionRecordArray )
{
    FNode* Edge0Node0 = Edge0->Nodes[0];
    FNode* Edge0Node1 = Edge0->Nodes[1];
    uint32 Edge0Node0EdgeCount = Edge0Node0->EdgeCount;
    uint32 Edge0Node1EdgeCount = Edge0Node1->EdgeCount;

    FNode* Edge1Node0 = Edge1->Nodes[0];
    FNode* Edge1Node1 = Edge1->Nodes[1];
    uint32 Edge1Node0EdgeCount = Edge1Node0->EdgeCount;
    uint32 Edge1Node1EdgeCount = Edge1Node1->EdgeCount;

    for ( uint32 i = 0; i < Edge0->FractionCount; i++ )
    {
        FFraction* Edge0Fraction = &Edge0->Fractions[i];
        FVector2D& Edge0P0Coords = Edge0Fraction->Points[0]->Position;
        FVector2D& Edge0P1Coords = Edge0Fraction->Points[1]->Position;
        FVector2D Edge0FractionVector = ( Edge0P1Coords - Edge0P0Coords );
        int p = i - 1;
        int n = i + 1;

        // do not test twice, hence the pointer comparison
        if( Edge0 >= Edge1 )
        {
            if( ( Edge0Fraction->BBox.Min.X <= Edge1MaxWithTolerance.X )
             && ( Edge0Fraction->BBox.Max.X >= Edge1MinWithTolerance.X )
             && ( Edge0Fraction->BBox.Min.Y <= Edge1MaxWithTolerance.Y )
             && ( Edge0Fraction->BBox.Max.Y >= Edge1MinWithTolerance.Y ) )
            {
                for( uint32 j = 0; j < Edge1->FractionCount; j++ )
                {
                    FFraction* Edge1Fraction = &Edge1->Fractions[j];

                  if(    ( Edge0 >  Edge1 )
                    // test only once in case of self-intersecting segment
                    || ( ( Edge0 == Edge1 ) && ( Edge0Fraction > Edge1Fraction ) ) )
                  {
                    FVector2D& Edge1P0Coords = Edge1Fraction->Points[0]->Position;
                    FVector2D& Edge1P1Coords = Edge1Fraction->Points[1]->Position;
                    double Edge0FractionSubT, Edge1FractionSubT;

                    // to speed things up a bit (actually I've found out that it speeds things up by 2 or by 3)
                    if( ( Edge0Fraction->BBox.Max.X >= Edge1Fraction->BBox.Min.X ) && ( Edge0Fraction->BBox.Min.X <= Edge1Fraction->BBox.Max.X )
                     && ( Edge0Fraction->BBox.Max.Y >= Edge1Fraction->BBox.Min.Y ) && ( Edge0Fraction->BBox.Min.Y <= Edge1Fraction->BBox.Max.Y ) )
                    {
                        FVector2D Edge1FractionVector = ( Edge1P1Coords - Edge1P0Coords );

                        // check this is not the same sub-segment or adjacent sub-segment,
                        // i.e check they don't share a point in common
                        // or else they would always intersect
                        if( ( Edge0Fraction->Points[0] != Edge1Fraction->Points[1] )
                         && ( Edge0Fraction->Points[0] != Edge1Fraction->Points[0] )
                         && ( Edge0Fraction->Points[1] != Edge1Fraction->Points[0] )
                         && ( Edge0Fraction->Points[1] != Edge1Fraction->Points[1] ) )
                        {
                            // Test intersections in PaintGroup's coordinates system (struct member lineVertexInParent).
                            if ( FArianeCore::IntersectSegment ( Edge0P0Coords
                                                               , Edge0P1Coords
                                                               , Edge1P0Coords
                                                               , Edge1P1Coords
                                                               , &Edge0FractionSubT
                                                               , &Edge1FractionSubT ) )
                            {
                                // Find intersections coordinates in respective coordinates systems (struct member lineVertex).
                                // Note: we cannot use segment->GetPointAt() to determine the position
                                // of the intersection because it will not match the intersection that
                                // we detect via linear means. Cubic segments are not linear.
                                FVector2D Edge0ISXCoords = { Edge0P0Coords.X + ( Edge0FractionVector.X * Edge0FractionSubT )
                                                           , Edge0P0Coords.Y + ( Edge0FractionVector.Y * Edge0FractionSubT ) };
                                FVector2D Edge1ISXCoords = { Edge1P0Coords.X + ( Edge1FractionVector.X * Edge1FractionSubT )
                                                           , Edge1P0Coords.Y + ( Edge1FractionVector.Y * Edge1FractionSubT ) };
                                // find value T at intersection. This is coordinates system-independent.
                                double Edge0T = Edge0Fraction->FromT + ( Edge0FractionSubT * ( Edge0Fraction->ToT - Edge0Fraction->FromT ) );
                                double Edge1T = Edge1Fraction->FromT + ( Edge1FractionSubT * ( Edge1Fraction->ToT - Edge1Fraction->FromT ) );

                                if( ( Edge0T >= 0.0f && Edge0T <= 1.0f )
                                 && ( Edge1T >= 0.0f && Edge1T <= 1.0f ) )
                                {
                                    // ignore intersection at end points if there is only
                                    // one segment. The intersection is handled by the gap
                                    // management
                                    {
                                        if( ( Edge0T == 0.0f )
                                         && ( Edge0Node0EdgeCount == 1 ) )
                                        {
                                            continue;
                                        }

                                        if( ( Edge0T == 1.0f )
                                         && ( Edge0Node1EdgeCount == 1 ) )
                                        {
                                            continue;
                                        }

                                        if( ( Edge1T == 0.0f )
                                         && ( Edge1Node0EdgeCount == 1 ) )
                                        {
                                            continue;
                                        }

                                        if( ( Edge1T == 1.0f )
                                         && ( Edge1Node1EdgeCount == 1 ) )
                                        {
                                            continue;
                                        }
                                    }

                                    // save in temporary struct array will allow to alloc vertices in one go.
                                    Mutex.Lock();
                                    OutIntersectionRecordArray.Emplace( Edge0ISXCoords
                                                                      , Path0
                                                                      , Edge0
                                                                      , Edge0T
                                                                      , Path1
                                                                      , Edge1
                                                                      , Edge1T );
                                    Mutex.Unlock();
                                }
                            }
                        }
                    }
                  }
                }
            }
        }
    }
}

void
FArianeGraph::IntersectEdgeWithPath( FPath* Path, FEdge* Edge, FPath* IntersectedPath )
{
    FBox2D& EdgeBBox = Edge->BBox;
    FVector2D EdgeMinWithTolerance( EdgeBBox.Min.X - GapTolerance
                                  , EdgeBBox.Min.Y - GapTolerance );
    FVector2D EdgeMaxWithTolerance( EdgeBBox.Max.X + GapTolerance
                                  , EdgeBBox.Max.Y + GapTolerance );

    for( uint32 i = 0; i < IntersectedPath->GetEdgeCount(); i++ )
    {
        FEdge* IntersectedEdge = IntersectedPath->Edges[i];
        FBox2D& IntersectedEdgeBBox = IntersectedEdge->BBox;
        FVector2D IntersectedEdgeMinWithTolerance( IntersectedEdgeBBox.Min.X - GapTolerance
                                                 , IntersectedEdgeBBox.Min.Y - GapTolerance );
        FVector2D IntersectedEdgeMaxWithTolerance( IntersectedEdgeBBox.Max.X + GapTolerance
                                                 , IntersectedEdgeBBox.Max.Y + GapTolerance );

        if( ( EdgeMinWithTolerance.X <= IntersectedEdgeMaxWithTolerance.X )
         && ( EdgeMaxWithTolerance.X >= IntersectedEdgeMinWithTolerance.X )
         && ( EdgeMinWithTolerance.Y <= IntersectedEdgeMaxWithTolerance.Y )
         && ( EdgeMaxWithTolerance.Y >= IntersectedEdgeMinWithTolerance.Y ) )
        {
            IntersectEdges( Path
                          , Edge
                          , IntersectedPath
                          , IntersectedEdge
                          , IntersectedEdgeMinWithTolerance
                          , IntersectedEdgeMaxWithTolerance
                          , XIntersectionRecordArray );
        }
    }

/* this version is 4 times slower than the above !!!
    uint32 intersectionCount = 0;

    for( FOdysseyVectorSegment *intersectSegment : iSegmenList )
    {
        //if( intersectSegment->GetPaintingCode() != mPaintingCode )
        {
            ::ULIS::FRectD intersectRect = intersectSegment->GetBBoxInParent() & iSegment->GetBBoxInParent();

            if( intersectRect.Area() )
            {
                IntersectSegment( static_cast<FOdysseyVectorSegmentCubic*>(iSegment)
                                , static_cast<FOdysseyVectorSegmentCubic*>(intersectSegment)
                                , mGapTolerance
                                , iIntersectionArray );
            }
        }
    }
*/

    //return intersectionCount;
}

FVector
FArianeGraph::GetNodeWorldPositionOnPlane( FNode* Node )
{
    return ( LocalToWorldRotationQuat * FVector( Node->Position.X, Node->Position.Y, 0.0f ) ) + ProjectionPlaneOrigin;
}

void
FArianeGraph::Import( const FVector& ViewOrigin, const TArray<FArianeObject*>& Objects )
{
    FVector::ZAxisVector;
    uint32 NodeCount = 0;
    uint32 LinearEdgeCount = 0;
    uint32 CubicEdgeCount = 0; // will stay at 0 because no Bezier will stay a Bezier due to the projection
    uint32 FractionCount = 0;
    uint32 PathCount = 0;

    NodeBuffer.Empty();
    FractionBuffer.Empty();
    LinearEdgeBuffer.Empty();
    CubicEdgeBuffer.Empty();
    Edges.Empty();
    PathBuffer.Empty();

    // first step, evaluate the memory needed
    for( FArianeObject* Object : Objects )
    {
        if( Object->HasBaseClass( FArianePath::StaticClass() ) )
        {
            FArianePath* Path = static_cast<FArianePath*>(Object);

            NodeCount += Path->GetVertices().Num();

            // for each "sub-segment" (a fraction), we create an edge, beacause projected Bezier Segment won't be Beizer anymore
            for( FArianeSegmentID& SegmentID : Path->GetSegments() )
            {
                FArianeSegment* Segment = SegmentID.GetSegment();

                NodeCount += Segment->GetFractionCount() - 1;
                FractionCount += Segment->GetFractionCount();
                LinearEdgeCount += Segment->GetFractionCount(); // for 3D objects, there are as many edges as fractions
            }

            PathCount++;
        }
    }

    // Allocate memory in one go, for performance
    NodeBuffer.Reserve( NodeCount );
    FractionBuffer.Reserve( LinearEdgeCount );
    LinearEdgeBuffer.Reserve( LinearEdgeCount ); // for 3D objects, there are as many edges as fractions
    CubicEdgeBuffer.Reserve( CubicEdgeCount ); // for 3D objects, there are as many edges as fractions
    Edges.Reserve( LinearEdgeCount + CubicEdgeCount );
    PathBuffer.Reserve( PathCount );

    // first step, evaluate the memory needed
    for( FArianeObject* Object : Objects )
    {
        if( Object->HasBaseClass( FArianePath::StaticClass() ) )
        {
            FArianePath* Path = static_cast<FArianePath*>(Object);
            const FTransform& PathTransform = Path->GetTransform();
            uint32 PathNodeCount = 0;
            uint32 PathLinearEdgeCount = 0;
            uint32 PathCubicEdgeCount = 0;

            for( FArianeVertexID& VertexID : Path->GetVertices() )
            {
                FArianeVertex* Vertex = VertexID.GetVertex();
                FVector VertexWorldPosition = PathTransform.TransformPosition( Vertex->GetPosition() );
                FVector RayDirection = VertexWorldPosition - ViewOrigin;
                FVector IntersectAt;
                bool bProjected = ( FArianeCore::IntersectPlane( ProjectionPlane
                                                               , ViewOrigin
                                                               , RayDirection
                                                               , IntersectAt  ) > 0.0f ) ? true
                                                                                         : false;
                Vertex->SetID( NodeBuffer.Num() );

                NodeBuffer.Emplace( FVector2D( WorldToLocalRotationQuat * ( IntersectAt - ProjectionPlaneOrigin ) )
                                  , bProjected
                                  , VertexWorldPosition );

                PathNodeCount++;
            }

            for( FArianeSegmentID& SegmentID : Path->GetSegments() )
            {
                FArianeSegment* Segment = SegmentID.GetSegment();

                // In theory we could create the edges and the nodes in a single for loop, but this optimizations will come later
                for( FArianeSegment::FFractionStep& Step : Segment->GetFractionSteps() )
                {
                    if( ( Step.T != 0.0f ) && ( Step.T != 1.0f ) )
                    {
                        FVector PointWorldPosition = PathTransform.TransformPosition( Step.Point->GetPosition() );
                        FVector RayDirection = PointWorldPosition - ViewOrigin;
                        FVector IntersectAt;
                        bool bProjected = ( FArianeCore::IntersectPlane( ProjectionPlane
                                                                       , ViewOrigin
                                                                       , RayDirection
                                                                       , IntersectAt  ) > 0.0f ) ? true
                                                                                                 : false;
                        FVector PositionInPlaneSpace = WorldToLocalRotationQuat * ( IntersectAt - ProjectionPlaneOrigin );

                        Step.ID = (uint32) NodeBuffer.Num();

                        NodeBuffer.Emplace( FVector2D( PositionInPlaneSpace )
                                          , bProjected
                                          , PointWorldPosition );

                        PathNodeCount++;
                    }
                }

                // In theory we could create the edges and the nodes in a single for loop, but this optimizations will come later
                // Note: Here for each fraction we create and edge, due to the fact we cannot maintain a Bezier structure anyways
                // as we project it to a plane that will break it parametric nature.
                for( const FArianeSegment::FFraction& SegmentFraction : Segment->GetFractions() )
                {
                    uint32 Point0ID = ( SegmentFraction.Steps[0]->Point->HasBaseClass( FArianeVertex::StaticClass() ) ) ? Segment->GetVertex((uint32)0)->GetID()
                                                                                                                        : SegmentFraction.Steps[0]->ID;
                    uint32 Point1ID = ( SegmentFraction.Steps[1]->Point->HasBaseClass( FArianeVertex::StaticClass() ) ) ? Segment->GetVertex((uint32)1)->GetID()
                                                                                                                        : SegmentFraction.Steps[1]->ID;
                    FNode* Point0 = &NodeBuffer[Point0ID];
                    FNode* Point1 = &NodeBuffer[Point1ID];

                    FFraction* Fraction = &FractionBuffer.Emplace_GetRef( 0.0f, 1.0f, Point0, Point1 );

                    FEdgeLinear* LinearEdge = &LinearEdgeBuffer.Emplace_GetRef( Point0
                                                                              , Point1
                                                                              , Fraction );
                    Edges.Add( LinearEdge );

                    PathLinearEdgeCount++;
                }
            }

            PathBuffer.Emplace( PathNodeCount
                              , &NodeBuffer[ NodeBuffer.Num() - PathNodeCount ]
                              , PathLinearEdgeCount
                              , PathCubicEdgeCount
                              , &Edges[ Edges.Num() - ( PathLinearEdgeCount + PathCubicEdgeCount ) ] );
        }
    }
}

void
FArianeGraph::IntersectPath( FPath* Path )
{
    for( uint32 j = 0; j < Path->GetEdgeCount(); j++ )
    {
        FEdge* Edge = Path->Edges[j];

        for( int k = 0; k < PathBuffer.Num(); k++ )
        {
            FPath& IntersectedPath = PathBuffer[k];

            // populate mXIntersectionRecordArray
            IntersectEdgeWithPath ( Path, Edge, &IntersectedPath );
        }
    }
}

void
FArianeGraph::Intersect( TArray<FPath*>& OutSectionnablePaths
                       , uint32& OutTotalLinearSectionCount
                       , uint32& OutTotalCubicSectionCount )
{
    uint32 TotalSectionCount = 0;

    // find intersections
    if( bMultithreaded )
    {
        uint32 CoreCount = FPlatformMisc::NumberOfCoresIncludingHyperthreads();

#if !PLATFORM_MAC
        ParallelFor( CoreCount
                   , [ this
                     , &OutSectionnablePaths
                     , &OutTotalLinearSectionCount
                     , &OutTotalCubicSectionCount
                     , CoreCount ]( int32 CpuID )
            {
                for( int i = CpuID; i < PathBuffer.Num(); i += CoreCount )
                {
                    FPath& Path = PathBuffer[i];

                    IntersectPath( &Path );
                }
            }
            , EParallelForFlags::None );
#endif
    }
    else
    {
        for( int i = 0; i < PathBuffer.Num(); i++ )
        {
            FPath& Path = PathBuffer[i];

            IntersectPath( &Path );
        }
    }

    // another for-loop to do the counting
    for( FPath& Path : PathBuffer )
    {
        if( Path.IsLoop() || Path.bHasIntersections )
        {
            OutSectionnablePaths.Add( &Path );

            OutTotalLinearSectionCount += Path.LinearEdgeCount;
            OutTotalCubicSectionCount += Path.CubicEdgeCount;
        }
    }
}

void
FArianeGraph::CreateEdgeSection( FEdge* Edge
                               , FNode* SectionNode0
                               , FNode* SectionNode1
                               , double SectionNode0EdgeT
                               , double SectionNode1EdgeT
                               , TArray<FSection*>& ShortSections )
{
    FSection* Section = nullptr;

    if( Edge->GetClass() == FEdgeLinear::StaticClass() )
    {
        FEdgeLinear* LinearEdge = static_cast<FEdgeLinear*>(Edge);

        Section = &LinearSectionBuffer.Emplace_GetRef( LinearEdge
                                                     , SectionNode0
                                                     , SectionNode1
                                                     , SectionNode0EdgeT
                                                     , SectionNode1EdgeT
                                                     , true
                                                     , ShortSections );
    }

    if( Edge->GetClass() == FEdgeCubic::StaticClass() )
    {
        FEdgeCubic* CubicEdge = static_cast<FEdgeCubic*>(Edge);

        Section = &CubicSectionBuffer.Emplace_GetRef( CubicEdge
                                                    , SectionNode0
                                                    , SectionNode1
                                                    , SectionNode0EdgeT
                                                    , SectionNode1EdgeT
                                                    , true
                                                    , ShortSections );
    }

    Sections.Add( Section );
}

void
FArianeGraph::CreateEdgeSections( FEdge* Edge, TArray<FSection*>& ShortSections )
{
    FNode* Node0 = Edge->Nodes[0];
    FNode* Node1 = Edge->Nodes[1];
    FNode* SectionNode0 = Node0;
    double SectionNode0T = 0.0f;


    //Sections = &SectionBuffer[SectionBuffer.Num()];

    if( Edge->Intersections.Num() )
    {
        for( FIntersection* Intersection : Edge->Intersections )
        {
            FNode* SectionNode1 = Intersection->Node;
            double SectionNode1T = Intersection->EdgeT;
            // constructor also links sections to the vertex

            CreateEdgeSection( Edge
                             , SectionNode0
                             , SectionNode1
                             , SectionNode0T
                             , SectionNode1T
                             , ShortSections );

            SectionNode0 = SectionNode1;
            SectionNode0T = SectionNode1T;
        }
    }

    CreateEdgeSection( Edge
                     , SectionNode0
                     , Node1 // edge's second end point
                     , SectionNode0T
                     , 1.0f
                     , ShortSections );
}

void
FArianeGraph::CreatePathSections( FPath* Path, TArray<FSection*>& ShortSections )
{
    for( uint32 i = 0; i < Path->GetEdgeCount(); i++ )
    {
        FEdge *Edge = Path->Edges[i];
        //FNode* Node0 = Edge->Nodes( 0 );
        //FNode* Node1 = Edge->Nodes( 1 );

        CreateEdgeSections( Edge, ShortSections );

        //TODO::Possible optimization: call only if nearestVertex exists
        //CreateVertexGapSegment( vertex0 );
        //CreateVertexGapSegment( vertex1 );
    }
}

void
FArianeGraph::Build()
{
    uint32 TotalLinearSectionCount = 0;
    uint32 TotalCubicSectionCount = 0;
    TArray<FPath*> SectionnablePaths;
    TArray<FSection*> ShortSections;

    LinearSectionBuffer.Empty();
    CubicSectionBuffer.Empty();
    Sections.Empty();

    XIntersectionRecordArray.Empty();
    IntersectionNodeBuffer.Empty();

    SectionnablePaths.Reserve( PathBuffer.Num() );
    // Step2: find intersections
    Intersect( SectionnablePaths, TotalLinearSectionCount, TotalCubicSectionCount );

    // reserve memory to vertices in one go.
    IntersectionNodeBuffer.Reserve( XIntersectionRecordArray.Num() );

    // then init X-Junction intersection vertices.
    for( FNodeIntersection::XRecord& XintersectionRecord : XIntersectionRecordArray )
    {
        IntersectionNodeBuffer.Emplace( XintersectionRecord.Position
                                      , XintersectionRecord.Edge0
                                      , XintersectionRecord.Edge0T
                                      , XintersectionRecord.Edge1
                                      , XintersectionRecord.Edge1T );

        if( XintersectionRecord.Edge0->GetClass() == FEdgeLinear::StaticClass() )
        {
            TotalLinearSectionCount++;
        }

        if( XintersectionRecord.Edge0->GetClass() == FEdgeCubic::StaticClass() )
        {
            TotalCubicSectionCount++;
        }

        if( XintersectionRecord.Edge1->GetClass() == FEdgeLinear::StaticClass() )
        {
            TotalLinearSectionCount++;
        }

        if( XintersectionRecord.Edge1->GetClass() == FEdgeCubic::StaticClass() )
        {
            TotalCubicSectionCount++;
        }
    }

    LinearSectionBuffer.Reserve( TotalLinearSectionCount );
    CubicSectionBuffer.Reserve( TotalCubicSectionCount );
    Sections.Reserve( TotalLinearSectionCount + TotalCubicSectionCount );

    // create sections for exact intersections on each segment
    // only for path that have intersected segments.
    for( FPath* Path : SectionnablePaths )
    {
        CreatePathSections( Path, ShortSections );

        // create a cycle right now for untouched looped-paths
        if( ( Path->IsLoop() == true ) && ( Path->bHasIntersections == false ) )
        {
            TArray<uint32> PathNodeIndices;
            TArray<FSection*> PathSections;

            Path->ToNodeIndicesAndSections( PathNodeIndices, PathSections );

            if( PathNodeIndices.Num() )
            {
                Cycles.Add( new FCycle( PathNodeIndices
                                      , PathSections ) );
            }
        }
    }

    //for( FOdysseyVectorSegmentExtended& extendedSegment : mExtendedSegmentBuffer )
    //{
    //    extendedSegment.GetOwnerAsPath()->RemoveSegment( &extendedSegment );
    //}

    // Get rid of section of length 0
    for( FSection* ShortSection : ShortSections )
    {
        // Unlink() and stitch
        ShortSection->Stitch();
    }
}

void
FArianeGraph::UnlinkPendantSectionsRecursively( FSection* Section, FNode* Node )
{
    do
    {
        FNode* OtherNode = Section->GetOtherNode( Node );

        Section->Unlink( false );

        Node = nullptr;
        Section = nullptr;

        if( OtherNode->GetClass() != FNodeIntersection::StaticClass() )
        {
            if( OtherNode->SectionLinkInfos.Num() == 1 )
            {
                FSection* OtherSection = OtherNode->SectionLinkInfos[0].Section;

                Node = OtherNode;
                Section = OtherSection;
            }
        }
    }
    while( Node );
}

void
FArianeGraph::SimplifyGraph()
{
    // we iterate sections because paths without any intersections (or loops) won't have sections, that way it
    // is faster than iterating using the nodes or edges, as all paths haves nodes and edges, even those that will be ignored.
    for( FSection *Section : Sections )
    {
        if( Section->IsLinked() == true )
        {
            if( Section->Nodes[0] != Section->Nodes[1]  ) // exclude loops
            {
                if( Section->Nodes[0]->SectionLinkInfos.Num() == 1 )
                {
                    UnlinkPendantSectionsRecursively( Section, Section->Nodes[0] );
                }

                if( Section->Nodes[1]->SectionLinkInfos.Num() == 1 )
                {
                    UnlinkPendantSectionsRecursively( Section, Section->Nodes[1] );
                }
            }
        }
    }
}

uint32
FArianeGraph::FindPath( FSection* ReturnSection
                      , uint32 SectionNodeIndex
                      , FSection* Section
                      , TArray<uint32>& OutNodeIndexArray
                      , TArray<FSection*>& OutSectionArray
                      , double Orientation
                      , uint32 Depth ) // we could also use iVertexArray.size()
{
    FNode* SectionNode = Section->Nodes[SectionNodeIndex];
    uint32 SectionNextNodeIndex = ( SectionNodeIndex == 0 ) ? 1 : 0;
    FNode* SectionNextNode = Section->Nodes[SectionNextNodeIndex];
    uint32 Ret = FArianeGraph::NOCYCLE;
    bool bIsLoop = false;

    OutNodeIndexArray.Push( SectionNodeIndex );
    OutSectionArray.Push( Section );
    Section->Block( SectionNodeIndex );

    bIsLoop = ( OutSectionArray[0]->Nodes[OutNodeIndexArray[0]] == SectionNextNode );

    if( ( bIsLoop == true )// cycle detected
    && ( ( ( ReturnSection->IsLinked() == true ) && ( ReturnSection == Section ) ) // 1 return path accepted
        || ( ReturnSection->IsLinked() == false ) ) ) // any return path accepted
    {
        double NormalVector = GetCycleNormalVector( OutNodeIndexArray, OutSectionArray );

        if ( NormalVector > 0.0f )
        {
            Cycles.Add( new FCycle( OutNodeIndexArray, OutSectionArray ) );
        }

        Ret = FArianeGraph::HASCYCLE;
    }
    else
    {
        FSectionLinkInfo* SectionLinkInfo = SectionNextNode->GetSectionLinkInfo( Section, SectionNextNodeIndex );
        FSectionLinkInfo* NextSectionLinkInfo = SectionNextNode->GetCycleNextSection( SectionLinkInfo, 1.0f );

        if( NextSectionLinkInfo )
        {
            FSection* NextSection = NextSectionLinkInfo->Section;

            if( NextSection->IsBlocked( NextSectionLinkInfo->SectionNodeIndex ) == false )
            {
                Ret = FindPath( ReturnSection
                              , NextSectionLinkInfo->SectionNodeIndex
                              , NextSectionLinkInfo->Section
                              , OutNodeIndexArray
                              , OutSectionArray
                              , Orientation
                              , Depth + 1 );
            }
            else
            {
                Ret = FArianeGraph::BLOCKED;
            }
        }
    }

    // propbably useless
    OutNodeIndexArray.Pop();
    OutSectionArray.Pop();

    return Ret;
}

void
FArianeGraph::Explore( FExplorationPair* ExplorationPair )
{
    if( ExplorationPair->DepartSection )
    {
        if( ExplorationPair->DepartSection->IsLinked() == true )
        {
            if( ExplorationPair->DepartSection->IsBlocked( ExplorationPair->DepartNodeIndex ) == false )
            {
                TArray<uint32> NodeIndexArray;
                TArray<FSection*> SectionArray;

                NodeIndexArray.Reserve( 10 );
                SectionArray.Reserve( 10 );

                uint32 ret = FindPath( ExplorationPair->ReturnSection
                                     , ExplorationPair->DepartNodeIndex // lies on DepartSection
                                     , ExplorationPair->DepartSection
                                     , NodeIndexArray
                                     , SectionArray
                                     , 1.0f
                                     , 0 );
            }
        }
    }
}

void
FArianeGraph::MergeCycles()
{
    for( FCycle *Cycle : Cycles )
    {
        FCycle* ParentCycle = Cycle->GetParentCycle();

        if( ParentCycle )
        {
            ParentCycle->Merge( Cycle );
        }
    }
}

void
FArianeGraph::OrderCycles()
{
    for( FCycle* Cycle : Cycles )
    {
        for( FCycle* InnerCycle : Cycles )
        {
           if( Cycle != InnerCycle )
           {
               if( InnerCycle->FitsIn( Cycle ) )
               {
                   FCycle* ParentCycle = InnerCycle->GetParentCycle();

                   if( ParentCycle )
                   {
                       if( ParentCycle->FitsIn( Cycle ) == false )
                       {
                           InnerCycle->SetParentCycle( Cycle );
                       }
                   }
                   else
                   {
                       InnerCycle->SetParentCycle( Cycle );
                   }
               }
           }
        }
    }

    for( FCycle* Cycle : Cycles )
    {
        if( Cycle->ParentCycle )
        {
            Cycle->ParentCycle->Children.Add( Cycle );
        }
    }
}

// Note: For ariane objects, there is no need for cubic segments because a projected cubic segment is not a cubic segment itself
void
FArianeGraph::Solve( const FVector& ViewOrigin, const FPlane& InProjectionPlane, const TArray<FArianeObject*>& Objects, double InGapTolerance )
{
    TArray<FExplorationPair> ExplorationPairsBuffer;

    GapTolerance = InGapTolerance;

    ProjectionPlane = InProjectionPlane;
    ProjectionPlaneOrigin = ProjectionPlane.GetOrigin();
    // find the angle between the Z axis and the plane's normal vector in order to find the rotation matrix.
    // we will then use it to convert 3D points coordinates in a 2D coordinate system (with Z = 0).
    WorldToLocalRotationQuat = FQuat::FindBetweenNormals( ProjectionPlane.GetNormal(), FVector::UpVector );
    // the inverse operation
    LocalToWorldRotationQuat = WorldToLocalRotationQuat.Inverse();

    // clear cycles
    for( FCycle* Cycle : Cycles )
    {
        delete Cycle;
    }

    Cycles.Empty();

    // Step1: Import Paths and convert them into 2D space
    Import( ViewOrigin, Objects );

    // Step2 Intersect and build the Graph
    Build();

    // Build exploration pair before simplification
    for( FNodeIntersection& IntersectionNode : IntersectionNodeBuffer )
    {
        IntersectionNode.BuildExplorationPairs( ExplorationPairsBuffer );
    }

    // sort exploration pairs in order to always have a propagation that starts from
    // the same vertex/section between sessions. This is needed in monothread and
    // multihread modes because the exploration pairs won't be in the same order
    // and we may switch from one to the other.
    ExplorationPairsBuffer.Sort( []( const FExplorationPair& PairA, const FExplorationPair& PairB )
        {
            return PairA.SectionLength > PairB.SectionLength;
        } );

    SimplifyGraph();

    // explore the graph from intersections
    for( FExplorationPair& ExplorationPair : ExplorationPairsBuffer )
    {
        Explore( &ExplorationPair );
    }

    OrderCycles();

    MergeCycles();
}

FArianeGraph::FCycle*
FArianeGraph::PickCycle( const FVector& RayOrigin, const FVector& RayDirection )
{
    FVector IntersectAt;

    if( FArianeCore::IntersectPlane( ProjectionPlane, RayOrigin, RayDirection, IntersectAt ) )
    {
        FVector PositionInPlaneSpace = WorldToLocalRotationQuat * ( IntersectAt - ProjectionPlaneOrigin );

        for( FCycle* Cycle : Cycles )
        {
            if( Cycle->HitTest( PositionInPlaneSpace.X, PositionInPlaneSpace.Y ) )
            {
                return Cycle;
            }
        }
    }

    return nullptr;
}

const FPlane&
FArianeGraph::GetProjectionPlane()
{
    return ProjectionPlane;
}
