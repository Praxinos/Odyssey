// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Headers
#include "ArianeKeyedPath.h"
#include "ArianePath.h"
#include "ArianeSegment.h"
#include "ArianeSegmentCubic.h"

FArianeKeyedPath::~FArianeKeyedPath()
{
}

FArianeKeyedPath::FArianeKeyedPath()
{
}

FArianeKeyedPath::FArianeKeyedPath( FArianePath* Path )
    : FArianeKeyedObject( Path )
    , KeyedColor ( Path->GetColor() )
{
    KeyedVertices.Reserve( Path->GetVertices().Num() );
    KeyedCubicSegments.Reserve( Path->GetCubicSegmentCount() );
    KeyedLinearSegments.Reserve( Path->GetLinearSegmentCount() );

    for( FArianeVertexID& VertexID : Path->GetVertices() )
    {
        KeyedVertices.Emplace( VertexID.GetVertex() );
    }

    for( FArianeSegmentID& SegmentID : Path->GetSegments() )
    {
        FArianeSegment* Segment = SegmentID.GetSegment();

        if( Segment->GetClass() == FArianeSegmentCubic::StaticClass() )
        {
            FArianeSegmentCubic* CubicSegment = static_cast<FArianeSegmentCubic*>(Segment);

            &KeyedCubicSegments.Emplace_GetRef( CubicSegment );
        }
    }

    BuildVertexLookup();
    BuildSegmentLookup();
}

FArianeKeyedVertex*
FArianeKeyedPath::GetKeyedVertex( const FGuid& VertexGuid )
{
    FArianeKeyedVertex** FoundVertex = nullptr;

    FoundVertex = KeyedVertexLookup.Find( VertexGuid );


    return FoundVertex ? *FoundVertex : nullptr;
}

FArianeKeyedSegment*
FArianeKeyedPath::GetKeyedSegment( const FGuid& SegmentGuid )
{
    FArianeKeyedSegment** FoundSegment = nullptr;

    FoundSegment = KeyedSegmentLookup.Find( SegmentGuid );


    return FoundSegment ? *FoundSegment : nullptr;
}

const FColor&
FArianeKeyedPath::GetKeyedColor() const
{
    return KeyedColor;
}

void
FArianeKeyedPath::PostLoad()
{
    Super::PostLoad();

    BuildVertexLookup();
    BuildSegmentLookup();
}

void
FArianeKeyedPath::PostEditUndo()
{
    Super::PostEditUndo();

    BuildVertexLookup();
    BuildSegmentLookup();
}

void
FArianeKeyedPath::BuildVertexLookup()
{
    KeyedVertexLookup.Empty();
    KeyedVertexLookup.Reserve( KeyedVertices.Num() );

    for( FArianeKeyedVertex& KeyedVertex : KeyedVertices )
    {
        KeyedVertexLookup.Add( KeyedVertex.GetGuid(), &KeyedVertex );
    }
}

void
FArianeKeyedPath::BuildSegmentLookup()
{
    KeyedSegmentLookup.Empty();
    KeyedSegmentLookup.Reserve( KeyedCubicSegments.Num() + KeyedLinearSegments.Num() );

    for( FArianeKeyedSegmentCubic& KeyedCubicSegment : KeyedCubicSegments )
    {
        KeyedSegmentLookup.Add( KeyedCubicSegment.GetGuid(), &KeyedCubicSegment );
    }
}
