// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeHandleSegment.h"

FArianeHandleSegment::~FArianeHandleSegment()
{

}

FArianeHandleSegment::FArianeHandleSegment( FArianeSegment* iOwnerSegment, double iX, double iY, double iZ )
    : OwnerSegment ( iOwnerSegment )
    , Position( iX, iY, iZ )
{

}

FArianeHandleSegment::FArianeHandleSegment( FArianeSegment* iOwnerSegment, const FVector& iPosition )
    // Delegating constructor
    : FArianeHandleSegment ( iOwnerSegment, iPosition.X, iPosition.Y, iPosition.Z )
{
}

void
FArianeHandleSegment::SetPosition( double iX, double iY, double iZ )
{
}

void
FArianeHandleSegment::SetPosition( const FVector& iPosition )
{
}

const FVector&
FArianeHandleSegment::GetPosition()
{
    return Position;
}
