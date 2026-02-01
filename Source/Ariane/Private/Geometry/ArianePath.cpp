// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianePath.h"
#include "ArianeVertex.h"
#include "ArianeSegment.h"

FArianePath::~FArianePath()
{
}

FArianePath::FArianePath()
{
}

void
FArianePath::AddVertex( FArianeVertex* iVertex )
{
    Vertices.Push( iVertex );

    iVertex->SetOwner( this );
}

void
FArianePath::RemoveVertex( FArianeVertex* iVertex )
{
    Vertices.Remove( iVertex );

    iVertex->SetOwner( nullptr );
}

void
FArianePath::AddSegment( FArianeSegment* iSegment )
{
    Segments.Push( iSegment );

    iSegment->Link();
    iSegment->SetOwner( nullptr );
}

void
FArianePath::RemoveSegment( FArianeSegment* iSegment )
{
    Segments.Remove( iSegment );

    iSegment->Unlink();
    iSegment->SetOwner( nullptr );
}

const TArray<FArianeSegment*>&
FArianePath::GetSegments()
{
    return Segments;
}

const TArray<FArianeVertex*>&
FArianePath::GetVertices()
{
    return Vertices;
}
