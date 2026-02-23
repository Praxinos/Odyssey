// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeVertex.h"
#include "ArianePath.h"
#include "ArianePainting3DComponent.h"

FArianeVertex::~FArianeVertex()
{
}

FArianeVertex::FArianeVertex( FArianeObject* Owner, const FVector& iPosition, const FVector& InNormal, double InRadius )
    : FArianePoint( iPosition )
    , Guid( FGuid::NewGuid() )
    , OwnerID( Owner )
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

FArianeObject*
FArianeVertex::GetOwner()
{
    return OwnerID.GetObject();
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
