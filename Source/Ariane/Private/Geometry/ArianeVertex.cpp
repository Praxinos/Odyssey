// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeVertex.h"

FArianeVertex::~FArianeVertex()
{
}

FArianeVertex::FArianeVertex( double X, double Y, double Z, double InRadius )
    : FArianePoint( X, Y, Z )
    , Radius( InRadius )
{
}

FArianeVertex::FArianeVertex( const FVector& iPosition, double InRadius )
    // Delegating constructor
    : FArianeVertex( iPosition.X, iPosition.Y, iPosition.Z, InRadius )
{
}

void
FArianeVertex::SetOwner( FArianeObject* iOwner )
{
    Owner = iOwner;
}

void
FArianeVertex::AddSegment( FArianeSegment* iSegment )
{
    Segments.Add( iSegment );
}

void
FArianeVertex::RemoveSegment( FArianeSegment* iSegment )
{
    Segments.Remove( iSegment );
}

const TArray<FArianeSegment*>&
FArianeVertex::GetSegments()
{
    return Segments;
}

double
FArianeVertex::GetRadius()
{
    return Radius;
}
