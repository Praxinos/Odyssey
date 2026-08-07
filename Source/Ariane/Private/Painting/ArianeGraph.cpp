// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeGraph.h"
#include "ArianeCore.h"
#include "ArianeObject.h"
#include "ArianePath.h"
#include "ArianeVertex.h"
#include "ArianeSegment.h"

FVector2D
FArianeGraph::FSectionLinkInfo::GetVector()
{
    return Section->GetVector( SectionNodeIndex );
}

FArianeGraph::FPoint::FPoint( const FVector2D& InPosition, bool bInProjected, const FVector& InOriginalPosition )
    : Position ( InPosition )
    , bProjected ( bInProjected )
    , OriginalPosition ( InOriginalPosition )
{
}

FArianeGraph::FNode::FNode( const FVector2D& InPosition, bool bInProjected, const FVector& InOriginalPosition )
    : FPoint( InPosition, bInProjected, InOriginalPosition )
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

FArianeGraph::FIntersection::FIntersection( FNode* InNode, float InEdgeT )
    : Node ( InNode )
    , EdgeT ( InEdgeT )
{
}

FArianeGraph::FNodeIntersection::FNodeIntersection( const FVector2D& InPosition
                                                  , FEdge* InEdge0
                                                  , double InEdge0T
                                                  , FEdge* InEdge1
                                                  , double InEdge1T )
    : FNode ( InPosition, true, InEdge0->GetOriginalPosition( InEdge0T ) )
    , Intersections { FIntersection( this, InEdge0T )
                    , FIntersection( this, InEdge1T ) }
    , Edges { InEdge0,  InEdge1 }
{
    // attach to edges
    Edges[0]->Intersections.Add( &Intersections[0] );
    Edges[1]->Intersections.Add( &Intersections[1] );
}

