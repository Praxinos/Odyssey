// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeVertex.h"
#include "ArianePath.h"
#include "ArianeSegment.h"
#include "ArianePainting3DComponent.h"

FArianeVertex::~FArianeVertex()
{
}

FArianeVertex::FArianeVertex()
    : Radius( 0.0f )
    , Normal ( FVector::Zero() )
{
}

FArianeVertex::FArianeVertex( FArianeObject* Owner, const FVector& iPosition, const FVector& InNormal, double InRadius )
    : FArianePoint( iPosition )
    , Guid( FGuid::NewGuid() )
    , OwnerID( Owner )
    , Radius( InRadius )
    , Normal ( InNormal )
    , bChained( false )
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
FArianeVertex::InvalidateSegments()
{
    for( FArianeSegment* Segment : Segments )
    {
        Segment->Invalidate();
    }

    // TODO: invalidate the owner
}

void
FArianeVertex::Invalidate()
{
    InvalidateSegments();
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

void
FArianeVertex::SetRadius( double InRadius )
{
    Radius = InRadius;
}

double
FArianeVertex::GetRadius()
{
    return Radius;
}

const FGuid&
FArianeVertex::GetGuid()
{
    return Guid;
}

FArianeSegment*
FArianeVertex::GetOtherSegment( FArianeSegment* Segment )
{
    for( FArianeSegment* otherSegment : Segments )
    {
        if( otherSegment != Segment )
        {
            return otherSegment;
        }
    }

    return nullptr;
}

FArianeSegment*
FArianeVertex::GetFirstSegment()
{
    return Segments.Num() ? Segments.Last() : nullptr;
}

uint32
FArianeVertex::GetIndex( FArianeSegment* Segment )
{
    return ( this == Segment->GetVertex( 0 ) ) ? 0 : 1;
}

void
FArianeVertex::SetID( uint32 InID )
{
    ID = InID;
}

uint32
FArianeVertex::GetID()
{
    return ID;
}
