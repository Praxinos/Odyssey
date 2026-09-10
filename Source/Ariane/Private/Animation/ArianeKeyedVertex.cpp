// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Headers
#include "ArianeKeyedVertex.h"
#include "ArianeVertex.h"

FArianeKeyedVertex::~FArianeKeyedVertex()
{
}

FArianeKeyedVertex::FArianeKeyedVertex()
{
}

FArianeKeyedVertex::FArianeKeyedVertex( FArianeVertex* Vertex )
    : Guid( Vertex->GetGuid() )
{
    Position = Vertex->GetPosition();
    Radius = Vertex->GetRadius();
}

const FVector&
FArianeKeyedVertex::GetPosition() const
{
    return Position;
}

const FGuid&
FArianeKeyedVertex::GetGuid() const
{
    return Guid;
}

double
FArianeKeyedVertex::GetRadius() const
{
    return Radius;
}