bool
FArianeGraph::FNodeIntersection::SelfIntersects()
{
    return Edges[0] == Edges[1];
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

FArianeGraph::FEdge::FEdge( FNode* InNode0
                          , FNode* InNode1
                          , uint32 InFractionCount
                          , FFraction* InFractions
                          , double InLength )
    : Nodes { InNode0, InNode1 }
    , Fractions( InFractions )
    , FractionCount ( 1 )
    , IntersectionSlotCount ( 0 )
    , Length ( InLength )
{
    Nodes[0]->Edges[Nodes[0]->EdgeCount++] = this;
    Nodes[1]->Edges[Nodes[1]->EdgeCount++] = this;
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
FArianeGraph::FEdgeLinear::GetOriginalPosition( float T )
{
    return Nodes[0]->OriginalPosition + ( T * ( Nodes[1]->OriginalPosition
                                              - Nodes[0]->OriginalPosition ) );
}

FArianeGraph::FEdgeCubic::FEdgeCubic( FNode* InNode0
                                    , const FVector2D& Handle0Position
                                    , const FVector& OriginalHandle0Position
                                    , const FVector2D& Handle1Position
                                    , const FVector& OriginalHandle1Position
                                    , FNode* InNode1
                                    , uint32 InFractionCount
                                    , FFraction* InFractions )
    : FEdge( InNode0, InNode1, 1, InFractions, 0.0f )
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
FArianeGraph::FEdgeCubic::GetOriginalPosition( float T )
{
    return ::ULIS::CubicBezierPointAtParameter<FVector>( Nodes[0]->OriginalPosition
                                                       , OriginalHandlePosition[0]
                                                       , OriginalHandlePosition[1]
                                                       , Nodes[1]->OriginalPosition
                                                       , T );
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

FArianeGraph::FArianeGraph()
    : GapTolerance ( 0.0f )
{
#if PLATFORM_MAC
    bMultithreaded = false;
#else
    bMultithreaded = true;
    bMultithreaded = false;
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
             && ( Edge0Fraction->BBox.Max.Y >= Edge1MinWithTolerance.X )
             && ( Edge0Fraction->BBox.Min.X <= Edge1MaxWithTolerance.Y )
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

        if( ( EdgeMinWithTolerance.X < IntersectedEdgeMaxWithTolerance.X )
         && ( EdgeMaxWithTolerance.X > IntersectedEdgeMinWithTolerance.X )
         && ( EdgeMinWithTolerance.Y < IntersectedEdgeMaxWithTolerance.Y )
         && ( EdgeMaxWithTolerance.Y > IntersectedEdgeMinWithTolerance.Y ) )
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

void
FArianeGraph::Import( const FVector& ViewOrigin, const TArray<FArianeObject*>& Objects )
{
    // find the angle between the Z axis and the plane's normal vector in order to find the rotation matrix.
    // we will then use it to convert 3D points coordinates in a 2D coordinate system (with Z = 0).
    FQuat RotationQuat = FQuat::FindBetweenNormals( ProjectionPlane.GetNormal(), FVector::UpVector );
    FVector ProjectionPlaneOrigin = ProjectionPlane.GetOrigin();
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
                FVector RayDirection = PathTransform.TransformPosition( Vertex->GetPosition() ) - ViewOrigin;
                FVector IntersectAt;
                bool bProjected = ( FArianeCore::IntersectPlane( ProjectionPlane
                                                               , ViewOrigin
                                                               , RayDirection
                                                               , IntersectAt  ) > 0.0f ) ? true
                                                                                         : false;
                Vertex->SetID( NodeBuffer.Num() );

                NodeBuffer.Emplace( FVector2D( RotationQuat * ( IntersectAt - ProjectionPlaneOrigin ) )
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
                        FVector PositionInPlaneSpace = RotationQuat * ( IntersectAt - ProjectionPlaneOrigin );

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

// returns true if the path needs to be taken into account in the graph
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
FArianeGraph::Intersect( TArray<FPath*>& SectionnablePaths
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
                     , &SectionnablePaths
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
            SectionnablePaths.Add( &Path );

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

// Note: For ariane objects, there is no need for cubic segments because a projected cubic segment is not a cubic segment itself
void
FArianeGraph::Build( const FVector& ViewOrigin, const FPlane& InProjectionPlane, const TArray<FArianeObject*>& Objects )
{
    uint32 TotalLinearSectionCount = 0;
    uint32 TotalCubicSectionCount = 0;
    TArray<FPath*> SectionnablePaths;
    TArray<FSection*> ShortSections;

    ProjectionPlane = InProjectionPlane;


    // clear cycles
    for( FCycle* Cycle : Cycles )
    {
        delete Cycle;
    }

    Cycles.Empty();

    // Step1: Import Paths and convert them into 2D space
    Import( ViewOrigin, Objects );


    LinearSectionBuffer.Empty();
    CubicSectionBuffer.Empty();
    Sections.Empty();

    IntersectionNodeBuffer.Empty();

    SectionnablePaths.Reserve( PathBuffer.Num() );
    // Step2: find intersections
    Intersect( SectionnablePaths, TotalLinearSectionCount, TotalCubicSectionCount );

    LinearSectionBuffer.Reserve( TotalLinearSectionCount );
    CubicSectionBuffer.Reserve( TotalCubicSectionCount );
    Sections.Reserve( TotalLinearSectionCount + TotalCubicSectionCount );

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

FArianeGraph::FCycle*
FArianeGraph::PickCycle( const FVector& RayOrigin, const FVector& RayDirection )
{
    // find the angle between the Z axis and the plane's normal vector in order to find the rotation matrix.
    // we will then use it to convert 3D points coordinates in a 2D coordinate system (with Z = 0).
    FQuat RotationQuat = FQuat::FindBetweenNormals( ProjectionPlane.GetNormal(), FVector::UpVector );
    FVector ProjectionPlaneOrigin = ProjectionPlane.GetOrigin();
    FVector IntersectAt;

    if( FArianeCore::IntersectPlane( ProjectionPlane, RayOrigin, RayDirection, IntersectAt ) )
    {
        FVector PositionInPlaneSpace = RotationQuat * ( IntersectAt - ProjectionPlaneOrigin );

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
