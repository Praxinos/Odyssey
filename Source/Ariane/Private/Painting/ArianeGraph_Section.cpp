// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeGraph.h"

FArianeGraph::FSection::FSection( FEdge* InEdge
                                , FNode* InNode0
                                , FNode* InNode1
                                , double InEdgeT0
                                , double InEdgeT1 )
    : Edge ( InEdge )
    , Nodes { InNode0, InNode1 }
    , OriginalNodes { InNode0, InNode1 }
    , Flags ( 0 )
    , CycleCount ( 0 )
    , Cycles { nullptr, nullptr }
    , Length ( 0 )
    , EdgeT{ InEdgeT0, InEdgeT1 }
{
}

void
FArianeGraph::FSection::Stitch()
{
    TArray<FSectionLinkInfo> SectionLinkInfosCopy;

    // unlink first or else it will be returned in the arrays
    Unlink( false );

    // Intersection vertices have priority because exploration pairs are built from it
    // so we wan't to keep them in the graph
    if( Nodes[1]->GetClass() == FNodeIntersection::StaticClass() )
    {
        SectionLinkInfosCopy = Nodes[0]->SectionLinkInfos;

        for( FSectionLinkInfo& SectionLinkInfo : SectionLinkInfosCopy )
        {
            SectionLinkInfo.Section->Unlink( false );
            SectionLinkInfo.Section->Nodes[SectionLinkInfo.SectionNodeIndex] = Nodes[1];
            SectionLinkInfo.Section->Link();
        }
    }
    else
    {
        SectionLinkInfosCopy = Nodes[1]->SectionLinkInfos;

        for( FSectionLinkInfo& SectionLinkInfo : SectionLinkInfosCopy )
        {
            SectionLinkInfo.Section->Unlink( false );
            SectionLinkInfo.Section->Nodes[SectionLinkInfo.SectionNodeIndex] = Nodes[0];
            SectionLinkInfo.Section->Link();
        }
    }
}

double
FArianeGraph::FSection::GetLength()
{
    return Length;
}

FVector2D
FArianeGraph::FSection::GetVector( uint32 NodeIndex )
{
    return Vector[NodeIndex];
}

/* Gary
bool
FArianeGraph::FSection::IsValid()
{
    if( ( Length == 0.0f )
     && ( ( Nodes[0]->GetSectionCount( eVertexSectionTypeQuery::Any, nullptr, nullptr ) > 2 )
       || ( Nodes[1]->GetSectionCount( eVertexSectionTypeQuery::Any, nullptr, nullptr ) > 2 ) ) )
    {
        return false;
    }

    return true;
}
*/
/*
bool
FArianeGraph::FSection::IsGap()
{
    return ( mSegment->GetClass() == FOdysseyVectorSegmentCubicGap::StaticClass() )
        || ( mSegment->GetClass() == FOdysseyVectorSegmentExtended::StaticClass() );
}
*/

FArianeGraph::FCycle*
FArianeGraph::FSection::GetCycle( uint32 CycleID )
{
    return Cycles[CycleID];
}

void
FArianeGraph::FSection::UnBlock( uint32 NodeIndex )
{
    uint32 blocked = ( NodeIndex == 0 ) ? BLOCKNODE0
                                        : BLOCKNODE1;

    Flags &= (~blocked);
}

void
FArianeGraph::FSection::Block( uint32 NodeIndex )
{
    uint32 Blocked = ( NodeIndex == 0 ) ? BLOCKNODE0
                                        : BLOCKNODE1;

    Flags |= Blocked;
}

bool
FArianeGraph::FSection::IsBlocked( uint32 NodeIndex )
{
    uint32 Blocked = ( NodeIndex == 0 ) ? BLOCKNODE0
                                        : BLOCKNODE1;

    return ( Flags & Blocked ) ? true : false;
}

bool
FArianeGraph::FSection::IsLinked()
{
    return ( Flags & LINKED ) ? true : false;
}

bool
FArianeGraph::FSection::IsErased()
{
    return ( Flags & ERASED ) ? true : false;
}

void
FArianeGraph::FSection::SetErased( bool bErased )
{
    if( bErased )
    {
        Flags |= ERASED;
    }
    else
    {
        Flags &= (~ERASED);
    }
}

void
FArianeGraph::FSection::AddCycle( FCycle* InCycle )
{
    Cycles[CycleCount++] = InCycle;
}

FArianeGraph::FCycle*
FArianeGraph::FSection::GetOtherCycle( FCycle* InCycle )
{
    return ( Cycles[0] == InCycle ) ? Cycles[1] : Cycles[0];
}

bool
FArianeGraph::FSection::HasCycle( FCycle* InCycle )
{
    return ( ( Cycles[0] == InCycle ) || ( Cycles[1] == InCycle ) );
}

void
FArianeGraph::FSection::Link()
{
    // Note: a looping section will be added twice
    Nodes[0]->AddSection( this, 0 );
    Nodes[1]->AddSection( this, 1 );

    Flags |= LINKED;
}

void
FArianeGraph::FSection::Unlink( bool bRestore )
{
    Nodes[0]->RemoveSection( this, 0 );
    Nodes[1]->RemoveSection( this, 1 );

    if( bRestore )
    {
        Nodes[0] = OriginalNodes[0];
        Nodes[1] = OriginalNodes[1];
    }

    Flags &= (~LINKED);
}

FArianeGraph::FNode*
FArianeGraph::FSection::GetOtherNode( FNode* InNode )
{
    return ( InNode == Nodes[0] ) ? Nodes[1] : Nodes[0];
}

FArianeGraph::FEdge*
FArianeGraph::FSection::GetEdge()
{
    return Edge;
}

double
FArianeGraph::FSection::GetEdgeT( uint32 Index )
{
    return EdgeT[Index];
}
