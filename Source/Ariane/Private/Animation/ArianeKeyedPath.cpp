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
    //KeyedCubicSegments.Reserve( Path->GetCubicSegmentCount() );
    //KeyedLinearSegments.Reserve( Path->GetLinearSegmentCount() );
    KeyedSegments.Reserve( Path->GetSegments().Num() );

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

            KeyedSegments.Add( &KeyedCubicSegments.Emplace_GetRef( CubicSegment ) );
        }
    }
}

FArianeKeyedVertex*
FArianeKeyedPath::GetKeyedVertex( const FGuid& VertexGuid )
{
    for( FArianeKeyedVertex& KeyedVertex : KeyedVertices )
    {
        if( KeyedVertex.GetGuid() == VertexGuid )
        {
            return &KeyedVertex;
        }
    }

    return nullptr;
}

FArianeKeyedSegment*
FArianeKeyedPath::GetKeyedSegment( const FGuid& SegmentGuid )
{
    for( FArianeKeyedSegment& KeyedLinearSegment : KeyedLinearSegments )
    {
        if( KeyedLinearSegment.GetGuid() == SegmentGuid )
        {
            return &KeyedLinearSegment;
        }
    }

    for( FArianeKeyedSegmentCubic& KeyedCubicSegment : KeyedCubicSegments )
    {
        if( KeyedCubicSegment.GetGuid() == SegmentGuid )
        {
            return &KeyedCubicSegment;
        }
    }

    return nullptr;
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
}

void
FArianeKeyedPath::PostEditUndo()
{
    Super::PostEditUndo();
}
