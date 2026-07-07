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
    , AllocationModel( EArianeAllocationModel::InstancedStruct )
{
}

FArianeVertex::FArianeVertex( FArianeObject* Owner
                            , const FVector& iPosition
                            , const FVector& InNormal
                            , double InRadius
                            , EArianeAllocationModel InAllocationModel )
    : FArianePoint( iPosition )
    , Guid( FGuid::NewGuid() )
    , OwnerID( Owner )
    , Radius( InRadius )
    , Normal ( InNormal )
    , bHandleAligned( true )
    , AllocationModel( InAllocationModel )
    , bChained( false )
{
}

EArianeAllocationModel
FArianeVertex::GetAllocationModel()
{
    return AllocationModel;
}

bool
FArianeVertex::HasBaseClass( uint32 BaseClassID )
{
    if( StaticClass() == BaseClassID )
    {
        return true;
    }

    return Super::HasBaseClass( BaseClassID );
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

    Invalidate();
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

bool
FArianeVertex::IsLocked()
{
    return false;
}

bool
FArianeVertex::IsSelected()
{
    return false;
}

void
FArianeVertex::SetPosition_Private( const FVector& InPosition )
{
    Super::SetPosition_Private( InPosition );

    Invalidate();
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

int32
FArianeVertex::GetIndex( FArianeSegment* Segment )
{
    if( this == Segment->GetVertex( 0 ) )
        return 0;

    if( this == Segment->GetVertex( 1 ) )
        return 1;

    return -1;
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

//static
void
FArianeVertex::ArrayToSegmentArray( const TArray<FArianeVertex*>& InVertices
                                  , TArray<FArianeSegment*>& OutSegments
                                  , bool bEmptyFirst )
{
    if( bEmptyFirst )
    {
        OutSegments.Empty();
    }

    OutSegments.Reserve( OutSegments.Max() + InVertices.Num() );

    for( int i = 0; i < InVertices.Num(); i++ )
    {
        FArianeVertex* Vertex = InVertices[i];

        for( const FArianeSegmentID& SegmentID : Vertex->GetSegments() )
        {
            FArianeSegment* Segment = const_cast<FArianeSegmentID&>(SegmentID).GetSegment();

            if( OutSegments.Find( Segment ) == INDEX_NONE )
            {
                OutSegments.Add( Segment );
            }
        }
    }
}
