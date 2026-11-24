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

    for( FArianeVertex* Vertex : Path->GetVertices() )
    {
        KeyedVertices.Emplace( Vertex );
    }

    for( FArianeSegment* Segment : Path->GetSegments() )
    {
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
    // The cache is not serialized: it is empty after a load, a paste or an undo
    if( KeyedVertexLookup.Num() != KeyedVertices.Num() )
    {
        BuildVertexLookup();
    }

    int32* FoundIndex = KeyedVertexLookup.Find( VertexGuid );

    return FoundIndex ? &KeyedVertices[*FoundIndex]
                      : nullptr;
}

FArianeKeyedSegment*
FArianeKeyedPath::GetKeyedSegment( const FGuid& SegmentGuid )
{
    // The cache is not serialized: it is empty after a load, a paste or an undo
    if( KeyedSegmentLookup.Num() != ( KeyedCubicSegments.Num() + KeyedLinearSegments.Num() ) )
    {
        BuildSegmentLookup();
    }

    int32* FoundIndex = KeyedSegmentLookup.Find( SegmentGuid );

    if( FoundIndex )
    {
        // Cubic segments come first in the index space
        return ( *FoundIndex < KeyedCubicSegments.Num() ) ? static_cast<FArianeKeyedSegment*>( &KeyedCubicSegments[*FoundIndex] )
                                                          : &KeyedLinearSegments[*FoundIndex - KeyedCubicSegments.Num()];
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

    // The undone arrays may have the same size but other elements: let the getters rebuild the caches
    KeyedVertexLookup.Empty();
    KeyedSegmentLookup.Empty();
}

void
FArianeKeyedPath::BuildVertexLookup()
{
    KeyedVertexLookup.Empty();
    KeyedVertexLookup.Reserve( KeyedVertices.Num() );

    for( int32 i = 0; i < KeyedVertices.Num(); i++ )
    {
        KeyedVertexLookup.Add( KeyedVertices[i].GetGuid(), i );
    }
}

void
FArianeKeyedPath::BuildSegmentLookup()
{
    KeyedSegmentLookup.Empty();
    KeyedSegmentLookup.Reserve( KeyedCubicSegments.Num() + KeyedLinearSegments.Num() );

    for( int32 i = 0; i < KeyedCubicSegments.Num(); i++ )
    {
        KeyedSegmentLookup.Add( KeyedCubicSegments[i].GetGuid(), i );
    }

    for( int32 i = 0; i < KeyedLinearSegments.Num(); i++ )
    {
        KeyedSegmentLookup.Add( KeyedLinearSegments[i].GetGuid(), KeyedCubicSegments.Num() + i );
    }
}
