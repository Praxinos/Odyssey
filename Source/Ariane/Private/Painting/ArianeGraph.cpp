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
// initial and final vertices is not enough: the walk can come back to o
// through another section without having closed the face.
// At each node, the next section is always picked by the same rule
// (FindNextSectionLinkInfo), so a section explored in a given direction always
// leads to the same next section. Following them always brings the exploration
// back to the depart section, in the same direction, and the face is closed
// exactly then: when the next section is the depart section (same section,
// same direction). At intersection point o,
// a walk is started from each of the 4 sections connected to it :
//
//                        1.      2.        3.          4.
//                      \                      /     \     /
//                       \ o       o        o /       \ o /
//                        /       / \        \
//                       /       /   \        \
//
// See functions:
//     FArianeGraph::Explore()
//     FArianeGraph::FindPath()
//
// The intersection nodes are explored by decreasing Weight (sum of the connected
// section lengths), so that the cycles are always found in the same order between
// sessions, whatever the core count. This is needed by the propagation.
//
// There are also orphaned sections (that lead to nowhere),
// and we get rid of them because they would cause problems. E.g :
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
    FVector2D LastSectionVector = LastSectionLinkInfo->GetVector();
    double MinDot =  DBL_MAX;
    double MaxDot = -DBL_MAX;

    // one pass: best candidate on each side, the right side wins at the end
    for( FSectionLinkInfo& CandidateSectionInfo : CandidateSections )
    {
        if( &CandidateSectionInfo != LastSectionLinkInfo )
        {
            FVector2D SectionVector = CandidateSectionInfo.GetVector();

            if( SectionVector.SquaredLength() )
            {
                double CrossProduct = FVector2D::CrossProduct( -LastSectionVector, SectionVector );
                double Dot = LastSectionVector.Dot( SectionVector );

                if( ( CrossProduct * Orientation >= 0.0f ) && ( Dot > MaxDot ) )
                {
                    RightRet = &CandidateSectionInfo;
                    MaxDot = Dot;
                }

                if( ( CrossProduct * Orientation <= 0.0f ) && ( Dot < MinDot ) )
                {
                    WrongRet = &CandidateSectionInfo;
                    MinDot = Dot;
                }
            }
        }
    }

    if( RightRet )
    {
        return RightRet;
    }

    return WrongRet;
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

FVector2D
FArianeGraph::FSectionLinkInfo::GetVector()
{
    return Section->GetVector( SectionNodeIndex );
}

FArianeGraph::FPoint::~FPoint()
{
}

FArianeGraph::FPoint::FPoint( const FVector& InOriginalWorldPosition )
    : OriginalWorldPosition ( InOriginalWorldPosition )
{
}


FArianeGraph::FNode::~FNode()
{
}

FArianeGraph::FNode::FNode( const FVector& InOriginalWorlPosition )
    : FPoint( InOriginalWorlPosition )
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
                                                  , const FVector& InOriginalWorldPosition
                                                  , FEdge* InEdge0
                                                  , double InEdge0T
                                                  , FEdge* InEdge1
                                                  , double InEdge1T )
    : FNode ( InOriginalWorldPosition )
    , Intersections { FIntersection( this, InEdge0T )
                    , FIntersection( this, InEdge1T ) }
    , Edges { InEdge0,  InEdge1 }
    , Weight ( 0.0 )
{
    Position = InPosition;
    // found between two projected edges
    bProjected = true;

    // attach to edges (ordered, depends on T value)
    Edges[0]->AddIntersection( &Intersections[0] );
    Edges[1]->AddIntersection( &Intersections[1] );
}

void
FArianeGraph::FNodeIntersection::AddSection( FSection* Section, uint32 SectionNodeIndex )
{
    FNode::AddSection( Section, SectionNodeIndex );

    Weight += Section->GetLength();
}

void
FArianeGraph::FNodeIntersection::RemoveSection( FSection* Section, uint32 SectionNodeIndex )
{
    int32 RemovedCount = SectionLinkInfos.Num();

    FNode::RemoveSection( Section, SectionNodeIndex );

    // RemoveAll may remove nothing if the section was not linked here
    RemovedCount -= SectionLinkInfos.Num();
    Weight -= RemovedCount * Section->GetLength();
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
                                                 , const FVector& InAverageWorldPosition
                                                 , FPath* InPath0
                                                 , FEdge* InEdge0
                                                 , double InEdge0T
                                                 , FPath* InPath1
                                                 , FEdge* InEdge1
                                                 , double InEdge1T )
    : Path0 ( InPath0 )
    , Path1 ( InPath1 )
    , Edge0 ( InEdge0 )
    , Edge1 ( InEdge1 )
    , Edge0T ( InEdge0T )
    , Edge1T ( InEdge1T )
    , Position ( InPosition )
    , AverageWorldPosition ( InAverageWorldPosition )
{
    // records are created concurrently by several cores, which may share edges and paths:
    // the edge and path counters are updated afterwards, on a single thread (see Intersect)
}

