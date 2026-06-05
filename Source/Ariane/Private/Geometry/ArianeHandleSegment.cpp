// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeHandleSegment.h"
#include "ArianeSegment.h"

FArianeHandleSegment::~FArianeHandleSegment()
{
}

FArianeHandleSegment::FArianeHandleSegment( FArianeSegment* iOwnerSegment, double iX, double iY, double iZ )
    : FArianePoint( iX, iY, iZ )
    , OwnerSegment ( iOwnerSegment )
{
}

FArianeHandleSegment::FArianeHandleSegment( FArianeSegment* iOwnerSegment, const FVector& iPosition )
    // Delegating constructor
    : FArianeHandleSegment ( iOwnerSegment, iPosition.X, iPosition.Y, iPosition.Z )
{
}

void
FArianeHandleSegment::SetPosition( double InX, double InY, double InZ )
{
    Super::SetPosition( InX, InY, InZ );

    OwnerSegment->Invalidate();
}

void
FArianeHandleSegment::SetPosition( const FVector& iPosition )
{
    Super::SetPosition( iPosition );

    OwnerSegment->Invalidate();
}

const FVector&
FArianeHandleSegment::GetPosition()
{
    return Position;
}

void
FArianeHandleSegment::SetOwnerSegment( FArianeSegment* InOwnerSegment )
{
    OwnerSegment = InOwnerSegment;
}
