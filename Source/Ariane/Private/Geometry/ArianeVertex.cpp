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
    : FArianePoint( iPosition )
    , Radius( InRadius )
{
}

FArianeVertex::FArianeVertex( const FVector& iPosition, const FVector& InNormal, double InRadius )
    : FArianePoint( iPosition )
    , Radius( InRadius )
    , Normal ( InNormal )
{
}

void
FArianeVertex::SetNormal( const FVector& InNormal )
{
    Normal = InNormal;
}

const FVector&
FArianeVertex::GetNormal()
{
    return Normal;
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