FArianeGraph::FFraction::~FFraction()
{
}

FArianeGraph::FFraction::FFraction( double InFromT, double InToT, FPoint* InPoint0, FPoint* InPoint1 )
    : FromT ( InFromT )
    , ToT ( InToT )
    , Points { InPoint0, InPoint1 }
{
}

void
FArianeGraph::FFraction::MakeBBox()
{
    BBox.Min.X = FMath::Min( Points[0]->Position.X, Points[1]->Position.X );
    BBox.Min.Y = FMath::Min( Points[0]->Position.Y, Points[1]->Position.Y );
    BBox.Max.X = FMath::Max( Points[0]->Position.X, Points[1]->Position.X );
    BBox.Max.Y = FMath::Max( Points[0]->Position.Y, Points[1]->Position.Y );
    // invalid when a point could not be projected (behind the camera): the box is meaningless
    BBox.bIsValid = ( Points[0]->bProjected && Points[1]->bProjected );
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
    // nodes are not projected yet: BBox and Length are computed later by MakeBBoxAndLength
    : FEdge( InNode0, InNode1, 1, InFractions, 0.0f )
{
}

void
FArianeGraph::FEdgeLinear::MakeBBoxAndLength()
{
    Length = ( Nodes[1]->Position - Nodes[0]->Position ).Length();

    BBox.Min.X = FMath::Min( Nodes[0]->Position.X, Nodes[1]->Position.X );
    BBox.Min.Y = FMath::Min( Nodes[0]->Position.Y, Nodes[1]->Position.Y );
    BBox.Max.X = FMath::Max( Nodes[0]->Position.X, Nodes[1]->Position.X );
    BBox.Max.Y = FMath::Max( Nodes[0]->Position.Y, Nodes[1]->Position.Y );
    // invalid when a node could not be projected (behind the camera): read by IsProjected()
    BBox.bIsValid = ( Nodes[0]->bProjected && Nodes[1]->bProjected );
}

FVector
FArianeGraph::FEdgeLinear::GetOriginalWorldPosition( float T )
{
    return Nodes[0]->OriginalWorldPosition + ( T * ( Nodes[1]->OriginalWorldPosition
                                                   - Nodes[0]->OriginalWorldPosition ) );
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

void
FArianeGraph::FEdgeCubic::MakeBBoxAndLength()
{
    // end points follow the projected nodes, handles are kept as given at construction
    Bezier[0] = Nodes[0]->Position;
    Bezier[3] = Nodes[1]->Position;

    Length = FArianeCore::GetCubicBezierApproximateLength( Bezier, 8 );

    BBox.Min.X = FMath::Min( Bezier[0].X, FMath::Min3 ( Bezier[1].X, Bezier[2].X, Bezier[3].X ) );
    BBox.Min.Y = FMath::Min( Bezier[0].Y, FMath::Min3 ( Bezier[1].Y, Bezier[2].Y, Bezier[3].Y ) );
    BBox.Max.X = FMath::Max( Bezier[0].X, FMath::Max3 ( Bezier[1].X, Bezier[2].X, Bezier[3].X ) );
    BBox.Max.Y = FMath::Max( Bezier[0].Y, FMath::Max3 ( Bezier[1].Y, Bezier[2].Y, Bezier[3].Y ) );
    // invalid when a node could not be projected (behind the camera): read by IsProjected()
    BBox.bIsValid = ( Nodes[0]->bProjected && Nodes[1]->bProjected );
}

FVector
FArianeGraph::FEdgeCubic::GetOriginalWorldPosition( float T )
{
    return ::ULIS::CubicBezierPointAtParameter<FVector>( Nodes[0]->OriginalWorldPosition
                                                       , OriginalHandlePosition[0]
                                                       , OriginalHandlePosition[1]
                                                       , Nodes[1]->OriginalWorldPosition
                                                       , T );
}

FArianeGraph::FPath::~FPath()
{
}

FArianeGraph::FPath::FPath( FArianePath* InOriginalPath
                          , uint32 InNodeCount
                          , FNode* InNodes
                          , uint32 InLinearEdgeCount
                          , uint32 InCubicEdgeCount
                          , FEdge** InEdges )
    : bHasIntersections ( false )
    , bProjected ( false )
    , OriginalPath( InOriginalPath )
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
    // cycles are allocated with new
    Reset( EArianeGraphResetPolicy::All );
}

FArianeGraph::FArianeGraph()
    : SegmentExtensionScheme ( EArianeGraphSegmentExtensionScheme::None )
    , GapTolerance ( 50.0f )
    , Progress ( 0.0f )
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
void
FArianeGraph::IntersectEdges( FPath* Path0
                            , FEdge* Edge0
                            , FPath* Path1
                            , FEdge* Edge1
                            , TArray<FNodeIntersection::XRecord>& OutXIntersectionRecordArray )
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
            if( ( Edge0Fraction->BBox.Min.X <= Edge1->BBox.Max.X )
             && ( Edge0Fraction->BBox.Max.X >= Edge1->BBox.Min.X )
             && ( Edge0Fraction->BBox.Min.Y <= Edge1->BBox.Max.Y )
             && ( Edge0Fraction->BBox.Max.Y >= Edge1->BBox.Min.Y ) )
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
                                        const FVector& Edge0P0WorldPosition = Edge0Fraction->Points[0]->OriginalWorldPosition;
                                        const FVector& Edge0P1WorldPosition = Edge0Fraction->Points[1]->OriginalWorldPosition;
                                        FVector CameraToEdge0Fraction = ( Edge0P0WorldPosition - ViewOrigin );
                                        FVector Edge0FractionWorldVector = ( Edge0P1WorldPosition - Edge0P0WorldPosition ).GetSafeNormal();
                                        FPlane Edge0FractionPlane = FPlane( Edge0P0WorldPosition, FVector::CrossProduct( Edge0FractionWorldVector
                                                                                                                       , CameraToEdge0Fraction ).GetSafeNormal() );
                                        FVector WorldISXEdge1;



                                        const FVector& Edge1P0WorldPosition = Edge1Fraction->Points[0]->OriginalWorldPosition;
                                        const FVector& Edge1P1WorldPosition = Edge1Fraction->Points[1]->OriginalWorldPosition;
                                        FVector CameraToEdge1Fraction = ( Edge1P0WorldPosition - ViewOrigin );
                                        FVector Edge1FractionWorldVector = ( Edge1P1WorldPosition - Edge1P0WorldPosition ).GetSafeNormal();
                                        FPlane Edge1FractionPlane = FPlane( Edge1P0WorldPosition, FVector::CrossProduct( Edge1FractionWorldVector
                                                                                                                       , CameraToEdge1Fraction ).GetSafeNormal() );
                                        FVector WorldISXEdge0;

                                        // We also need to compute the intersection in 3D space
                                        // to be able to create a cycle that will respect the projection because
                                        // if we deduce it from the linear T value in 2D space, we lose precision
                                        // due to the perspective.
                                        FArianeCore::IntersectPlane ( Edge0FractionPlane
                                                                    , Edge1P0WorldPosition
                                                                    , Edge1FractionWorldVector
                                                                    , WorldISXEdge1 );

                                        FArianeCore::IntersectPlane ( Edge1FractionPlane
                                                                    , Edge0P0WorldPosition
                                                                    , Edge0FractionWorldVector
                                                                    , WorldISXEdge0 );

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
                                        // no lock: each core has its own array
                                        OutXIntersectionRecordArray.Emplace( Edge0ISXCoords
                                                                          , ( WorldISXEdge0 + WorldISXEdge1 ) * 0.5f
                                                                          , Path0
                                                                          , Edge0
                                                                          , Edge0T
                                                                          , Path1
                                                                          , Edge1
                                                                          , Edge1T );
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
FArianeGraph::IntersectEdgeWithPath( FPath* Path
                                   , FEdge* Edge
                                   , FPath* IntersectedPath
                                   , TArray<FNodeIntersection::XRecord>& OutXIntersectionRecordArray )
{
    const FBox2D& EdgeBBox = Edge->BBox;

    for( uint32 i = 0; i < IntersectedPath->GetEdgeCount(); i++ )
    {
        FEdge* IntersectedEdge = IntersectedPath->Edges[i];
        const FBox2D& IntersectedEdgeBBox = IntersectedEdge->BBox;

        // an edge going behind the camera has no meaningful 2D image
        if( IntersectedEdge->IsProjected() )
        {
            if( EdgeBBox.Intersect( IntersectedEdgeBBox ) )
            {
                IntersectEdges( Path
                              , Edge
                              , IntersectedPath
                              , IntersectedEdge
                              , OutXIntersectionRecordArray );
            }
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

TArray<FArianeGraph::FNode*>&
FArianeGraph::GetExtendedNodes()
{
    return ExtendedNodes;
}

uint32
FArianeGraph::MakeExtendedSegments( FArianeVertex* Vertex
                                  , FNode& VertexNode
                                  , const FTransform& PathTransform )
{
    FArianeSegment* Segment = Vertex->GetFirstSegment();
    FVector VectorAtVertex = Segment->GetVectorLeavingFromVertex( Vertex, false );
    FVector WorldVertexNormal = PathTransform.TransformVector( Vertex->GetNormal() ).GetSafeNormal();
    FVector WorldVectorAtVertex = PathTransform.TransformVector( VectorAtVertex ).GetSafeNormal();
    FVector VertexWorldPosition = PathTransform.TransformPosition( Vertex->GetPosition() );
    FVector PerpendicularVector = FVector::CrossProduct( WorldVertexNormal, WorldVectorAtVertex ).GetSafeNormal();
    FVector ExtensionVectors[3] = {   WorldVectorAtVertex
                                  ,   PerpendicularVector
                                  , - PerpendicularVector };

    for( uint32 i = 0; i < (uint32) SegmentExtensionScheme; i++ )
    {
        FVector GapVertexWorldPosition = VertexWorldPosition - ( ExtensionVectors[i] * GapTolerance );
        FNode& ExtendedNode = NodeBuffer.Emplace_GetRef( GapVertexWorldPosition );

        FFraction* ExtendedFraction = &FractionBuffer.Emplace_GetRef( 0.0f
                                                                    , 1.0f
                                                                    , &VertexNode
                                                                    , &ExtendedNode );
        FEdgeLinear* ExtendedLinearEdge = &LinearEdgeBuffer.Emplace_GetRef( &VertexNode
                                                                          , &ExtendedNode
                                                                          , ExtendedFraction );

        ExtendedNodes.Add ( &ExtendedNode );
        Edges.Add( ExtendedLinearEdge );
    }

    return (uint32) SegmentExtensionScheme;
}

float
FArianeGraph::GetProgress() const
{
    return Progress.load( std::memory_order_relaxed );
}

void
FArianeGraph::ResetProgress()
{
    Progress.store( 0.0f, std::memory_order_relaxed );
}

void
FArianeGraph::Reset( EArianeGraphResetPolicy ResetPolicy )
{
    ResetProgress();

    for( FCycle* Cycle : Cycles )
    {
        delete Cycle;
    }

    if( ResetPolicy == EArianeGraphResetPolicy::All )
    {
        // Empty() frees the memory, the graph is not needed anymore
        Cycles.Empty();
        LinearSectionBuffer.Empty();
        CubicSectionBuffer.Empty();
        Sections.Empty();
        XIntersectionRecordArrays.Empty();
        IntersectionNodeBuffer.Empty();

        PointBuffer.Empty();
        NodeBuffer.Empty();
        FractionBuffer.Empty();
        LinearEdgeBuffer.Empty();
        CubicEdgeBuffer.Empty();
        ExtendedNodes.Empty();
        Edges.Empty();
        PathBuffer.Empty();

        return;
    }

    // Arrangement: Reset() keeps the allocations for the next solve
    Cycles.Reset();
    LinearSectionBuffer.Reset();
    CubicSectionBuffer.Reset();
    Sections.Reset();
    // per core: keeps each core's allocation for the next solve
    for( TArray<FNodeIntersection::XRecord>& CoreXIntersectionRecords : XIntersectionRecordArrays )
    {
        CoreXIntersectionRecords.Reset();
    }
    IntersectionNodeBuffer.Reset();

    // what BuildArrangement and SolveArrangement wrote into the imported geometry
    for( FNode& Node : NodeBuffer )
    {
        Node.SectionLinkInfos.Reset();
    }

    for( FEdge* Edge : Edges )
    {
        Edge->Intersections.Reset();
        Edge->IntersectionSlotCount = 0;
    }

    for( FPath& Path : PathBuffer )
    {
        Path.bHasIntersections = false;
    }
}

void
FArianeGraph::ProjectGeometry( const FVector& InViewOrigin, const FPlane& InProjectionPlane )
{
    ViewOrigin = InViewOrigin;
    ProjectionPlane = InProjectionPlane;
    ProjectionPlaneOrigin = ProjectionPlane.GetOrigin();
    // find the angle between the Z axis and the plane's normal vector in order to find the rotation matrix.
    // we will then use it to convert 3D points coordinates in a 2D coordinate system (with Z = 0).
    WorldToLocalRotationQuat = FQuat::FindBetweenNormals( ProjectionPlane.GetNormal(), FVector::UpVector );
    // the inverse operation
    LocalToWorldRotationQuat = WorldToLocalRotationQuat.Inverse();

    for( FPath& Path : PathBuffer )
    {
        for( uint32 i = 0; i < Path.NodeCount; i++ )
        {
            FNode* Node = &Path.Nodes[i];
            FVector RayDirection = Node->OriginalWorldPosition - ViewOrigin;
            FVector IntersectAt;
            bool bProjected = FArianeCore::IntersectPlane( ProjectionPlane
                                                         , ViewOrigin
                                                         , RayDirection.GetSafeNormal()
                                                         , IntersectAt );

            FVector PositionInPlaneSpace = WorldToLocalRotationQuat * ( IntersectAt - ProjectionPlaneOrigin );

            Node->Position = FVector2D( PositionInPlaneSpace );
            Node->bProjected = bProjected;
        }

        FBox2D& PathBBox = Path.BBox;

        // stays invalid for a path without edges, and clears the previous projection
        PathBBox = FBox2D( ForceInit );
        // cleared by the first edge that cannot be projected
        Path.bProjected = true;

        // Build bounding boxes (and lengths) of the edges and their fractions, now that nodes are projected
        for( uint32 i = 0; i < Path.GetEdgeCount(); i++ )
        {
            FEdge* Edge = Path.Edges[i];

            Edge->MakeBBoxAndLength();

            // path bbox = union of its projected edge bboxes, gap extensions included.
            // A mirrored (unprojected) box could be huge and would defeat the path test
            if( Edge->IsProjected() )
            {
                if( PathBBox.bIsValid == false )
                {
                    // first projected edge, copies bIsValid = true
                    PathBBox = Edge->BBox;
                }
                else
                {
                    PathBBox.Min.X = FMath::Min( PathBBox.Min.X, Edge->BBox.Min.X );
                    PathBBox.Min.Y = FMath::Min( PathBBox.Min.Y, Edge->BBox.Min.Y );
                    PathBBox.Max.X = FMath::Max( PathBBox.Max.X, Edge->BBox.Max.X );
                    PathBBox.Max.Y = FMath::Max( PathBBox.Max.Y, Edge->BBox.Max.Y );
                }
            }
            else
            {
                Path.bProjected = false;
            }

            for( uint32 j = 0; j < Edge->FractionCount; j++ )
            {
                FFraction* Fraction = &Edge->Fractions[j];

                Fraction->MakeBBox();
            }
        }
    }

    // step weights are estimates, to calibrate with Unreal Insights
    Progress.store( 0.1f, std::memory_order_relaxed );
}

void
FArianeGraph::ImportGeometry( const TArray<FArianeObject*>& Objects
                            , double InGapTolerance
                            , EArianeGraphSegmentExtensionScheme InSegmentExtensionScheme )
{
    FVector::ZAxisVector;
    uint32 NodeCount = 0;
    uint32 LinearEdgeCount = 0;
    uint32 CubicEdgeCount = 0; // will stay at 0 because no Bezier will stay a Bezier due to the projection
    uint32 FractionCount = 0;
    uint32 PathCount = 0;
    // Gaps handling
    uint32 TotalGapSegmentCount = 0;
    uint32 ExtendedNodeCount = 0;
    uint32 ExtendedEdgeCount = 0;

    Reset( EArianeGraphResetPolicy::All );

    // frozen until the next import: BuildArrangement may run on a worker thread meanwhile
    GapTolerance = InGapTolerance;
    SegmentExtensionScheme = InSegmentExtensionScheme;

    // first step, evaluate the memory needed
    for( FArianeObject* Object : Objects )
    {
        if( Object->HasBaseClass( FArianePath::StaticClass() ) )
        {
            FArianePath* Path = static_cast<FArianePath*>(Object);

            NodeCount += Path->GetVertices().Num();

            // for each "sub-segment" (a fraction), we create an edge, beacause projected Bezier Segment won't be Beizer anymore
            for( FArianeSegment* Segment : Path->GetSegments() )
            {
                NodeCount += Segment->GetFractionCount() - 1;
                FractionCount += Segment->GetFractionCount();
                LinearEdgeCount += Segment->GetFractionCount(); // for 3D objects, there are as many edges as fractions
            }

            if( GapTolerance )
            {
                for( FArianeVertex* Vertex : Path->GetVertices() )
                {
                    if( Vertex->GetSegments().Num() == 1 )
                    {
                        ExtendedNodeCount += (int) SegmentExtensionScheme;
                        ExtendedEdgeCount = ExtendedNodeCount;
                    }
                }
            }

            PathCount++;
        }
    }

    // Allocate memory in one go, for performance
    NodeBuffer.Reserve( NodeCount + ExtendedNodeCount );
    FractionBuffer.Reserve( LinearEdgeCount + ExtendedEdgeCount );
    LinearEdgeBuffer.Reserve( LinearEdgeCount + ExtendedEdgeCount ); // for 3D objects, there are as many edges as fractions
    CubicEdgeBuffer.Reserve( CubicEdgeCount );
    ExtendedNodes.Reserve( ExtendedNodeCount );
    Edges.Reserve( LinearEdgeCount + CubicEdgeCount + ExtendedEdgeCount );
    PathBuffer.Reserve( PathCount );

    // Second step, create the nodes in world space (projection is done later by ProjectGeometry).
    for( FArianeObject* Object : Objects )
    {
        if( Object->HasBaseClass( FArianePath::StaticClass() ) )
        {
            FArianePath* OriginalPath = static_cast<FArianePath*>(Object);
            const FTransform& OriginalPathTransform = OriginalPath->GetTransform();
            uint32 PathNodeCount = 0;
            uint32 PathLinearEdgeCount = 0;
            uint32 PathCubicEdgeCount = 0;

            for( FArianeVertex* Vertex : OriginalPath->GetVertices() )
            {
                FVector VertexWorldPosition = OriginalPathTransform.TransformPosition( Vertex->GetPosition() );

                Vertex->SetID( NodeBuffer.Num() );

                FNode& Node = NodeBuffer.Emplace_GetRef( VertexWorldPosition );

                if( GapTolerance )
                {
                    // Gap handling
                    if( Vertex->GetSegments().Num() == 1 )
                    {
                        uint32 ExtensionCount = MakeExtendedSegments( Vertex
                                                                    , Node
                                                                    , OriginalPathTransform );

                        PathLinearEdgeCount += ExtensionCount;
                        PathNodeCount += ExtensionCount;
                    }
                }

                PathNodeCount++;
            }

            for( FArianeSegment* Segment : OriginalPath->GetSegments() )
            {
                // In theory we could create the edges and the nodes in a single for loop, but this optimizations will come later
                for( FArianeSegment::FFractionStep& Step : Segment->GetFractionSteps() )
                {
                    if( Step.Point->HasBaseClass( FArianeVertex::StaticClass() ) == false )
                    {
                        FVector PointWorldPosition = OriginalPathTransform.TransformPosition( Step.Point->GetPosition() );

                        Step.ID = (uint32) NodeBuffer.Num();

                        NodeBuffer.Emplace( PointWorldPosition );

                        PathNodeCount++;
                    }
                }

                // In theory we could create the edges and the nodes in a single for loop, but this optimizations will come later
                // Note: Here for each fraction we create and edge, due to the fact we cannot maintain a Bezier structure anyways
                // as we project it to a plane that will break its parametric nature.
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

            if( PathLinearEdgeCount + PathCubicEdgeCount )
            {
                PathBuffer.Emplace( OriginalPath
                                  , PathNodeCount
                                  , &NodeBuffer[ NodeBuffer.Num() - PathNodeCount ]
                                  , PathLinearEdgeCount
                                  , PathCubicEdgeCount
                                  , &Edges[ Edges.Num() - ( PathLinearEdgeCount + PathCubicEdgeCount ) ] );
            }
        }
    }
}

void
FArianeGraph::IntersectPath( FPath* Path, TArray<FNodeIntersection::XRecord>& OutXIntersectionRecordArray )
{
    const FBox2D& PathBBox = Path->BBox;
    // edges are allocated path after path: a pair of edges from two paths passes IntersectEdges'
    // "Edge0 >= Edge1" test only from the path with the higher index. So this path is only tested
    // against itself and the paths before it, the pairs with later paths being tested from those paths
    const int32 PathIndex = (int32)( Path - PathBuffer.GetData() );

    for( int k = 0; k <= PathIndex; k++ )
    {
        FPath& IntersectedPath = PathBuffer[k];
        const FBox2D& IntersectedPathBBox = IntersectedPath.BBox;

        // paths that do not overlap cannot intersect: all their edge pairs are skipped (bounds included)
        if( PathBBox.Intersect( IntersectedPathBBox ) )
        {
            for( uint32 j = 0; j < Path->GetEdgeCount(); j++ )
            {
                FEdge* Edge = Path->Edges[j];
                const FBox2D& EdgeBBox = Edge->BBox;

                // an edge going behind the camera has no meaningful 2D image
                if( Edge->IsProjected() )
                {
                    // same test for this edge alone, before walking all the edges of the intersected path
                    if( EdgeBBox.Intersect( IntersectedPathBBox ) )
                    {
                        // populate the X-intersection records of this core
                        IntersectEdgeWithPath ( Path, Edge, &IntersectedPath, OutXIntersectionRecordArray );
                    }
                }
            }
        }
    }
}

uint32
FArianeGraph::Intersect( TArray<FPath*>& OutSectionnablePaths
                       , uint32& OutTotalLinearSectionCount
                       , uint32& OutTotalCubicSectionCount )
{
    uint32 TotalSectionCount = 0;
    uint32 TotalXIntersectionRecordCount = 0;

    // find intersections
    if( bMultithreaded )
    {
        uint32 CoreCount = FPlatformMisc::NumberOfCoresIncludingHyperthreads();

        // one record array per core: no lock, and the same order on every solve
        XIntersectionRecordArrays.SetNum( CoreCount );

#if !PLATFORM_MAC
        ParallelFor( CoreCount
                   , [ this
                     , &OutSectionnablePaths
                     , &OutTotalLinearSectionCount
                     , &OutTotalCubicSectionCount
                     , CoreCount ]( int32 CpuID )
            {
                TArray<FNodeIntersection::XRecord>& CoreXIntersectionRecords = XIntersectionRecordArrays[CpuID];

                for( int i = CpuID; i < PathBuffer.Num(); i += CoreCount )
                {
                    FPath& Path = PathBuffer[i];

                    IntersectPath( &Path, CoreXIntersectionRecords );
                }
            }
            , EParallelForFlags::None );
#endif
    }
    else
    {
        XIntersectionRecordArrays.SetNum( 1 );

        for( int i = 0; i < PathBuffer.Num(); i++ )
        {
            FPath& Path = PathBuffer[i];

            IntersectPath( &Path, XIntersectionRecordArrays[0] );
        }
    }

    // single thread: edges and paths may be shared between cores
    for( TArray<FNodeIntersection::XRecord>& CoreXIntersectionRecords : XIntersectionRecordArrays )
    {
        TotalXIntersectionRecordCount += CoreXIntersectionRecords.Num();

        for( FNodeIntersection::XRecord& XIntersectionRecord : CoreXIntersectionRecords )
        {
            XIntersectionRecord.Edge0->IntersectionSlotCount++;
            XIntersectionRecord.Edge1->IntersectionSlotCount++;

            XIntersectionRecord.Path0->bHasIntersections = true;
            XIntersectionRecord.Path1->bHasIntersections = true;
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

    return TotalXIntersectionRecordCount;
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

        // an edge going behind the camera is left out of the arrangement: the path is cut there,
        // and the pendant parts are removed by SimplifyGraph
        if( Edge->IsProjected() )
        {
            CreateEdgeSections( Edge, ShortSections );
        }

        //TODO::Possible optimization: call only if nearestVertex exists
        //CreateVertexGapSegment( vertex0 );
        //CreateVertexGapSegment( vertex1 );
    }
}

// Expects Reset( Arrangement ) and ProjectGeometry to have been called.
void
FArianeGraph::BuildArrangement()
{
    uint32 TotalLinearSectionCount = 0;
    uint32 TotalCubicSectionCount = 0;
    TArray<FPath*> SectionnablePaths;
    TArray<FSection*> ShortSections;

    SectionnablePaths.Reserve( PathBuffer.Num() );
    // Step2: find intersections
    uint32 TotalXIntersectionRecordCount = Intersect( SectionnablePaths, TotalLinearSectionCount, TotalCubicSectionCount );

    Progress.store( 0.5f, std::memory_order_relaxed );

    // reserve memory to vertices in one go.
    IntersectionNodeBuffer.Reserve( TotalXIntersectionRecordCount );

    // then init X-Junction intersection vertices, in core order (same order on every solve)
    for( TArray<FNodeIntersection::XRecord>& CoreXIntersectionRecords : XIntersectionRecordArrays )
    {
        for( FNodeIntersection::XRecord& XintersectionRecord : CoreXIntersectionRecords )
        {
            IntersectionNodeBuffer.Emplace( XintersectionRecord.Position
                                          , XintersectionRecord.AverageWorldPosition
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
        // a loop going behind the camera encloses no finite area on the plane, and some of its edges have no section
        if( ( Path->IsLoop() == true ) && ( Path->bHasIntersections == false ) && ( Path->bProjected == true ) )
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

    Progress.store( 0.6f, std::memory_order_relaxed );
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

        // a node left with a single section is a pendant end, whatever its class. Stopping at
        // intersection nodes would leave pendant sections behind, depending on the section order
        if( OtherNode->SectionLinkInfos.Num() == 1 )
        {
            FSection* OtherSection = OtherNode->SectionLinkInfos[0].Section;

            Node = OtherNode;
            Section = OtherSection;
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
FArianeGraph::FindPath( uint32 SectionNodeIndex
                      , FSection* Section
                      , TArray<uint32>& OutNodeIndexArray
                      , TArray<FSection*>& OutSectionArray
                      , double Orientation )
{
    uint32 Ret = FArianeGraph::NOCYCLE;
    bool bExploring = true;

    while( bExploring == true )
    {
        uint32 SectionNextNodeIndex = ( SectionNodeIndex == 0 ) ? 1 : 0;
        FNode* SectionNextNode = Section->Nodes[SectionNextNodeIndex];

        OutNodeIndexArray.Push( SectionNodeIndex );
        OutSectionArray.Push( Section );
        Section->Block( SectionNodeIndex );

        FSectionLinkInfo* SectionLinkInfo = SectionNextNode->GetSectionLinkInfo( Section, SectionNextNodeIndex );
        FSectionLinkInfo* NextSectionLinkInfo = SectionNextNode->GetCycleNextSection( SectionLinkInfo, 1.0f );

        // dead end: no next section
        bExploring = false;

        if( NextSectionLinkInfo )
        {
            // the face is closed when the next section is the depart section, in the same direction.
            // Tested before IsBlocked: the depart section is blocked in this direction since the first step
            bool bIsLoop = ( NextSectionLinkInfo->Section          == OutSectionArray[0]   )
                        && ( NextSectionLinkInfo->SectionNodeIndex == OutNodeIndexArray[0] );

            if( bIsLoop == true )
            {
                double NormalVector = GetCycleNormalVector( OutNodeIndexArray, OutSectionArray );

                if( NormalVector > 0.0f )
                {
                    Cycles.Add( new FCycle( OutNodeIndexArray, OutSectionArray ) );
                }

                Ret = FArianeGraph::HASCYCLE;
            }
            else if( NextSectionLinkInfo->Section->IsBlocked( NextSectionLinkInfo->SectionNodeIndex ) == false )
            {
                // go on with the next section
                SectionNodeIndex = NextSectionLinkInfo->SectionNodeIndex;
                Section = NextSectionLinkInfo->Section;
                bExploring = true;
            }
            else
            {
                Ret = FArianeGraph::BLOCKED;
            }
        }
    }

    // Reset keeps the allocation for the next departure
    OutNodeIndexArray.Reset();
    OutSectionArray.Reset();

    return Ret;
}

void
FArianeGraph::Explore( FNodeIntersection* IntersectionNode )
{
    TArray<uint32> NodeIndexArray;
    TArray<FSection*> SectionArray;

    NodeIndexArray.Reserve( 10 );
    SectionArray.Reserve( 10 );

    for( FSectionLinkInfo& DepartSectionLinkInfo : IntersectionNode->SectionLinkInfos )
    {
        // a section blocked in this direction already belongs to a detected face
        if( DepartSectionLinkInfo.Section->IsBlocked( DepartSectionLinkInfo.SectionNodeIndex ) == false )
        {
            // FindPath empties the arrays before returning
            FindPath( DepartSectionLinkInfo.SectionNodeIndex
                    , DepartSectionLinkInfo.Section
                    , NodeIndexArray
                    , SectionArray
                    , 1.0f );
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

void
FArianeGraph::SortIntersectionNodes( TArray<FNodeIntersection*>& OutSortedIntersectionNodes )
{
    OutSortedIntersectionNodes.Reserve( IntersectionNodeBuffer.Num() );

    for( FNodeIntersection& IntersectionNode : IntersectionNodeBuffer )
    {
        OutSortedIntersectionNodes.Add( &IntersectionNode );
    }

    OutSortedIntersectionNodes.Sort( []( const FNodeIntersection& NodeA, const FNodeIntersection& NodeB )
        {
            if( NodeA.Weight != NodeB.Weight )
            {
                return NodeA.Weight > NodeB.Weight;
            }

            if( NodeA.Position.X != NodeB.Position.X )
            {
                return NodeA.Position.X < NodeB.Position.X;
            }

            return NodeA.Position.Y < NodeB.Position.Y;
        } );
}

// Note: For ariane objects, there is no need for cubic segments because a projected cubic segment is not a cubic segment itself
// Expects BuildArrangement to have been called.
void
FArianeGraph::SolveArrangement()
{
    // pointers only: sections point to the nodes, sorting IntersectionNodeBuffer itself would move them
    TArray<FNodeIntersection*> SortedIntersectionNodes;

    SimplifyGraph();

    Progress.store( 0.7f, std::memory_order_relaxed );

    // IntersectionNodeBuffer order depends on the core count (mono/multithread, machine).
    // Sorting gives the same exploration order, hence the same cycle order for the future
    // propagation, on every session. Position breaks ties between equal weights (symmetric drawings).
    // Note: TArray<T*>::Sort passes the pointed objects to the predicate, not the pointers
    SortIntersectionNodes( SortedIntersectionNodes );

    // explore the graph from intersections
    for( FNodeIntersection* IntersectionNode : SortedIntersectionNodes )
    {
        Explore( IntersectionNode );
    }

    Progress.store( 0.9f, std::memory_order_relaxed );

    OrderCycles();

    MergeCycles();

    Progress.store( 1.0f, std::memory_order_relaxed );
}

FArianeGraph::FCycle*
FArianeGraph::PickCycle( const FVector& RayOrigin, const FVector& RayDirection )
{
    FVector IntersectAt;

    if( FArianeCore::IntersectPlane( ProjectionPlane, RayOrigin, RayDirection, IntersectAt ) )
    {
        FVector PositionInPlaneSpace = WorldToLocalRotationQuat * ( IntersectAt - ProjectionPlaneOrigin );
        FVector2D Position2D = FVector2D( PositionInPlaneSpace );

        for( FCycle* Cycle : Cycles )
        {
            // cheap exact rejection before the Blend2D hit test. Bounds included, like FitsIn
            if( Cycle->BBox.IsInsideOrOn( Position2D ) )
            {
                if( Cycle->HitTest( PositionInPlaneSpace.X, PositionInPlaneSpace.Y ) )
                {
                    return Cycle;
                }
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
