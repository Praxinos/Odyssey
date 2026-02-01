// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianePoint.h"

FArianePoint::~FArianePoint()
{
}

FArianePoint::FArianePoint( double X, double Y, double Z )
    : Position( X, Y, Z )
{
}

FArianePoint::FArianePoint( const FVector& InPosition )
    : Position( InPosition )
{
}

const FVector&
FArianePoint::GetPosition()
{
    return Position;
}
